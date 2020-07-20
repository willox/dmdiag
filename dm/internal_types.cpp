#include "internal_types.h"
#include "State.h"

namespace dm
{

const Value Value::Null;

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
Mob* Ref<Mob>::get()
{
	return current_state->GetMob(index);
}

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

template<>
MobFields* Ref<MobFields>::get()
{
	return current_state->GetMobFields(index);
}

template<>
Proc* Ref<Proc>::get()
{
	return current_state->GetProc(index);
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

}

