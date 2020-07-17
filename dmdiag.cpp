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
		dm::Ref<dm::Mob> mob{i};

		auto name = dm::Mob::GetField(mob, *state.GetStringRef("name"));
		auto desc = dm::Mob::GetField(mob, *state.GetStringRef("desc"));
		auto layer = dm::Mob::GetField(mob, *state.GetStringRef("layer"));
		auto stat = dm::Mob::GetField(mob, *state.GetStringRef("stat"));

		continue;
	}

	return 0;
}