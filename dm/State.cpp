#include "State.h"
#include "elf.h"

namespace dm
{

State* current_state = nullptr;

State::State(const char* path)
	: _file(ELF_File::Parse(path))
	, _string_table(nullptr)
	, _current_execution_context(nullptr)
	, _mob_table(nullptr)
{
	current_state = this;

	// TODO: Debug output when failing to read certain values

	auto* pStringTable = _file.Scan<VPtr<dm::StringTable>>("\x55\x89\xe5\x83\xec?\x8b\x45?\x39\x05????\x76?\x8b\x15????\x8b\x04?\x85\xc0", 19);
	auto* ppCurrentExecutionContext = _file.Scan<VPtr<VPtr<dm::ExecutionContext>>>("\x8B\x0D????\x8B\x09\x8B\x51?\x8B\x41?\x8B\x8D????\x89\x7C\x24?", 2);
	auto* pMobTable = _file.Scan<VPtr<MobTable>>("\xA1????\x8B\x0C?\x85\xC9\x0F\x85????\x31\xC0\xE9?????\x66\x3B\x71?", 1);
	auto* pMobTypeTable = _file.Scan<VPtr<MobTypeTable>>("\x3B\x15????\x0F\x83????\xC1\xE2\x04\x03\x15????\x0F\x84????", 17);
	auto* ppNamesTable = _file.Scan<VPtr<VPtr<Ref<String>>>>("\xA1????\x8B\x04?\x89\x34\x24\x89\x44\x24?\xE8????\x8B\x55?\x8B\x45?\x83\xEC\x04\x89\x54\x24?\x8B\x55?\x89\x44\x24?\x8B\x45?\x89\x54\x24?\x8B\x55?\x89\x04\x24\x89\x54\x24?\xE8????\x66\x3B\x3B", 1);
	auto* ppGlobalValuesTable = _file.Scan<VPtr<VPtr<Value>>>("\x03\x35????\x89\x44\x24?\x89\x54\x24?\x89\x34\x24\xE8????\xB8\x01\x00\x00\x00", 2);
	auto* pObjPathTable = _file.Scan<VPtr<ObjPathTable>>("\x8B\x1D????\x01\xD9\x89\x4D?\x8B\x0D????\x8D\x04?\x89\x45?\x8B\x5D?", 2);
	auto* pMiscTable = _file.Scan<VPtr<MiscTable>>("\x8B\x15????\x8B\x04?\x85\xC0\x0F\x84????\x8B\x78?\x0F\xB7\x00\x66\x85\xC0", 2);

	if (pStringTable)
	{
		_string_table = pStringTable->get();
		LoadStrings();
	}
	
	if (ppCurrentExecutionContext)
	{
		_current_execution_context = (*ppCurrentExecutionContext)->get();
	}

	if (pMobTable)
	{
		_mob_table = pMobTable->get();
	}

	if (pMobTypeTable)
	{
		_mobtype_table = pMobTypeTable->get();
	}

	if (ppNamesTable)
	{
		_names_table = (*ppNamesTable)->get();
	}

	if (ppGlobalValuesTable)
	{
		_global_values_table = (*ppGlobalValuesTable)->get();
	}

	if (pObjPathTable)
	{
		_obj_path_table = pObjPathTable->get();
	}

	if (pMiscTable)
	{
		_misc_table = pMiscTable->get();
	}
}

void State::LoadStrings()
{
	// TODO: Verify that strings don't run out of memory bounds
	_strings.resize(_string_table->size);

	for (uint32_t i = 0; i < _string_table->size; i++)
	{
		VPtr<dm::String> entry = _string_table->strings[i];
		_strings[i] = entry->data.get();
	}
}

const char* State::GetString(uint32_t index)
{
	if (index >= _strings.size())
	{
		return nullptr;
	}

	return _strings[index];
}

Mob* State::GetMob(uint32_t index)
{
	if (index >= _mob_table->length)
	{
		return nullptr;
	}

	return _mob_table->elements[index].get();
}

MobType* State::GetMobType(uint32_t index)
{
	if (index >= _mobtype_table->count)
	{
		return nullptr;
	}

	return &_mobtype_table->elements[index];
}

ObjPath* State::GetObjPath(uint32_t index)
{
	if (index >= _obj_path_table->count)
	{
		return nullptr;
	}

	return &_obj_path_table->elements[index];
}

Misc* State::GetMisc(uint32_t index)
{
	if (index >= _misc_table->count)
	{
		return nullptr;
	}

	VPtr<Misc> pMisc = _misc_table->elements[index];

	if (!pMisc)
	{
		return nullptr;
	}

	return pMisc.get();
}

std::byte* State::Translate(uint32_t pointer, uint32_t size)
{
	return _file.Translate(pointer, size);
}

}