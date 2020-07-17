#include <Windows.h>
#include "dm/internal_types.h"
#include "dm/State.h"

static const char dump_path[] = "C:\\Users\\wwall\\Downloads\\Dump\\core.80099";

int main(int argc, char** argv)
{
	dm::State state(dump_path);

	// TODO: remove
	dm::SetupMobFieldGetters();

	for (uint32_t i = 0; i < 100; i++)
	{
		auto* mob = state.GetMob(i);
		auto name = mob->GetField(*state.GetStringRef("name"));
		auto desc = mob->GetField(*state.GetStringRef("desc"));
		auto layer = mob->GetField(*state.GetStringRef("layer"));
		auto stat = mob->GetField(*state.GetStringRef("stat"));

		continue;
	}

	return 0;
}