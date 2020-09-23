#include "String.h"
#include "State.h"

namespace dm
{

template<>
String* Ref<String>::get()
{
	return current_state->_string_table->strings[index].get();
}

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