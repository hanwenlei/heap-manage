#ifndef _CL_THREADCACHE_H_
#define _CL_THREADCACHE_H_

#include"CL_FreeList.h"
#include"CL_PageHeapAllocator.h"
#include"CL_Queue.h"

class CL_ThreadCache
{
private:
	DWORD m_ThreadId;
	CL_Queue* m_GarbageQueue;
	atomic_int m_GarbageCount;
	bool m_IsFreeTooMany;
	
private:
	int InitionQueue();
	void* GetFromCentralCache(size_t size, size_t class_size);
	int ReleaseToCentralCache(CL_FreeList* list, size_t class_size, int number);
	
public:
	CL_ThreadCache* m_pNext;
	CL_ThreadCache* m_pPrev;
	CL_FreeList m_list[kNumClasses];
	static int s_ThreadHeapsCount;
	static CL_ThreadCache* s_pThreadHeaps;
	static bool s_IsInited;
	
	#ifdef HAVE___ATTRIBUTE__
	#define ATTR_INITIAL_EXEC __attribute__ ((tls_model ("initial-exec")))
	#else
	#define ATTR_INITIAL_EXEC
	#endif
	struct ThreadLocalData
	{
    	CL_ThreadCache* heap;
	};
	static __declspec(thread) ThreadLocalData threadlocal_data_ ATTR_INITIAL_EXEC;
	
public:
	CL_ThreadCache();
	int Init(DWORD ThreadId);
	int ClearUp();
	int FreeListLength(size_t class_size) const {return m_list[class_size].Length();}
	void* Allocate(size_t size, size_t class_size);
	int DeAllocate(void* ptr, size_t class_size);
	int TempDeAllocate(void* ptr);
	int CollectGarbage();

public:
	static int InitModule();
	static CL_ThreadCache* CreateCache();
	static CL_ThreadCache* NewCache(DWORD ThreadId);
	static CL_ThreadCache* GetThreadHeap();
	static CL_ThreadCache* GetCache();
	static int BecomeIdle();
	static int DestroyThreadCache(void* ptr);
	static int DeleteCache(CL_ThreadCache* heap);
};

extern CL_PageHeapAllocator<CL_ThreadCache> ThreadcacheAllocator;
extern CL_PageHeapAllocator<CL_Queue> QueueAllocator;

#endif