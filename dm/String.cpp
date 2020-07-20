#include "String.h"
#include "State.h"

namespace dm
{

// TODO: Replace with get() implementation
template<>
const char* Ref<String>::string()
{
	return current_state->GetString(index);
};

// TODO: Replace with get() implementation
template<>
const char* Ref<VarName>::string()
{
	return current_state->GetVarName(index);
};

}