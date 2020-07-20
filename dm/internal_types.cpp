#include "internal_types.h"
#include "State.h"

namespace dm
{
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
Misc* Ref<Misc>::get()
{
	return current_state->GetMisc(index);
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

