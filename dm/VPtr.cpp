#include "VPtr.h"
#include "State.h"

namespace dm
{
	std::byte* VPtr_Translate(uint32_t pointer, uint32_t size)
	{
		return current_state->Translate(pointer, size);
	}
}
