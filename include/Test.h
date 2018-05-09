#include<stdio.h>
#include<malloc.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include"MyMalloc.h"

#define WM_MALLOC 504

int display_mallinfo();

int Test();

int VectorMemoryCost();

int MyAllocate();

unsigned int ThreadMallocFunction(void*);

unsigned int ThreadFreeFunction(void*);