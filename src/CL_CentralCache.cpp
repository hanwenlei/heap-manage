#include"LinkedList.h"
#include"CL_Static.h"
#include"CL_ThreadCache.h"

CL_CentralCache::CL_CentralCache(){}

int CL_CentralCache::Init(size_t class_size)
{
	m_SizeClass = class_size;
	SP_Init(&m_EmptySpanList);
	SP_Init(&m_NonEmptySpanList);
	SP_Init(m_pCurrentSpan);
	m_SpansNumber = 0;
	m_EmptyMemoryNumber = 0;
	return 0;
}

int CL_CentralCache::ReleaseListToSpans(void* start, CL_ThreadCache* ThreadCache)
{
	while(start)
	{
		void* next = SL_Next(start);
		int temp = ReleaseToSpans(start, ThreadCache);
		if (temp == -1)
		{
			break;
		}
		start = next;
	}
	return 0;
}

Span* CL_CentralCache::MapObjectToSpan(void* object)
{
	uintptr_t result = reinterpret_cast<uintptr_t>(object);
	Span* pSpan = &m_EmptySpanList;
	bool flag = true;
	while(pSpan->next != pSpan)
	{
		if((result >= pSpan->StartAddress) && (result <= pSpan->StartAddress + 64*1024))
		{
			flag = false;
			break;
		}
		pSpan = pSpan->next;
	}
	
	if(flag)
	{
		pSpan = &m_NonEmptySpanList;
		while(pSpan != NULL)
		{
			if (pSpan == pSpan->next)
			{
				return NULL;
			}
			else if((result >= pSpan->StartAddress) && (result <= pSpan->StartAddress + 64*1024))
			{
				break;
			}
			pSpan = pSpan->next;
		}
	}
	
	return pSpan;
}

int CL_CentralCache::ReleaseToSpans(void* object, CL_ThreadCache* ThreadCache)
{
	Span* pSpan = MapObjectToSpan(object);
	if(pSpan == NULL || pSpan->ReferenceCount <= 0)
	{
		return -1;
	}
	
	if(pSpan->objects == NULL)
	{
		SP_Remove(&m_EmptySpanList, pSpan);
		SP_Prepend(&m_NonEmptySpanList, pSpan);
	}
	
	m_EmptyMemoryNumber++;
	pSpan->ReferenceCount--;
	
	if(pSpan->ReferenceCount == 0)
	{
		if(pSpan->GetNext() == NULL)
		{
			m_NonEmptySpanList.next = &m_NonEmptySpanList;
		}
		else
		{
			SP_Remove(&m_NonEmptySpanList, pSpan);
		}
		
		{
			CL_CriticalSection cs;
			CL_Static::PageHeap()->RemoveFromCentralCache(pSpan);
		}
		
		m_EmptyMemoryNumber -= 64*1024/m_SizeClass;
		m_SpansNumber--;
	}
	else
	{
		*(reinterpret_cast<void**>(object)) = pSpan->objects;
		pSpan->objects = object;
	}
	
	if(m_EmptyMemoryNumber > (int)(pSpan->ObjectsNumber/2))
	{
		ThreadCache->m_list[m_SizeClass].m_MaxLength = 1;
	}
	
	return 0;
}

int CL_CentralCache::InsertRange(void* start, void* end, int number, CL_ThreadCache* ThreadCache)
{
	if (number < 0 || number > g_SizeOfQueue)
	{
		return -1;
	}

	for(int i = 0; i < number; i++)
	{
		int temp = ReleaseToSpans(start, ThreadCache);
		if (temp == -1)
		{
			break;
		}
		start = SL_Next(start);
	}
	return 0;
}

int CL_CentralCache::RemoveRange(void** start, void** ends, int number)
{
	if (number > g_SizeOfQueue)
	{
		#if _DEBUG
		printf("error in CL_CentralCache::RemoveRange\n");
		#endif
		return -1;
	}

	CL_CriticalSection cs;
	int result = 0;
	void* head = NULL;
	void* tail = NULL;

	tail = GetFromSpansCurrent();
	if (tail != NULL)
	{
		head = tail;
		result = 1;
		while (result < number)
		{
			void* t = GetFromSpansList();
			if (t == NULL)
			{
				break;
			}
			SL_Push(&head, t);
			result++;
		}
	}

	*start = head;
	*ends = tail;
	return result;
}

void* CL_CentralCache::GetFromSpansCurrent()
{
	void* t = GetFromSpansList();

	if(t == NULL)
	{
		GetFromPage();
		t = GetFromSpansList();
	}

	return t;
}

void* CL_CentralCache::GetFromSpansList()
{
	Span* temp = NULL;
	if(m_pCurrentSpan == NULL)
	{
		if (SP_IsEmpty(&m_NonEmptySpanList))
		{
			return NULL;
		}
		temp = &m_NonEmptySpanList;
		m_pCurrentSpan = &m_NonEmptySpanList;
	}
	else
	{
		temp = m_pCurrentSpan;
	}
	
	temp->ReferenceCount++;
	void* result = temp->objects;
	if(result == NULL)
	{
		SP_Remove(&m_NonEmptySpanList, temp);
		SP_Prepend(&m_EmptySpanList, temp);
	}
	else
	{
		temp->objects = GetNext(result);
	}
	m_EmptyMemoryNumber--;
	return result;
}

int CL_CentralCache::GetFromPage()
{
	Span* pSpan = NULL;
	
	{
		CL_CriticalSection cs;
		pSpan = CL_Static::PageHeap()->NewPage(16);
	}
	
	if(pSpan == NULL)
	{
		#if _DEBUG
		printf("error in CL_CentralCache::GetFromPage\n");
		#endif
		return -1;
	}
	
	void** tail = &pSpan->objects;
	char* ptr = reinterpret_cast<char*>(pSpan->StartAddress);
	char* limit = ptr + SizeOfPage*16;
	const size_t size = CL_Static::SizeMap()->GetSize(m_SizeClass);
	int number = 0;
	
	while(ptr + size <= limit)
	{
		*tail = ptr;
		tail = reinterpret_cast<void**>(ptr);
		ptr += size;
		number++;
	}
	*tail = NULL;
	pSpan->ReferenceCount = 0;
	
	{
		CL_CriticalSection cs;
		Span* temp = &m_NonEmptySpanList;
		SP_Prepend(temp, pSpan);
		m_SpansNumber++;
		m_EmptyMemoryNumber += number;
		m_pCurrentSpan = &m_NonEmptySpanList;
	}
	return 0;
}

void* CL_CentralCache::GetNext(void* pElement)
{
	return *(reinterpret_cast<void**>(pElement));
}

int CL_CentralCache::GetLength()
{
    return m_EmptyMemoryNumber;
}

void* CL_CentralCache::LargeAlloc(size_t size)
{
	void* result = NULL;
	Span* pSpan = &m_NonEmptySpanList;
	size_t limit = pSpan->StartAddress + SizeOfPage*16 - pSpan->CurentPoint;
	
	while(size > limit)
	{
		pSpan = pSpan->next;
		limit = pSpan->StartAddress + SizeOfPage*16 - pSpan->CurentPoint;
	}
	
	if(pSpan != NULL)
	{
		result = (void*)pSpan->CurentPoint;
		pSpan->CurentPoint = pSpan->CurentPoint + (uintptr_t)size;
	}
	else
	{
		GetFromPage();
		result = LargeAlloc(size);
	}
	return result;
}