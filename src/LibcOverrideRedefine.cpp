#include"LibcOverrideRedefine.h"
#include"MyMalloc.h"

#ifndef WIN32_DO_PATCHING
void* malloc(size_t s) __THROW
{
	return my_malloc(s);
}

void  free(void* p) __THROW
{
	my_free(p);
}
#endif