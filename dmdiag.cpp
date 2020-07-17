#include <Windows.h>
#include "dm/internal_types.h"
#include "dm/State.h"

#include "squirrel.h"
#include "sqstdaux.h"
#include "sqstdio.h"
#include "sqstdmath.h"
#include "sqstdstring.h"
#include "sqstdsystem.h"

static const char dump_path[] = "C:\\Users\\wwall\\Downloads\\Dump\\core.80099";

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

int main(int argc, char** argv)
{
	dm::State state(dump_path);

	// TODO: remove
	dm::SetupMobFieldGetters();

	for (uint32_t i = 0; i < 100; i++)
	{
		dm::Ref<dm::Mob> mob{i};

		auto name = dm::Mob::GetField(mob, *state.GetStringRef("name"));
		auto desc = dm::Mob::GetField(mob, *state.GetStringRef("desc"));
		auto layer = dm::Mob::GetField(mob, *state.GetStringRef("layer"));
		auto stat = dm::Mob::GetField(mob, *state.GetStringRef("stat"));

		continue;
	}

	HSQUIRRELVM v = sq_open(1024);
	sqstd_seterrorhandlers(v);
	sq_setprintfunc(v, printfunc, printfunc);
	
	sqstd_register_iolib(v);
	sqstd_register_mathlib(v);
	sqstd_register_stringlib(v);
	sqstd_register_systemlib(v);

	sq_pushroottable(v);

	sq_pushstring(v, "UnknownType", sizeof("UnknownType") - 1);
	sq_newclass(v, SQFalse);
	sq_newslot(v, -3, SQFalse);

	sq_pushstring(v, "Mob", 3);
	sq_newclass(v, SQFalse);
	sq_newslot(v, -3, SQFalse);
	
	if(SQ_FAILED(sqstd_dofile(v, _SC("E:\\dmdiag\\test.nut"), SQFalse, SQTrue))) {
		printf("Error: failed to run test.nut\n");
	}

	sq_pop(v, 1);
	sq_close(v);

	return 0;
}