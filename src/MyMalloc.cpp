#include"MyMalloc.h"

void* my_malloc(size_t size) __THROW
{
	return DoMalloc(size);
}

void my_free(void* ptr) __THROW
{
	DoFree(ptr);
}