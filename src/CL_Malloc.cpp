#include<memory>
#include"CL_Malloc.h"
#include"CL_ThreadCache.h"

int InvalidFree(void* ptr)
{
	#if _DEBUG
	printf("error in InvalidFree\n");
	#endif
	return 0;
}

void* DoMalloc(size_t size)
{
	return DoMallocNoErron(size);
}

void* DoMallocNoErron(size_t size)
{
	if(size <= SizeOfPage)
	{
		Small* temp = (Small*)DoMallocSmall(CL_ThreadCache::GetCache(), size+sizeof(Small));
		if (temp == NULL)
		{
			return NULL;
		}
		temp->size = size + sizeof(Small);
		temp->OwnerThreadCache = CL_ThreadCache::GetCache();
		void* result = (void*)((uintptr_t)temp+sizeof(Small));
		return result;
	}
	else if(size >= 8*SizeOfPage)
	{
		Huge* temp = (Huge*)DoMallocHuge(size+sizeof(Huge));
		temp->size = size+sizeof(Huge);
		void* result = (void*)((uintptr_t)temp+sizeof(Huge));
		return result;
	}
	else
	{
		Large* ptr = (Large*)DoMallocLarge(size+sizeof(Large));
		ptr->size = size+sizeof(Large);
		Large* temp = CL_Static::s_pFreeList;
		
		if(temp->NextPtr != NULL)
		{
			ptr->NextPtr = temp->NextPtr;
			ptr->NextPtr->PrevPtr = ptr;
			temp->NextPtr = ptr;
		}
		else
		{
			temp->NextPtr = ptr;
		}
		
		void* result = (void*)((uintptr_t)ptr+sizeof(Large));
		return result;
	}
}

void* DoMallocLarge(size_t size)
{
	if(!CL_Static::IsInited())
	{
		#if _DEBUG
		printf("error in DoMallocSmall\n");
		#endif
		return NULL;
	}
	
	Large* temp = CL_Static::s_pFragmentList;
	while(temp != NULL)
	{
		temp = temp->NextPtr;
		if(temp->size >= size)
		{
			temp->PrevPtr->NextPtr = temp->NextPtr;
			temp->NextPtr->PrevPtr = temp->PrevPtr;
			return temp;
		}
	}
	
	size_t cl = CL_Static::SizeMap()->GetClass(size);
	void* result = CL_Static::CentralCache()[cl].LargeAlloc(size);
	return result;
}

void* DoMallocHuge(size_t size)
{
	if(!CL_Static::IsInited())
	{
		#if _DEBUG
		printf("error in DoMallocSmall\n");
		#endif
		return NULL;
	}
	return (CL_Static::PageHeap()->AllocHuge(size));
}

void* DoMallocSmall(CL_ThreadCache* heap, size_t size)
{
	if (!CL_Static::IsInited() || heap == NULL)
	{
		#if _DEBUG
		printf("error in DoMallocSmall\n");
		#endif
		return NULL;
	}
	
	size_t cl = CL_Static::SizeMap()->GetClass(size);
	size = CL_Static::SizeMap()->GetSize(cl);
	return (heap->Allocate(size, cl));
}

int DoFree(void* ptr)
{
	return DoFreeWithCallback(ptr, &InvalidFree);
}

int DoFreeWithCallback(void* ptr, int(*InvalidFreeFunc)(void*))
{
	if (ptr == NULL)
	{
		return -1;
	}

	if (!CL_Static::IsInited())
	{
		return FreeNullOrInvalid(ptr, InvalidFreeFunc);
	}

	unsigned int* temp = (unsigned int*)(reinterpret_cast<uintptr_t>(ptr) - sizeof(unsigned int));
	if (!CheckMemory(temp)) return -1;
	if (*temp <= SizeOfPage)
	{
		Small* result = (Small*)(reinterpret_cast<uintptr_t>(ptr) - sizeof(Small));
		size_t cl = CL_Static::SizeMap()->GetClass(result->size);
		CL_ThreadCache* p = result->OwnerThreadCache;
		p->TempDeAllocate(result);
	}
	else if (*temp >= 8 * SizeOfPage)
	{
		CL_Static::PageHeap()->FreeHuge(ptr);
	}
	else
	{
		Large* temp = (Large*)ptr;
		if(temp->NextPtr != NULL && temp->PrevPtr != NULL)
		{
			temp->PrevPtr->NextPtr = temp->NextPtr;
			temp->NextPtr->PrevPtr = temp->PrevPtr;
		}

		Large* stay = CL_Static::s_pFragmentList;
		if(stay == NULL)
		{
			stay = temp;
		}
		else
		{
			temp->NextPtr = stay->NextPtr;
			temp->PrevPtr = stay;
			temp->NextPtr->PrevPtr = temp;
			stay->NextPtr = temp;
		}
	}
	
	return 0;
}

int FreeNullOrInvalid(void* ptr, int (*InvalidFreeFunc)(void*))
{
	if(ptr != NULL)
	{
    	(*InvalidFreeFunc)(ptr);
	}
	return 0;
}
/*
bool CheckMalloc(void* pElement)
{
	if (pElement < (void*)0x0000c000)
	{
		return true;
	}
	return false;
}
*/