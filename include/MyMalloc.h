#include"CL_Malloc.h"
#include"LibcOverride.h"

#ifndef PERFTOOLS_DLL_DECL
#	ifdef _WIN32
#		define PERFTOOLS_DLL_DECL  __declspec(dllimport)
#	else
#		define PERFTOOLS_DLL_DECL
#	endif
#endif

#ifndef __THROW
#define __THROW
#endif

extern "C" void* my_malloc(size_t size) __THROW;

extern "C" void my_free(void* ptr) __THROW;