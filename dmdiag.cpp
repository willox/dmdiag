#include "dm/dm.h"
#include <iostream>
#include <string.h>
#include <string>

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		std::cout << "pass path to Linux 513.1257 dump as arg 1\n";
		return 0;
	}

	dm::State state(argv[1]);

	// TODO: remove
	dm::SetupMobFieldGetters();

	if (!(state._sleeper_buffer && state._sleeper_front && state._sleeper_back))
	{
		std::cout << "Couldn't find sleeper buffer (not a 153.1257 linux dump?)";
		return 0;
	}

	std::cout << "Dumping " << (state._sleeper_back - state._sleeper_front) << " sleeping proc traces (remember to read them backwards)" << '\n';

	for (uint32_t idx = *state._sleeper_front; idx < *state._sleeper_back; idx++)
	{
		std::cout << '\n' << "Stack #" << idx << '\n';

		auto* instance = state._sleeper_buffer[idx].get();
		if (!instance)
		{
			std::cout << "error: Couldn't read stack" << std::endl;
			continue;
		}

		auto* context = instance->context.get();
		if (context)
		{
			uint32_t frame = 0;
			dm::ExecutionContext* current = context;
			while (current)
			{
				const char* pName = current->proc_instance->proc.get()->path.string();
				if (strlen(pName) == 0)
					pName = "<unnamed>";

				const char* pFile = current->dbg_proc_file.string();
				if (strlen(pFile) == 0)
					pFile = "<none>";

				uint32_t line = current->dbg_current_line;

				std::cout << '#' << frame << "  " << pName << " @ " << pFile << ':' << line << '\n';

				frame++;
				current = current->parent_context.get();
			}
		}
		else
		{
			std::cout << "error: Couldn't read proc context" << std::endl;
		}
	}

	return 0;
}