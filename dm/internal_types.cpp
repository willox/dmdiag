#include "internal_types.h"
#include "State.h"

namespace dm
{

template<>
const char* Ref<String>::string()
{
	return current_state->GetString(index);
};

template<>
const char* Ref<VarName>::string()
{
	return current_state->GetVarName(index);
};

template<>
Value* Ref<Value>::get()
{
	return &current_state->GetValue(index);
}

template<>
ObjPath* Ref<ObjPath>::get()
{
	return current_state->GetObjPath(index);
};

template<>
MobType* Ref<MobType>::get()
{
	return current_state->GetMobType(index);
};

template<>
Misc* Ref<Misc>::get()
{
	return current_state->GetMisc(index);
}

// TODO: Can be a binary search
Value* VariableTable::GetField(Ref<String> key)
{
	Variable* cached_elements = elements.get();

	for (uint32_t i = 0; i < count; i++)
	{
		if (cached_elements[i].name == key)
		{
			return &cached_elements[i].value;
		}
	}

	return nullptr;
}

Value* Mob::GetField(Ref<String> name)
{
	uint32_t count = variables.count;
	Variable* vars = variables.elements.get();

	// TODO: Could use a binary search here
	for (uint32_t idx = 0; idx < count; idx++)
	{
		auto& x = vars[idx];

		if (x.name == name)
		{
			return &x.value;
		}
	}

	Value* initial = GetInitialField(name);
	if (initial != nullptr)
	{
		return initial;
	}

	Value* global = GetGlobalField(name);
	if (global != nullptr)
	{
		return global;
	}

	return nullptr;
}

Value* Mob::GetInitialField(Ref<String> name)
{
	MobType* pMobType = mob_type.get();
	if (pMobType == nullptr)
		return nullptr;

	ObjPath* pObjPath = pMobType->obj_path.get();
	if (pObjPath == nullptr)
		return nullptr;

	Misc* pInitialVars = pObjPath->initial_vars.get();
	if (pInitialVars == nullptr)
		return nullptr;

	uint32_t count = pInitialVars->initial_variable_table.count();
	InitialVariable* vars = pInitialVars->initial_variable_table.variables.get();

	// TODO: Could use a binary search here
	for (uint32_t idx = 0; idx < count; idx++)
	{
		auto& x = vars[idx];

		if (x.name == name)
		{
			return &x.value;
		}
	}

	return nullptr;
}

Value* Mob::GetGlobalField(Ref<String> name)
{
	MobType* pMobType = mob_type.get();
	if (pMobType == nullptr)
		return nullptr;

	ObjPath* pObjPath = pMobType->obj_path.get();
	if (pObjPath == nullptr)
		return nullptr;

	Misc* pVarDeclarations = pObjPath->var_declarations.get();
	if (pVarDeclarations == nullptr)
		return nullptr;

	uint32_t count = pVarDeclarations->var_declaration_table.count();
	VarDeclaration* vars = pVarDeclarations->var_declaration_table.variables.get();

	// TODO: Could use a binary search here
	for (uint32_t idx = 0; idx < count; idx++)
	{
		auto& x = vars[idx];

		if (!x.isGlobal())
			continue;

		// Dumb match against str pointers, is ok
		if (x.name.string() == name.string())
		{
			return x.value.get();
		}
	}

	return nullptr;
}

}

