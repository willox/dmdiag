#pragma once

#include <stdint.h>
#include <optional>
#include "VPtr.h"

namespace dm
{
struct Value;
struct Variable;
struct VariableTable;
struct InitialVariable;
struct InitialVariableTable;
struct VarDeclaration;
struct VarDeclarationTable;
struct Misc;
struct MiscTable;

struct Proc;
struct ProcInstance;
struct ExecutionContext;
struct ProcTable;

struct String;
struct VarName;
struct StringTable;

struct Mob;
struct MobFields;
struct MobTable;
struct MobType;
struct MobTypeTable;

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

template<typename T>
struct Ref
{
	uint32_t index;
	T* get();

	// Only for T = String
	const char* string();

	bool operator==(const Ref<T>& rhs) const
	{
		return index == rhs.index;
	}

	bool operator!=(const Ref<T>& rhs) const
	{
		return index != rhs.index;
	}

	// is this valid for all refs?
	static Ref<T> Invalid() { return {0xFFFF}; }
};
static_assert(sizeof(Ref<void>) == 4);

// Might just be generic 'Path'? Or 'Type'.
struct ObjPath
{
	Ref<String> path;
	Ref<ObjPath> parent;
	uint32_t wat;
	char unk_0[0x40];
	Ref<String> test;
	Ref<Misc> interesting_index;
	Ref<Misc> var_declarations;
	Ref<Misc> another;
	Ref<Misc> initial_vars;
	char unk_2[0x4];
};

static_assert(sizeof(ObjPath) == 100);

struct ObjPathTable
{
	VPtr<ObjPath> elements;
	uint32_t count;
};

static_assert(sizeof(ObjPathTable) == 8);

enum class Gender
{
	Neuter,
	Male,
	Female,
	Plural
};

// wtf is going on down here

struct SomeGlobals
{
	char unk_0[0x40];
	VPtr<VPtr<MobFields>> mob_fields;
	uint32_t mob_fields_count;
};

}

