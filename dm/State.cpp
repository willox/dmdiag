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

	using namespace std::literals;
	auto* pStringTable = _file.Scan<VPtr<dm::StringTable>>("\x55\x89\xe5\x83\xec?\x8b\x45?\x39\x05????\x76?\x8b\x15????\x8b\x04?\x85\xc0"sv, 19);
	auto* ppCurrentExecutionContext = _file.Scan<VPtr<VPtr<dm::ExecutionContext>>>("\x8B\x0D????\x8B\x09\x8B\x51?\x8B\x41?\x8B\x8D????\x89\x7C\x24?"sv, 2);
	auto* pMobTable = _file.Scan<VPtr<MobTable>>("\xA1????\x8B\x0C?\x85\xC9\x0F\x85????\x31\xC0\xE9?????\x66\x3B\x71?"sv, 1);
	auto* pMobTypeTable = _file.Scan<VPtr<MobTypeTable>>("\x3B\x15????\x0F\x83????\xC1\xE2\x04\x03\x15????\x0F\x84????"sv, 17);
	auto* ppVarNameTable = _file.Scan<VPtr<VPtr<Ref<String>>>>("\xA1????\x8B\x04?\x89\x34\x24\x89\x44\x24?\xE8????\x8B\x55?\x8B\x45?\x83\xEC\x04\x89\x54\x24?\x8B\x55?\x89\x44\x24?\x8B\x45?\x89\x54\x24?\x8B\x55?\x89\x04\x24\x89\x54\x24?\xE8????\x66\x3B\x3B"sv, 1);
	auto* ppValueTable = _file.Scan<VPtr<VPtr<Value>>>("\x03\x35????\x89\x44\x24?\x89\x54\x24?\x89\x34\x24\xE8????\xB8\x01\x00\x00\x00"sv, 2);
	auto* pObjPathTable = _file.Scan<VPtr<ObjPathTable>>("\x8B\x1D????\x01\xD9\x89\x4D?\x8B\x0D????\x8D\x04?\x89\x45?\x8B\x5D?"sv, 2);
	auto* pMiscTable = _file.Scan<VPtr<MiscTable>>("\x8B\x15????\x8B\x04?\x85\xC0\x0F\x84????\x8B\x78?\x0F\xB7\x00\x66\x85\xC0"sv, 2);
	auto* ppSomeGlobals = _file.Scan<VPtr<VPtr<SomeGlobals>>>("\xA1????\x85\xC0\x0F\x84????\x8B\x40?\x85\xC0\x0F\x84????"sv, 1);
	auto* ppSleeperBuffer = _file.Scan<VPtr<VPtr<VPtr<ProcInstance>>>>("\xA1????\xC7\x44\x24?\x26\x00\x00\x00\x8B\x04?\x8B\x00\x89\x34\x24\x89\x44\x24?"sv, 1);
	auto* pProcTable = _file.Scan<VPtr<ProcTable>>("\x03\x05????\x8B\x4D?\x8B\x40?\x89\x45?\x0F\xB7\x59?\x8B\x31\x0F\xB7\xCB"sv, 2);

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

	if (ppVarNameTable)
	{
		_varname_table = (*ppVarNameTable)->get();
	}

	if (ppValueTable)
	{
		_value_table = (*ppValueTable)->get();
	}

	if (pObjPathTable)
	{
		_obj_path_table = pObjPathTable->get();
	}

	if (pMiscTable)
	{
		_misc_table = pMiscTable->get();
	}

	if (ppSomeGlobals)
	{
		_someglobals = (*ppSomeGlobals)->get();
	}

	if (ppSleeperBuffer)
	{
		_sleeper_buffer = (*ppSleeperBuffer)->get();
	}

	if (pProcTable)
	{
		_proc_table = pProcTable->get();
	}

	return;
}

void State::LoadStrings()
{
	// TODO: Verify that strings don't run out of memory bounds
	_strings.resize(_string_table->size);

	for (uint32_t i = 0; i < _string_table->size; i++)
	{
		VPtr<dm::String> entry = _string_table->strings[i];
		_strings[i] = entry->data.get();
		_string_refs[entry->data.get()] = Ref<String>{i};
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

Ref<String>* State::GetStringRef(const char* string)
{
	auto& x = _string_refs.find({string});
	if (x != _string_refs.end())
	{
		return &x->second;
	}

	return nullptr;
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

Ref<MobType> State::FindMobType(Ref<ObjPath> path)
{
	uint32_t count = _mobtype_table->count;
	MobType* elements = _mobtype_table->elements.get();

	for (uint32_t i = 0; i < count; i++)
	{
		if (elements[i].obj_path != path)
			continue;

		return Ref<MobType>{i};
	}

	return Ref<MobType>::Invalid();
}

const char* State::GetVarName(uint32_t index)
{
/*
	if (index >= _varname_table->count)
	{
		return nullptr;
	}
*/

	return _varname_table[index].string();
}

Value& State::GetValue(uint32_t index)
{
	/*
	if (index >= _value_table->count)
	{
	return nullptr;
	}
	*/

	return _value_table[index];
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

MobFields* State::GetMobFields(uint32_t index)
{
	if (index >= _someglobals->mob_fields_count)
	{
		return nullptr;
	}

	VPtr<MobFields> pFields = _someglobals->mob_fields[index];
	return pFields.get();
}

Proc* State::GetProc(uint32_t index)
{
	if (index >= _proc_table->count)
	{
		return nullptr;
	}

	Proc* pProcs = _proc_table->elements.get();
	return &pProcs[index];
}

std::byte* State::Translate(uint32_t pointer, uint32_t size)
{
	return _file.Translate(pointer, size);
}

}