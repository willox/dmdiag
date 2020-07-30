#include "elf.h"

namespace dm
{

ELF_Provider ELF_Provider::Parse(const char* path)
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


	return {std::move(buffer)};
}

ELF_Provider::ELF_Provider(std::vector<std::byte> in_buffer)
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
std::byte* ELF_Provider::Translate(uint32_t pointer, uint32_t size)
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

std::byte* ELF_Provider::Read(uint32_t offset, uint32_t size)
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
std::optional<std::vector<std::pair<uint32_t, uint32_t>>> ELF_Provider::FindRegions()
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

}