#pragma once

#include <stdint.h>

namespace internal
{

struct stringtable_entry
{
	uint32_t ptr;
	int unk_0;
	int unk_1;
	uint32_t reference_count;
};



}