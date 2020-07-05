#include <Windows.h>
#include "internal_types.h"
#include "elf.h"

static const char dump_path[] = "C:\\Users\\wwall\\Downloads\\Dump\\core.80099";

static const char* Scan(const char* base, size_t length, const char* signature, size_t offset)
{
	size_t loc = 0;

	// i'm pretty sure this could miss a result in certain cases
	for (const char* ptr = base; ptr < base + length; ptr++) {
		if (signature[loc] == '?' || signature[loc] == *ptr) {
			if (signature[loc + 1] == '\0') {
				return ptr - loc + offset;
			}

			loc++;
		} else {
			loc = 0;
		}
	}

	return nullptr;
}

int main(int argc, char** argv)
{
	ELF_File fh = ELF_File::Parse(dump_path);

	auto ranges = fh.FindRegions();

	for (auto range : *ranges)
	{
		size_t length = range.second - range.first;
		char* begin = reinterpret_cast<char*>(fh.Translate(range.first, length));

		const char* result = Scan(begin, length, "\x55\x89\xe5\x83\xec?\x8b\x45?\x39\x05????\x76?\x8b\x15????\x8b\x04?\x85\xc0", 19);

		if (result)
		{
			std::vector<const char*> strings;

			uint32_t stringtable_ptr_ptr_ptr = *reinterpret_cast<const uint32_t*>(result);
			uint32_t strings_count = *reinterpret_cast<uint32_t*>(fh.Translate(stringtable_ptr_ptr_ptr + 4, 4));

			uint32_t stringtable_ptr_array = *reinterpret_cast<uint32_t*>(fh.Translate(stringtable_ptr_ptr_ptr, 4));

			for (uint32_t i = 0; i < strings_count; i++)
			{
				uint32_t stringtable_entry = *reinterpret_cast<uint32_t*>(fh.Translate(stringtable_ptr_array + i * 4, 4));
				dm::stringtable_entry* entry = reinterpret_cast<dm::stringtable_entry*>(fh.Translate(stringtable_entry, sizeof(stringtable_entry)));

				strings.push_back(reinterpret_cast<const char*>(fh.Translate(entry->ptr, 2)));
			}

			break;
		}
	}

	auto test = fh.Translate(0xf7952010, 4);

	return 0;
}