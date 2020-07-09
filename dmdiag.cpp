#include <Windows.h>
#include "internal_types.h"
#include "elf.h"

ELF_File* current_elf = nullptr;

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
	current_elf = &fh;


	auto* result = fh.Scan<VPtr<dm::StringTable>>("\x55\x89\xe5\x83\xec?\x8b\x45?\x39\x05????\x76?\x8b\x15????\x8b\x04?\x85\xc0", 19);


	std::vector<const char*> strings;
	if (result)
	{
		

		dm::StringTable& string_table = **result;

		for (uint32_t i = 0; i < string_table.size; i++)
		{
			VPtr<dm::String> entry = string_table.strings[i];

			if (strcmp("gaslist", entry->data.get()) == 0)
			{
				const char* x = entry->data.get();
				continue;
			}

			strings.push_back(entry->data.get());
		}
	}

	auto* current_execution_context = fh.Scan<VPtr<VPtr<dm::ExecutionContext>>>("\x8B\x0D????\x8B\x09\x8B\x51?\x8B\x41?\x8B\x8D????\x89\x7C\x24?", 2);
	dm::ExecutionContext& context = ***current_execution_context;

	auto* some_globals_ptr = fh.Scan<VPtr<VPtr<dm::SomeGlobals>>>("\xA1????\x89\x54\x24?\x8B\x40?\x89\x04\x24\xE8????\x8B\x8D????", 1);
	dm::SomeGlobals& some_globals = ***some_globals_ptr;

	auto* mob_table_ptr = fh.Scan<VPtr<dm::MobTable>>("\xA1????\x8B\x0C?\x85\xC9\x0F\x85????\x31\xC0\xE9?????\x66\x3B\x71?", 1);

	if (mob_table_ptr)
	{
		dm::MobTable& mob_table = **mob_table_ptr;

		for (uint32_t i = 0; i < mob_table.length; i++)
		{
			if (mob_table.elements[i] == nullptr)
				continue;

			dm::Mob& mob = *mob_table.elements[i];

			auto* data= some_globals.mob_fields[mob.mob_fields_index].get();

			continue;
			/*
			dm::Variable* vars = mob.variables.elements.get();

			for (uint32_t j = 0; j < mob.variables.count; j++)
			{
				dm::Variable& var = vars[j];

				const char* name = strings[var.name_index];
				continue;
			}
			*/
		}
	}

	return 0;
}