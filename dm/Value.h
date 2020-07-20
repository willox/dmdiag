#pragma once

#include "internal_types.h"

namespace dm
{

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

}