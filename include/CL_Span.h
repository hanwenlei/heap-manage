#ifndef _CL_SPAN_H_
#define _CL_SPAN_H_

#include"CL_SizeMap.h"
#include"CL_StorageStruct.h"

class Span
{
public:
	uintptr_t StartAddress;
	uintptr_t CurentPoint;
	atomic<Span*> prev;
	atomic<Span*> next;
	void* objects;
	unsigned int ObjectsNumber;
	atomic_int ReferenceCount;
	unsigned int location;
	bool flag;
	enum
	{
		IN_USE = 1, ON_ALIGN_FREELIST, ON_RECYCLE_FREELIST,
	};

	Span& operator = (Span& pElenent);
	Span* GetNext();
	Span* GetPrev();
};

Span* NewSpan();

int DeleteSpan(Span* span);

int SP_Init(Span* list);

int SP_Remove(Span* list, Span* span);

bool SP_IsEmpty(const Span* list);

int SP_Prepend(Span* list, Span* span);

int SP_Length(const Span* list);

int CS_Prepend(Span* list, Span* span);

Span* Search(Span* list);

Span* GetUnmarked(Span* span);

//int Remove(Span* list, Span* span);

#endif