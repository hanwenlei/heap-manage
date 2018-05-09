#ifndef _CL_STATIC_H_
#define _CL_STATIC_H_

#include"CL_CentralCache.h"
#include"CL_PageHeap.h"
#include"CL_PageHeapAllocator.h"

class CL_Static
{
private:
	static CL_SizeMap s_SizeMap;
	static CL_CentralCache s_CentralCache[kNumClasses];
	static CL_PageHeapAllocator<Span> s_SpanAllocator;
	static CL_PageHeapAllocator<CL_PageHeap> s_PageHeapAllocator;
	static CL_PageHeap* s_pPageHeap;
	
public:
	static Large* s_pFreeList;
	static Large* s_pFragmentList;

public:
	static int InitStatic();
	static CL_CentralCache* CentralCache();
	static CL_SizeMap* SizeMap();
	static CL_PageHeap* PageHeap();
	static CL_PageHeapAllocator<Span>* SpanAllocator();
	static CL_PageHeapAllocator<CL_PageHeap>* PageHeapAllocator();
	static bool IsInited();
};

#endif