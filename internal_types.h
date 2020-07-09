#pragma once

#include <stdint.h>

#include "elf.h"

namespace dm
{

enum class DataType : uint8_t
{
	NULL_D = 0x00,
	TURF = 0x01,
	OBJ = 0x02,
	MOB = 0x03,
	AREA = 0x04,
	CLIENT = 0x05,
	STRING = 0x06,
	MOB_TYPEPATH = 0x08,
	OBJ_TYPEPATH = 0x09,
	TURF_TYPEPATH = 0x0A,
	AREA_TYPEPATH = 0x0B,
	RESOURCE = 0x0C,
	IMAGE = 0x0D,
	WORLD_D = 0x0E,
	LIST = 0x0F,
	LIST_ARGS = 0x10,
	LIST_MOB_VERBS = 0x11,
	LIST_VERBS = 0x12,
	LIST_TURF_VERBS = 0x13,
	LIST_AREA_VERBS = 0x14,
	LIST_CLIENT_VERBS = 0x15,
	LIST_SAVEFILE_DIR = 0x16,
	LIST_MOB_CONTENTS = 0x17,
	LIST_TURF_CONTENTS = 0x18,
	LIST_AREA_CONTENTS = 0x19,
	LIST_WORLD_CONTENTS = 0x1A,
	LIST_GROUP = 0x1B,
	LIST_CONTENTS = 0x1C,
	DATUM_TYPEPATH = 0x20,
	DATUM = 0x21,
	SAVEFILE = 0x23,
	SAVEFILE_TYPEPATH = 0x24,
	PROCPATH = 0x26,
	FILE_ = 0x27,
	LIST_TYPEPATH = 0x28,
	PREFAB = 0x29,
	NUMBER = 0x2A,
	LIST_MOB_VARS = 0x2C,
	LIST_OBJ_VARS = 0x2D,
	LIST_TURF_VARS = 0x2E,
	LIST_AREA_VARS = 0x2F,
	LIST_CLIENT_VARS = 0x30,
	LIST_VARS = 0x31, //maybe?
	LIST_MOB_OVERLAYS = 0x32,
	LIST_MOB_UNDERLAYS = 0x33,
	LIST_OVERLAYS = 0x34,
	LIST_UNDERLAYS = 0x35,
	LIST_TURF_OVERLAYS = 0x36,
	LIST_TURF_UNDERLAYS = 0x37,
	LIST_AREA_OVERLAYS = 0x38,
	LIST_AREA_UNDERLAYS = 0x39,
	APPEARANCE = 0x3A,
	CLIENT_TYPEPATH = 0x3B,
	IMAGE_TYPEPATH = 0x3F,
	LIST_IMAGE_OVERLAYS = 0x40,
	LIST_IMAGE_UNDERLAYS = 0x41,
	LIST_IMAGE_VARS = 0x42,
	LIST_IMAGE_VERBS = 0x43,
	LIST_IMAGE_CONTENTS = 0x44, // wait wtf
	LIST_CLIENT_IMAGES = 0x46,
	LIST_CLIENT_SCREEN = 0x47,
	LIST_TURF_VIS_CONTENTS = 0x4B,
	LIST_VIS_CONTENTS = 0x4C,
	LIST_MOB_VIS_CONTENTS = 0x4D,
	LIST_TURF_VIS_LOCS = 0x4E,
	LIST_VIS_LOCS = 0x4F,
	LIST_MOB_VIS_LOCS = 0x50,
	LIST_WORLD_VARS = 0x51,
	LIST_GLOBAL_VARS = 0x52,
	FILTERS = 0x53,
	LIST_IMAGE_VIS_CONTENTS = 0x54,
};

struct Value
{
	DataType type;
	union
	{
		uint32_t value_int;
		uint32_t value_float;
	};
};

static_assert(sizeof(Value) == 8);

enum class DefaultStrings
{
	
};

struct String
{
	VPtr<char> data;
	uint32_t unk_0;
	uint32_t unk_1;
	uint32_t reference_count;
	uint32_t unk_2;
	uint32_t unk_3;
	uint32_t unk_4;
};

static_assert(sizeof(String) == 28);

struct StringRef
{
	uint32_t string_index;
};

static_assert(sizeof(StringRef) == 4);

struct StringTable
{
	VPtr<VPtr<String>> strings;
	uint32_t size;
};

static_assert(sizeof(StringTable) == 8);

// TODO: Might actually be more specific (VariableTableEntry?)
struct Variable
{
	uint32_t unk_0;
	uint32_t name_index; // TODO: retype?
	Value value;
};

static_assert(sizeof(Variable) == 16);

struct VariableTable
{
	VPtr<Variable> elements;
	uint16_t count;
	uint16_t capacity;
};

static_assert(sizeof(VariableTable) == 8);

struct Mob
{
	char unk_0[0x24];
	VPtr<uint32_t> unk_1;
	VariableTable variables;
	char unk_2[0xC];
	VPtr<uint32_t> unk_3;
	VPtr<uint32_t> unk_4;
	VPtr<uint32_t> unk_5;
	char unk_6[0x8];
	uint32_t mob_fields_index;
	char unk_7[0x54];
	VPtr<uint32_t> unk_8;
	char unk_9[0x10];
};

static const size_t x = offsetof(Mob, mob_fields_index);

static_assert(sizeof(Mob) == 188);

// Lots of things use this structure - could abstract it?
struct MobTable
{
	VPtr<VPtr<Mob>> elements;
	uint32_t length;
};

static_assert(sizeof(MobTable) == 8);

// wtf is going on down here

struct MobFields
{
	char unk_0[0x1c];
	uint32_t name_id;
	uint32_t desc_id;
};

struct SomeGlobals
{
	char unk_0[0x40];
	VPtr<VPtr<MobFields>> mob_fields;
	uint32_t mob_fields_count;
};

struct ExecutionContext;

struct ProcConstants
{
	int proc_id;
	int unknown1;
	Value usr;
	Value src;
	union
	{
		VPtr<ExecutionContext> context;
		VPtr<ProcConstants> next; // When we're part of the global `constants_freelist` linked list, this is the next element
	};
	union
	{
		int argslist_id;
	};

	int unknown4; //some callback thing
	union
	{
		int unknown5;
		int extended_profile_id;
	};
	union
	{
		int arg_count;
	};
	VPtr<Value> args;
	char unknown6[0x58];
	int time_to_resume;
};

struct ExecutionContext
{
	VPtr<ProcConstants> constants;
	VPtr<ExecutionContext> parent_context;
	std::uint32_t dbg_proc_file;
	std::uint32_t dbg_current_line;
	VPtr<std::uint32_t> bytecode;
	std::uint16_t current_opcode;
	char test_flag;
	char unknown1;
	Value cached_datum;
	char unknown2[16];
	Value dot;
	VPtr<Value> local_variables;
	VPtr<Value> stack;
	std::uint16_t local_var_count;
	std::uint16_t stack_size;
	std::int32_t unknown; //callback something
	VPtr<Value> current_iterator;
	std::uint32_t iterator_allocated;
	std::uint32_t iterator_length;
	std::uint32_t iterator_index;
	std::int32_t another_unknown2;
	char unknown4[3];
	char iterator_filtered_type;
	char unknown5;
	char iterator_unknown;
	char unknown6;
	std::uint32_t infinite_loop_count;
	char unknown7[2];
	bool paused;
	char unknown8[51];
};

static const size_t y = sizeof(ProcConstants);

}

