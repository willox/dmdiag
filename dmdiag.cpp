#include <Windows.h>
#include "internal_types.h"
#include "elf.h"

static const char dump_path[] = "C:\\Users\\wwall\\Downloads\\Dump\\core.80099";

int main(int argc, char** argv)
{
	ELF_File::Parse(dump_path);

	return 0;
}