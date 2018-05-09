#ifndef _CL_PAGEHEAPALLOCATOR_H_
#define _CL_PAGEHEAPALLOCATOR_H_

#include"CL_SizeMap.h"

template<class T>
class CL_PageHeapAllocator
{
private:
	char* m_pFreeArea;
	size_t m_FreeAvail;
	void* m_pFreeList;
	int m_InUse;
	
public:
	int Init()
	{
		m_InUse = 0;
		m_pFreeArea = NULL;
		m_FreeAvail = 0;
		m_pFreeList = NULL;
		Delete(New());
		return 0;
	}
	
	T* New()
	{
		void* result;
	    if(m_pFreeList != NULL)
		{
			result = m_pFreeList;
			m_pFreeList = *(reinterpret_cast<void**>(result));
		}
		else
		{
			if(m_FreeAvail < sizeof(T))
			{
				void* temp = HeapAlloc(CL_SizeMap::s_Heap, HEAP_ZERO_MEMORY, SizeOfPage*16);
		        m_pFreeArea = reinterpret_cast<char*>(temp);
		        if(m_pFreeArea == NULL)
				{
					#if _DEBUG
					printf("error in CL_PageHeapAllocator::New\n");
					#endif
		        }
		        m_FreeAvail = SizeOfPage*16;
			}
			result = m_pFreeArea;
			m_pFreeArea += sizeof(T);
			m_FreeAvail -= sizeof(T);
	    }
	    m_InUse++;
	    return reinterpret_cast<T*>(result);
	}
	
	int Delete(T* p)
	{
		*(reinterpret_cast<void**>(p)) = m_pFreeList;
	    m_pFreeList = p;
	    m_InUse--;
		return 0;
	}
	
	int Inuse() const
	{
		return m_InUse;
	}
};

#endif