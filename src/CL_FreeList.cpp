#include"CL_FreeList.h"
#include<stdio.h>

CL_FreeList::CL_FreeList(){}

int CL_FreeList::Init()
{
	m_pList = NULL;
    m_length = 0;
    m_MaxLength = 1;
    return 0;
}

int CL_FreeList::SetMaxLength(size_t NewMax)
{
	m_MaxLength = NewMax;
	return 0;
}

int CL_FreeList::Push(void* ptr)
{
	if (!CheckMemory(this)) return -1;
	SL_Push(&m_pList, ptr);
    m_length++;
	return 0;
}

void* CL_FreeList::Pop()
{
	if(m_pList == NULL)
	{
		#if _DEBUG
		printf("error in CL_FreeList::Pop\n");
		#endif
		return NULL;
	}
    m_length--;
    return SL_Pop(&m_pList);
}

void* CL_FreeList::Next()
{
	return SL_Next(&m_pList);
}

int CL_FreeList::PushRange(int count, void* start, void* end)
{
	SL_PushRange(&m_pList, &start, &end);
    m_length += count;
    return 0;
}

int CL_FreeList::PopRange(int count, void** start, void** end)
{
	if(m_length < (unsigned int)count)
	{
		#if _DEBUG
		printf("error in CL_FreeList::PopRange\n");
		#endif
		return -1;
	}
	SL_PopRange(&m_pList, count, start, end);
    m_length -= count;
    return 0;
}