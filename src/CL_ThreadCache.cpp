#include"CL_ThreadCache.h"
#include"CL_Static.h"
#include"LinkedList.h"

static bool phinited = false;
CL_PageHeapAllocator<CL_ThreadCache> ThreadcacheAllocator;
CL_PageHeapAllocator<CL_Queue> QueueAllocator;
CL_ThreadCache* CL_ThreadCache::s_pThreadHeaps = NULL;
int CL_ThreadCache::s_ThreadHeapsCount = 0;
bool CL_ThreadCache::s_IsInited = false;

__declspec(thread) CL_ThreadCache::ThreadLocalData CL_ThreadCache::threadlocal_data_
    ATTR_INITIAL_EXEC = {0};

CL_ThreadCache::CL_ThreadCache(){}

int CL_ThreadCache::Init(DWORD ThreadId)
{
	m_IsFreeTooMany = false;
	m_GarbageCount = 0;
	m_ThreadId = ThreadId;
	m_GarbageQueue = NULL;
	m_pNext = NULL;
	m_pPrev = NULL;
	
	for(size_t cl = 0; cl < kNumClasses; cl++)
	{
		m_list[cl].Init();
	}
	InitionQueue();
	return 0;
}

int CL_ThreadCache::InitionQueue()
{
	m_GarbageQueue = new CL_Queue;
	m_GarbageQueue->InitQueue();
	return 0;
}

int CL_ThreadCache::ClearUp()
{
	for(int cl = 0; cl < kNumClasses; cl++)
	{
		if(m_list[cl].Length() > 0)
		{
			ReleaseToCentralCache(&m_list[cl], cl, m_list[cl].Length());
		}
	}
	return 0;
}

CL_ThreadCache* CL_ThreadCache::CreateCache()
{
	CL_ThreadCache* heap = NULL;
	{
		CL_CriticalSection cs;
		DWORD ThreadId = GetCurrentThreadId();
		for(CL_ThreadCache* h = s_pThreadHeaps; h != NULL; h = h->m_pNext)
		{
			if(h->m_ThreadId == ThreadId)
			{
				heap = h;
				break;
			}
		}
		
		if(heap == NULL)
		{
			heap = NewCache(ThreadId);
		}
		threadlocal_data_.heap = heap;
	}
	return heap;
}

CL_ThreadCache* CL_ThreadCache::NewCache(DWORD ThreadId)
{
//	CL_ThreadCache* heap = ThreadcacheAllocator.New();
	CL_ThreadCache* heap = new CL_ThreadCache;
	heap->Init(ThreadId);
	heap->m_pNext = s_pThreadHeaps;
	heap->m_pPrev = NULL;
	
	if(s_pThreadHeaps != NULL)
	{
    	s_pThreadHeaps->m_pPrev = heap;
	}
	
	s_pThreadHeaps = heap;
	s_ThreadHeapsCount++;
	return heap;
}

void* CL_ThreadCache::Allocate(size_t size, size_t class_size)
{
	if(size > kMaxSize)
	{
		#if _DEBUG
		printf("error in CL_ThreadCache::Allocate\n");
		#endif
		return NULL;
	}

	CL_FreeList* list = &m_list[class_size];
	if (m_GarbageQueue == NULL)
	{
		InitionQueue();
	}
	if (m_GarbageQueue->GetRear() - m_GarbageQueue->GetFront() >= g_SizeOfQueue / 2)
	{
		CollectGarbage();
	}

	if(list->IsEmpty())
	{
		if(list->MaxLength() < 32*1024/size)
		{
			list->m_MaxLength++;
		}
		return GetFromCentralCache(size, class_size);
	}
	else
	{
		return list->Pop();
	}
}

void* CL_ThreadCache::GetFromCentralCache(size_t size, size_t class_size)
{
	CL_FreeList* list = &m_list[class_size];
	if(!list->IsEmpty())
	{
		#if _DEBUG
		printf("error in CL_ThreadCache::GetFromCentralCache\n");
		#endif
		return NULL;
	}
	
	void *start = NULL, *end;
	int temp = list->MaxLength();
	int GetCount = CL_Static::CentralCache()[class_size].RemoveRange(&start, &end, temp);
	
	if(start == NULL || GetCount <= 0)
	{
		return NULL;
	}
	
	if(--GetCount >= 0)
	{
		list->PushRange(GetCount, SL_Next(start), end);
		list->m_length += GetCount;
	}
	return start;
}

int CL_ThreadCache::TempDeAllocate(void* ptr)
{
	if (!s_IsInited)
	{
		CreateCache();
	}

	if (m_GarbageQueue->GetRear() - m_GarbageQueue->GetFront() < g_SizeOfQueue)
	{
		m_GarbageQueue->Enqueue(ptr);
		int temp = m_GarbageCount;
		m_GarbageCount.compare_exchange_weak(temp, m_GarbageCount + 1);
	}
	return 0;
}

int CL_ThreadCache::CollectGarbage()
{
	while(!m_GarbageQueue->IsEmpty() || m_GarbageCount <= 0)
	{
		Small* temp = (Small*)(m_GarbageQueue->Dequeue());
		size_t cl = CL_Static::SizeMap()->GetClass(temp->size);
		void* ptr = (void*)(reinterpret_cast<uintptr_t>(temp) + sizeof(Small));
		DeAllocate(temp, cl);
		m_GarbageCount--;
	}
	return 0;
}

int CL_ThreadCache::DeAllocate(void* ptr, size_t class_size)
{
	CL_FreeList* list = &m_list[class_size];
	int N = 0;
	
	if(ptr != list->Next())
	{
		list->Push(ptr);
		list->m_length++;
		
		if(list->m_length > list->m_MaxLength)
		{
			if(m_IsFreeTooMany)
			{
				N = list->m_length;
				list->m_MaxLength /= 2;
			}
			else
			{
				N = list->m_length/2;
				m_IsFreeTooMany = true;
			}
		}
		else
		{
			N = 1;
		}
	}
	
	ReleaseToCentralCache(list, class_size, N);
	return 0;
}

int CL_ThreadCache::ReleaseToCentralCache(CL_FreeList* list, size_t class_size, int number)
{
	if((unsigned)number > list->Length())
	{
		number = list->Length();
	}
	
	void *tail, *head;
	list->PopRange(number, &head, &tail);
	CL_Static::CentralCache()[class_size].InsertRange(head, tail, number, this);
	return 0;
}

CL_ThreadCache* CL_ThreadCache::GetThreadHeap()
{
	return threadlocal_data_.heap;
}

CL_ThreadCache* CL_ThreadCache::GetCache()
{
	CL_ThreadCache* ptr = NULL;
	if(!s_IsInited)
	{
		InitModule();
		s_IsInited = true;
	}
	else
	{
		ptr = GetThreadHeap();
	}
	
	if(ptr == NULL)
	{
		ptr = CreateCache();
	}
	return ptr;
}

int CL_ThreadCache::InitModule()
{
	if(!phinited)
	{
	    CL_Static::InitStatic();
	    ThreadcacheAllocator.Init();
	    phinited = true;
	}
	return 0;
}

int CL_ThreadCache::BecomeIdle()
{
	if(!s_IsInited)
	{
		return 0;
	}
	
	CL_ThreadCache* heap = GetThreadHeap();
	if(heap == NULL)
	{
		return 0;
	}
	
	threadlocal_data_.heap = NULL;
	if(GetThreadHeap() == heap)
	{
		return 0;
	}
	
	DeleteCache(heap);
	return 0;
}

int CL_ThreadCache::DestroyThreadCache(void* ptr)
{
	if(ptr == NULL)
	{
		return -1;
	}
	threadlocal_data_.heap = NULL;
	DeleteCache(reinterpret_cast<CL_ThreadCache*>(ptr));
	return 0;
}

int CL_ThreadCache::DeleteCache(CL_ThreadCache* heap)
{
	heap->ClearUp();
	
	{
		CL_CriticalSection cs;
		if(heap->m_pNext != NULL)
		{
			heap->m_pNext->m_pPrev = heap->m_pPrev;
		}
		if(heap->m_pPrev != NULL)
		{
			heap->m_pPrev->m_pNext = heap->m_pNext;
		}
		if(s_pThreadHeaps == heap)
		{
			s_pThreadHeaps = heap->m_pNext;
		}
		s_ThreadHeapsCount--;
	}
	
	ThreadcacheAllocator.Delete(heap);
	return 0;
}
/*
bool CL_ThreadCache::Check(void* p)
{
if (p >= (void*)0xF0000000)
{
return true;
}
return false;
}
*/