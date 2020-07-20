#pragma once

#include <stdint.h>
#include <optional>
#include "VPtr.h"

namespace dm
{
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

// Just a String but indexed from another container
struct VarName : public String
{};

static_assert(sizeof(VarName) == 28);

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

static_assert(sizeof(Ref<String>) == 4);

struct StringTable
{
	VPtr<VPtr<String>> strings;
	uint32_t size;
};

static_assert(sizeof(StringTable) == 8);

struct Value
{
	Value()
		: type(DataType::NULL_D)
		, _number(0.f)
	{}

	Value(DataType type, uint32_t data)
		: type(type)
		, _data(data)
	{}

	Value(float num)
		: type(DataType::NUMBER)
		, _number(num)
	{}

	Value(Ref<String> str)
		: type(DataType::STRING)
		, _string(str)
	{}

	Value(Ref<MobType> path)
		: type(DataType::MOB_TYPEPATH)
		, _mob_path(path)
	{}

	DataType type;
	union
	{
		uint32_t _data;
		float _number;
		Ref<String> _string;
		Ref<MobType> _mob_path;
	};

	static const Value Null;
};

static_assert(sizeof(Value) == 8);

struct Misc;

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

// TODO: Might actually be more specific (VariableTableEntry?)
struct Variable
{
	uint32_t unk_0;
	Ref<String> name;
	Value value;
};

static_assert(sizeof(Variable) == 16);

struct VariableTable
{
	// TODO: Is this sorted?
	VPtr<Variable> elements;
	uint16_t count;
	uint16_t capacity;

	Value* GetField(Ref<String> key);
};

static_assert(sizeof(VariableTable) == 8);


// wtf is going on down here

enum class Gender
{
	Neuter,
	Male,
	Female,
	Plural
};

struct SomeGlobals
{
	char unk_0[0x40];
	VPtr<VPtr<MobFields>> mob_fields;
	uint32_t mob_fields_count;
};

struct ExecutionContext;
struct Proc;

struct ProcInstance
{
	Ref<Proc> proc;
	int unknown1;
	Value usr;
	Value src;
	union
	{
		VPtr<ExecutionContext> context;
		VPtr<ProcInstance> next; // When we're part of the global `procinstance_freelist` linked list, this is the next element
	};
	int argslist_id;
	int unknown4; //some callback thing
	union
	{
		int unknown5;
		int extended_profile_id;
	};
	int arg_count;
	VPtr<Value> args;
	char unknown6[0x58];
	int time_to_resume;
};

struct ExecutionContext
{
	VPtr<ProcInstance> proc_instance;
	VPtr<ExecutionContext> parent_context;
	Ref<String> dbg_proc_file;
	uint32_t dbg_current_line;
	VPtr<uint32_t> bytecode;
	uint16_t current_opcode;
	char test_flag;
	char unknown1;
	Value cached_datum;
	char unknown2[16];
	Value dot;
	VPtr<Value> local_variables;
	VPtr<Value> stack;
	uint16_t local_var_count;
	uint16_t stack_size;
	int32_t unknown; //callback something
	VPtr<Value> current_iterator;
	uint32_t iterator_allocated;
	uint32_t iterator_length;
	uint32_t iterator_index;
	uint32_t another_unknown2;
	char unknown4[3];
	char iterator_filtered_type;
	char unknown5;
	char iterator_unknown;
	char unknown6;
	uint32_t infinite_loop_count;
	char unknown7[2];
	bool paused;
	char unknown8[51];
};

struct InitialVariable
{
	Ref<String> name;

	// Does anything use these? They exist.
	union
	{
		Ref<VarName> _name;
		Ref<Value> _value;
	};

	Value value;
};

static_assert(sizeof(InitialVariable) == 16);

struct InitialVariableTable
{
private:
	uint16_t count_mul2;
public:
	VPtr<InitialVariable> variables;
	uint32_t unk_0;

	uint16_t count() { return count_mul2 / 2; }
};

static_assert(sizeof(InitialVariableTable) == 12);

struct VarDeclaration
{
private:
	enum class Flags : uint8_t
	{
		Global = 0x01,
		Temporary = 0x04,
	};

public:
	union
	{
		Ref<VarName> name;
		Ref<Value> value;
	};

	Flags flags;

	bool isGlobal()
	{
		return (uint8_t)flags & (uint8_t)Flags::Global;
	}

	bool isTemporary()
	{
		return (uint8_t)flags & (uint8_t)Flags::Temporary;
	}
};

static_assert(sizeof(VarDeclaration) == 8);

// TODO: Name sort of needs validation
struct VarDeclarationTable
{
private:
	uint16_t count_mul2;
public:
	VPtr<VarDeclaration> variables;
	uint32_t unk0;

	uint16_t count() { return count_mul2 / 2; }
};

struct LocalVariableTable
{
	uint16_t count;
	VPtr<Ref<VarName>> names;
};

struct Misc
{
	union
	{
		InitialVariableTable initial_variable_table;
		VarDeclarationTable var_declaration_table;
		LocalVariableTable local_variable_table;
		// proc bytecode
	};
};

static_assert(sizeof(Misc) == 12);

struct MiscTable
{
	VPtr<VPtr<Misc>> elements;
	uint32_t count;
};

static_assert(sizeof(MiscTable) == 8);

struct Proc
{
	Ref<String> path;
	Ref<String> name;
	uint32_t unk_0;
	uint32_t unk_1;
	uint32_t unk_2;
	uint32_t unk_3;
	Ref<Misc> bytecode;
	Ref<Misc> locals;
	Ref<Misc> test;
};

static_assert(sizeof(Proc) == 36);

struct ProcTable
{
	VPtr<Proc> elements;
	uint32_t count;
};

static_assert(sizeof(ProcTable) == 8);

}

