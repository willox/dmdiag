#pragma once

#include "internal_types.h"
#include "elf.h"
#include <vector>
#include <map>

class ELF_File;

namespace dm
{

class State;
extern State* current_state;

class State
{
public:
	State(const char* path);

	const char* GetString(uint32_t index);
	Ref<String>* GetStringRef(const char* str);
	Mob* GetMob(uint32_t index);
	MobType* GetMobType(uint32_t index);
	Ref<MobType> FindMobType(Ref<ObjPath> path);
	const char* GetVarName(uint32_t index);
	Value& GetValue(uint32_t index);
	ObjPath* GetObjPath(uint32_t index);
	Misc* GetMisc(uint32_t index);
	MobFields* GetMobFields(uint32_t index);

	std::byte* Translate(uint32_t pointer, uint32_t size);

private:
	void LoadStrings();

public:
	ELF_File _file;
	StringTable* _string_table;
	ExecutionContext* _current_execution_context;
	MobTable* _mob_table;
	MobTypeTable* _mobtype_table;
	Ref<String>* _varname_table;
	Value* _value_table;
	ObjPathTable* _obj_path_table;
	MiscTable* _misc_table;
	SomeGlobals* _someglobals;

	std::vector<const char*> _strings;
	std::map<std::string, Ref<String>> _string_refs;
};

}