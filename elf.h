#pragma once

#include <stdint.h>
#include <string_view>
#include <fstream>
#include <vector>
#include <valarray>
#include <algorithm>
#include <optional>

// Mmmm

class ELF_File;
extern ELF_File* current_elf;

template<typename T>
class VPtr;

class ELF_File
{
private:
	struct Header
	{
		uint8_t magic[4];
		uint8_t ELF_class;
		uint8_t endianness;
		uint8_t ELF_version;
		uint8_t abi;
		uint8_t abi_2;
		uint8_t padding[7];
		uint16_t type;
		uint16_t machine;
		uint32_t ELF_version_again;
		uint32_t entry_offset;
		uint32_t programheader_offset;
		uint32_t sectionheader_offset;
		uint32_t flags;
		uint16_t header_size;
		uint16_t programheader_entry_size;
		uint16_t programheader_count;
		uint16_t sectionheader_entry_size;
		uint16_t sectionheader_count;
		uint16_t sectionheader_names_index;
	};

	struct ProgramHeader
	{
		uint32_t type;
		uint32_t offset;
		uint32_t virtual_address;
		uint32_t phyisical_address;
		uint32_t file_size;
		uint32_t memory_size;
		uint32_t flags;
		uint32_t alignment;
	};

	struct SectionHeader
	{
		uint32_t name;
		uint32_t type;
		uint32_t flags;
		uint32_t virtual_address;
		uint32_t offset;
		uint32_t size;
		uint32_t link;
		uint32_t info;
		uint32_t alignment;
		uint32_t entry_size;
	};

	struct NoteHeader
	{
		uint32_t name_size;
		uint32_t content_size;
		uint32_t type;
	};

	struct NT_File_Header
	{
		uint32_t count;
		uint32_t page_size;
	};

	struct NT_File_Element
	{
		uint32_t start;
		uint32_t end;
		uint32_t offset;
	};

public:
	static ELF_File Parse(const char* path)
	{
		std::ifstream stream(path, std::ios::binary | std::ios::ate);
		auto size = stream.tellg();
		stream.seekg(0, std::ios::beg);

		std::vector<std::byte> buffer(size);
		if (!stream.read(reinterpret_cast<char*>( buffer.data() ), size))
		{
			throw;
		}

		if (buffer.size() < sizeof(Header))
		{
			throw;
		}

		std::byte* base = buffer.data();
		std::byte* end = base + buffer.size();

		Header* elf_header = reinterpret_cast<Header*>(base);
		ProgramHeader* program_headers = reinterpret_cast<ProgramHeader*>(base + elf_header->programheader_offset);
		SectionHeader* section_headers = reinterpret_cast<SectionHeader*>(base + elf_header->sectionheader_offset);


		return ELF_File {std::move(buffer)};
	}

public:
	ELF_File(std::vector<std::byte> in_buffer)
		: buffer(std::move(in_buffer))
	{
		std::byte* base = buffer.data();
		std::byte* end = base + buffer.size();

		header = reinterpret_cast<Header*>(base);

		if (header->programheader_entry_size < sizeof(ProgramHeader))
		{
			throw;
		}

		if (header->sectionheader_entry_size < sizeof(SectionHeader))
		{
			throw;
		}

		for (uint32_t i = 0; i < header->programheader_count; i++)
		{
			std::byte* ptr = Read(header->programheader_offset + header->programheader_entry_size * i, header->programheader_entry_size);
			program_headers.push_back(reinterpret_cast<ProgramHeader*>(ptr));
		}

		for (uint32_t i = 0; i < header->sectionheader_count; i++)
		{
			std::byte* ptr = Read(header->sectionheader_offset + header->sectionheader_entry_size * i, header->sectionheader_entry_size);
			section_headers.push_back(reinterpret_cast<SectionHeader*>(ptr));
		}

		auto found_regions = FindRegions();
		if (found_regions)
		{
			regions = *found_regions;
		}

		return;
	}

	// Easy optimizations here
	std::byte* Translate(uint32_t pointer, uint32_t size)
	{
		if (pointer + size < pointer)
			return nullptr;

		for (auto& ph : program_headers)
		{
			if (ph->virtual_address + ph->file_size < ph->virtual_address)
				continue;

			if (pointer < ph->virtual_address || (pointer + size) > (ph->virtual_address + ph->file_size))
				continue;

			uint32_t data_offset = pointer - ph->virtual_address;

			if (ph->offset + data_offset < ph->offset)
				continue;

			uint32_t begin = ph->offset + data_offset;
			uint32_t end = begin + size;

			if (end < begin)
				continue;

			if (begin >= buffer.size() || end >= buffer.size())
				continue;
			
			return &buffer[begin];
		}

		return nullptr;
	}

	std::byte* Read(uint32_t offset, uint32_t size)
	{
		std::byte* base = buffer.data();
		size_t buffer_size = buffer.size();

		if (offset >= buffer_size ||
			offset + size < offset ||
			offset + size > buffer_size)
		{
			return nullptr;
		}

		return base + offset;
	}

	// fixups needed here
	std::optional<std::vector<std::pair<uint32_t, uint32_t>>> FindRegions()
	{
		for (auto& ph : program_headers)
		{
			if (ph->type != 0x00000004) // note
			{
				continue;
			}

			uint32_t begin = ph->offset;
			uint32_t end = ph->offset + ph->file_size;

			if (end <= begin)
			{
				break;
			}

			uint32_t current = begin;
			while (current <= end)
			{
				NoteHeader* note_header = reinterpret_cast<NoteHeader*>(Read(current, sizeof(NoteHeader)));

				uint32_t name_size_aligned = note_header->name_size;
				if (name_size_aligned % 4 != 0)
					name_size_aligned += 4 - name_size_aligned % 4;

				uint32_t content_size_aligned = note_header->content_size;
				if (content_size_aligned % 4 != 0)
					content_size_aligned += 4 - content_size_aligned % 4;

				if (note_header->type == 0x46494c45)
				{
					if (note_header->content_size < sizeof(NT_File_Header))
					{
						throw;
					}

					std::byte* note_data = Read(current + sizeof(NoteHeader) + name_size_aligned, note_header->content_size);

					NT_File_Header* nt_file_header = reinterpret_cast<NT_File_Header*>(note_data);

					// Check room for elements
					if (note_header->content_size < (sizeof(NT_File_Header) + sizeof(NT_File_Element) * nt_file_header->count))
					{
						throw;
					}

					NT_File_Element* elements = reinterpret_cast<NT_File_Element*>(note_data + sizeof(NT_File_Header));

					// The rest of the buffer is our strings separated by null bytes
					char* strings_begin = reinterpret_cast<char*>(note_data + sizeof(NT_File_Header) + sizeof(NT_File_Element) * nt_file_header->count);
					char* strings_end = reinterpret_cast<char*>(note_data + note_header->content_size);
					char search[] = "libbyond.so";

					auto match = std::search(strings_begin, strings_end, std::begin(search), std::end(search));

					std::vector<std::pair<uint32_t, uint32_t>> results;

					while (match != strings_end)
					{
						uint32_t index = std::count(strings_begin, match, '\0');

						if (index >= nt_file_header->count)
						{
							throw;
						}

						results.push_back({elements[index].start, elements[index].end});
						match = std::search(match + 1, strings_end, std::begin(search), std::end(search));
					}

					return results;
				}

				current += sizeof(NoteHeader) + name_size_aligned + content_size_aligned;
			}
		}

		return std::nullopt;
	}

	template<typename T>
	T* Scan(const char* signature, size_t offset)
	{
		for (auto region : regions)
		{
			size_t length = region.second - region.first;

			char* begin = reinterpret_cast<char*>(Translate(region.first, length));
			char* end = begin + length;

			size_t loc = 0;

			// i'm pretty sure this could miss a result in certain cases
			for (char* ptr = begin; ptr < end; ptr++) {
				if (signature[loc] == '?' || signature[loc] == *ptr) {
					if (signature[loc + 1] == '\0') {
						return reinterpret_cast<T*>(ptr - loc + offset); // TODO: Check size and offset
					}

					loc++;
				} else {
					loc = 0;
				}
			}
		}

		return nullptr;
	}

private:
	std::vector<std::byte> buffer;

	Header* header;
	std::vector<ProgramHeader*> program_headers;
	std::vector<SectionHeader*> section_headers;

	std::vector<std::pair<uint32_t, uint32_t>> regions;
};

template<typename T>
class VPtr
{
public:
	VPtr(uint32_t ptr)
		: ptr(ptr)
	{}

	bool operator==(std::nullptr_t rhs)
	{
		return ptr == 0;
	}

	template<typename T2>
	bool operator==(const VPtr<T2> rhs) const
	{
		return ptr == rhs.ptr;
	}

	template<typename T2>
	bool operator!=(const VPtr<T2> rhs) const
	{
		return ptr != rhs.ptr;
	}

	T& operator*()
	{
		if (ptr == 0) // https://www.youtube.com/watch?v=bLHL75H_VEM
			throw;

		return *reinterpret_cast<T*>(current_elf->Translate(ptr, sizeof(T)));
	}

	T* operator->()
	{
		return reinterpret_cast<T*>(current_elf->Translate(ptr, sizeof(T)));
	}

	T& operator[](size_t index)
	{
		if (ptr == 0) // https://www.youtube.com/watch?v=bLHL75H_VEM
			throw;

		return *reinterpret_cast<T*>(current_elf->Translate(ptr + index * sizeof(T), sizeof(T)));
	}

	operator bool()
	{
		return ptr != 0;
	}

	T* get()
	{
		return reinterpret_cast<T*>(current_elf->Translate(ptr, sizeof(T)));
	}

private:
	uint32_t ptr;
};

static_assert(sizeof(VPtr<uint32_t>) == sizeof(uint32_t));
