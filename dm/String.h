#pragma once

#include "internal_types.h"

namespace dm
{

struct String
{
	VPtr<char> data;
	Ref<String> self;
	VPtr<String> left;
	VPtr<String> right;
	uint32_t unk_2;
	uint32_t unk_3;
	uint32_t unk_4;
};
static_assert(sizeof(String) == 28);

struct VarName : public String
{};
static_assert(sizeof(VarName) == sizeof(String));

struct StringTable
{
	VPtr<VPtr<String>> strings;
	uint32_t size;
};

static_assert(sizeof(StringTable) == 8);

}