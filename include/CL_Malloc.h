#ifndef _CL_MALLOC_H_
#define _CL_MALLOC_H_

#include"CL_Static.h"

int InvalidFree(void* ptr);

void* DoMalloc(size_t size);

void* DoMallocNoErron(size_t size);

void* DoMallocSmall(CL_ThreadCache* heap, size_t size);

void* DoMallocLarge(size_t size);

void* DoMallocHuge(size_t size);

int DoFree(void* ptr);

int DoFreeWithCallback(void* ptr, int (*InvalidFreeFunc)(void*));

int FreeNullOrInvalid(void* ptr, int (*InvalidFreeFunc)(void*));

#endif