#ifndef _CL_CENTRALCACHE_H_
#define _CL_CENTRALCACHE_H_

#include"CL_SizeMap.h"
#include"CL_CriticalSection.h"
#include"CL_Span.h"

class CL_ThreadCache;

class CL_CentralCache
{
private:
	size_t m_SizeClass;
	atomic_int m_SpansNumber;
	atomic_int m_EmptyMemoryNumber;
	Span m_EmptySpanList;
	Span m_NonEmptySpanList;
	Span* m_pCurrentSpan;
	
private:
	void* GetFromSpansCurrent();
	void* GetFromSpansList();
	int GetFromPage();
	void* GetNext(void* pElement);
	int ReleaseListToSpans(void* start, CL_ThreadCache* ThreadCache);
	int ReleaseToSpans(void* object, CL_ThreadCache* ThreadCache);
//	int RemoveToEmptySpanList(Span* pSpan);
	Span* MapObjectToSpan(void* object);
	
public:
	CL_CentralCache();
	int Init(size_t class_size);
	int InsertRange(void* start, void* end, int number, CL_ThreadCache* ThreadCache);
	int RemoveRange(void** start, void** ends, int number);
	int GetLength();
	void* LargeAlloc(size_t size);
};

#endif