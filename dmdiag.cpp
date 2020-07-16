#include <Windows.h>
#include "dm/internal_types.h"
#include "dm/State.h"

static const char dump_path[] = "C:\\Users\\wwall\\Downloads\\Dump\\core.80099";

int main(int argc, char** argv)
{
	dm::State state(dump_path);

	for (uint32_t i = 0; i < 100; i++)
	{
		auto* mob = state.GetMob(i);
		auto* path = mob->path.get();
		dm::Ref<dm::String>* str = state.GetStringRef("name");
		if (str == nullptr)
			continue;

		auto var = mob->GetField(*str);
		if (var)
		{
			continue;
		}
	}

	return 0;
}