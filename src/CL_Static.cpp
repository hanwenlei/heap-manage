#include"CL_Static.h"

CL_SizeMap CL_Static::s_SizeMap;
CL_CentralCache CL_Static::s_CentralCache[kNumClasses];
CL_PageHeapAllocator<Span> CL_Static::s_SpanAllocator;
CL_PageHeapAllocator<CL_PageHeap> CL_Static::s_PageHeapAllocator;
CL_PageHeap* CL_Static::s_pPageHeap = NULL;
Large* CL_Static::s_pFreeList = NULL;
Large* CL_Static::s_pFragmentList = NULL;

int CL_Static::InitStatic()
{
	s_SizeMap.Init();
	s_SpanAllocator.Init();
	s_SpanAllocator.New();
	s_SpanAllocator.New();

	for(int i = 0; i < kNumClasses; i++)
	{
		s_CentralCache[i].Init(i);
	}
	s_pPageHeap = s_PageHeapAllocator.New();
	s_pPageHeap->Init();
	return 0;
}

CL_CentralCache* CL_Static::CentralCache()
{
	return s_CentralCache;
}

CL_SizeMap* CL_Static::SizeMap()
{
	return &s_SizeMap;
}

CL_PageHeap* CL_Static::PageHeap()
{
	return s_pPageHeap;
}

CL_PageHeapAllocator<Span>* CL_Static::SpanAllocator()
{
	return &s_SpanAllocator;
}

CL_PageHeapAllocator<CL_PageHeap>* CL_Static::PageHeapAllocator()
{
	return &s_PageHeapAllocator;
}

bool CL_Static::IsInited()
{
	return (PageHeap() != NULL);
}