#pragma once

#include <stdint.h>
#include <string_view>
#include <fstream>
#include <vector>
#include <valarray>
#include <algorithm>
#include <optional>
#include <memory>
#include <string_view>

// Mmmm

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
	static ELF_File Parse(const char* path);

public:
	ELF_File(std::vector<std::byte> in_buffer);

	std::byte* Translate(uint32_t pointer, uint32_t size);

	std::byte* Read(uint32_t offset, uint32_t size);

	std::optional<std::vector<std::pair<uint32_t, uint32_t>>> FindRegions();

	template<typename T>
	T* Scan(std::string_view signature, size_t offset)
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
					if (loc + 1 >= signature.size()) {
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
