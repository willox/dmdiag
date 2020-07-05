#pragma once

#include <stdint.h>
#include <string_view>
#include <fstream>
#include <vector>
#include <valarray>

// Mmmm

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

	/*
	* Format of NT_FILE note:
	*
	* long count     -- how many files are mapped
	* long page_size -- units for file_ofs
	* array of [COUNT] elements of
	*   long start
	*   long end
	*   long file_ofs
	* followed by COUNT filenames in ASCII: "FILE1" NUL "FILE2" NUL...
	*/

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

		for (size_t i = 0; i < header->programheader_count; i++)
		{
			std::byte* ptr = Read(header->programheader_offset + header->programheader_entry_size * i, header->programheader_entry_size);
			program_headers.push_back(reinterpret_cast<ProgramHeader*>(ptr));
		}

		for (size_t i = 0; i < header->sectionheader_count; i++)
		{
			std::byte* ptr = Read(header->sectionheader_offset + header->sectionheader_entry_size * i, header->sectionheader_entry_size);
			section_headers.push_back(reinterpret_cast<SectionHeader*>(ptr));
		}

		FindRanges();

		return;
	}

	std::byte* Read(size_t offset, size_t size)
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

	void /*std::vector<std::pair<size_t, size_t>>*/ FindRanges()
	{
		for (auto& ph : program_headers)
		{
			if (ph->type != 0x00000004) // note
				continue;

			std::byte* data = Read(ph->offset, ph->file_size);

			break;
		}
	}

private:
	std::vector<std::byte> buffer;

	Header* header;
	std::vector<ProgramHeader*> program_headers;
	std::vector<SectionHeader*> section_headers;
};
