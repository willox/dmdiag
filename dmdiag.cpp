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

		auto* mobtype = mob->mob_type.get();
		auto* obj_path = mobtype->obj_path.get();
		auto* initial_vars = obj_path->initial_vars.get();

		for (uint32_t var = 0; var < initial_vars->initial_variable_table.count; var++)
		{
			auto& x = initial_vars->initial_variable_table.variables[var];

			const char* name = x.name.string();
			dm::Value& val = x.value;

			continue;
		}

		continue;
	}

	return 0;
}