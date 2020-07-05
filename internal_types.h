#pragma once

#include <stdint.h>

namespace dm
{

struct stringtable_entry
{
	uint32_t ptr;
	int unk_0;
	int unk_1;
	uint32_t reference_count;
	int unk_2;
	int unk_3;
	int unk_4;
};



}