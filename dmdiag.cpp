#include <Windows.h>
#include "dm/internal_types.h"
#include "dm/State.h"
#include <iostream>

#include "squirrel.h"
#include "sqstdaux.h"
#include "sqstdio.h"
#include "sqstdmath.h"
#include "sqstdstring.h"
#include "sqstdsystem.h"
#include <string>

//static const char dump_path[] = "C:\\Users\\wwall\\Downloads\\Dump\\core.80099";
static const char dump_path[] = "C:\\Users\\wwall\\Downloads\\Dump\\core.10813";

static dm::ProcInstance* current_frame = nullptr;

SQInteger frame(HSQUIRRELVM v)
{
	if (sq_gettop(v) != 2)
		return 0;

	SQInteger index;
	sq_getinteger(v, 2, &index);

	uint32_t frame = 0;
	dm::ExecutionContext* ctx = dm::current_state->_current_execution_context;
	while (ctx)
	{
		if (frame == index)
		{
			current_frame = ctx->proc_instance.get();
		}

		frame++;
		ctx = ctx->parent_context.get();
	}

	return 0;
}

std::string Var2Str(dm::Value& val)
{
	if (val.type == dm::DataType::NULL_D)
	{
		return "null";
	}

	if (val.type == dm::DataType::NUMBER)
	{
		return std::to_string(val._number);
	}

	if (val.type == dm::DataType::LIST)
	{
		return "list#" + std::to_string(val._data);
	}

	if (val.type == dm::DataType::DATUM)
	{
		return "datum#" + std::to_string(val._data);
	}

	return "<unknown type>";
}

SQInteger locals(HSQUIRRELVM v)
{
	dm::ExecutionContext* ctx = current_frame->context.get();

	std::cout << ". = " << Var2Str(ctx->dot) << '\n';
	std::cout << "src = " << Var2Str(ctx->proc_instance->src) << '\n';

	uint32_t local_count = ctx->local_var_count;
	dm::Ref<dm::VarName>* local_names = current_frame->proc.get()->locals.get()->local_variable_table.names.get();

	for (uint32_t i = 0; i < local_count; i++)
	{
		std::cout << local_names[i].string() << " = " << Var2Str(ctx->local_variables[i]) << '\n';
	}

	return 0;
}

SQInteger trace(HSQUIRRELVM v)
{
	uint32_t frame = 0;
	dm::ExecutionContext* ctx = dm::current_state->_current_execution_context;
	while (ctx)
	{
		const char* pName = ctx->proc_instance->proc.get()->path.string();
		if (strlen(pName) == 0)
			pName = "<unnamed>";

		const char* pFile = ctx->dbg_proc_file.string();
		if (strlen(pFile) == 0)
			pFile = "<none>";

		uint32_t line = ctx->dbg_current_line;

		std::cout << ((current_frame == ctx->proc_instance.get()) ? "(selected) " : "") << '#' << frame << "  " << pName << " @ " << pFile << ':' << line << '\n';

		frame++;
		ctx = ctx->parent_context.get();
	}

	return 0;
}

void printfunc(HSQUIRRELVM v, const SQChar *s, ...) { 
	va_list arglist; 
	va_start(arglist, s); 
	vprintf(s, arglist); 
	va_end(arglist); 
}

SQInteger file_lexfeedASCII(SQUserPointer file)
{
	int ret;
	char c;
	if( ( ret=fread(&c,sizeof(c),1,(FILE *)file )>0) )
		return c;
	return 0;
}

int compile_file(HSQUIRRELVM v,const char *filename)
{
	FILE *f=fopen(filename,"rb");
	if(f)
	{
		sq_compile(v,file_lexfeedASCII,f,filename,1);
		fclose(f);
		return 1;
	}
	return 0;
}

static void PushValue(HSQUIRRELVM v, dm::Value val)
{
	switch (val.type)
	{
	case dm::DataType::NUMBER:
		sq_pushfloat(v, val._number);
		break;
	case dm::DataType::STRING:
		sq_pushstring(v, val._string.string(), strlen(val._string.string()));
		break;
	case dm::DataType::MOB:
		sq_pushroottable(v);
		sq_pushstring(v, "Mob", 3);
		sq_get(v, -2);
		sq_pushroottable(v);
		sq_pushinteger(v, val._data);
		sq_call(v, 1, 0, SQFalse);
		break;
	default:
		sq_pushroottable(v);
		sq_pushstring(v, "UnknownType", 3);
		sq_get(v, -2);
		sq_pushroottable(v);
		sq_pushinteger(v, (uint32_t) val.type);
		sq_call(v, 2, 0, SQFalse);
		break;
	}
}

SQInteger GetField_Mob(HSQUIRRELVM v)
{
	
	if (sq_gettop(v) != 3)
		return 0;

	SQInteger index;
	const SQChar* key;
	sq_getinteger(v, 2, &index);
	sq_getstring(v, 3, &key);

	dm::Ref<dm::Mob> mob{index};
	dm::Ref<dm::String> str = *dm::current_state->GetStringRef(key);

	PushValue(v, *dm::Mob::GetField(mob, str));
	return 1;
}

int main(int argc, char** argv)
{
	dm::State state(dump_path);

	// TODO: remove
	dm::SetupMobFieldGetters();

	HSQUIRRELVM v = sq_open(1024);
	sqstd_seterrorhandlers(v);
	sq_setprintfunc(v, printfunc, printfunc);
	
	sqstd_register_iolib(v);
	sqstd_register_mathlib(v);
	sqstd_register_stringlib(v);
	sqstd_register_systemlib(v);

	sq_pushroottable(v);

	sq_pushstring(v, "GetField_Mob", sizeof("GetField_Mob") - 1);
	sq_newclosure(v, GetField_Mob, 0);
	sq_newslot(v, -3, false);

	sq_pushstring(v, "locals", sizeof("locals") - 1);
	sq_newclosure(v, locals, 0);
	sq_newslot(v, -3, false);

	sq_pushstring(v, "frame", sizeof("frame") - 1);
	sq_newclosure(v, frame, 0);
	sq_newslot(v, -3, false);

	sq_pushstring(v, "trace", sizeof("trace") - 1);
	sq_newclosure(v, trace, 0);
	sq_newslot(v, -3, false);

	/*
	sq_pushstring(v, "UnknownType", sizeof("UnknownType") - 1);
	sq_newclass(v, SQFalse);
	sq_newslot(v, -3, SQFalse);

	sq_pushstring(v, "Mob", 3);
	sq_newclass(v, SQFalse);
	sq_newslot(v, -3, SQFalse);
	*/

	

	if(SQ_FAILED(sqstd_dofile(v, _SC("E:\\dmdiag\\test.nut"), SQFalse, SQTrue))) {
		printf("Error: failed to run test.nut\n");
	}

	while (true)
	{
		SQInteger oldtop=sq_gettop(v);

		std::string str;
		std::getline(std::cin, str);

		
		if(SQ_FAILED(sq_compilebuffer(v, str.c_str(), str.size(), "input", SQFalse)))
		{
			std::cout << "failed";
			continue;
		}

		sq_pushroottable(v);
		sq_call(v, 1, 0, SQTrue);
		std::cout << '\n';
		sq_settop(v, oldtop);
	}

	sq_pop(v, 1);
	sq_close(v);

	return 0;
}