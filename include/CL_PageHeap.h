#ifndef _CL_PAGEHEAP_H_
#define _CL_PAGEHEAP_H_

#include"CL_StorageStruct.h"
#include"CL_Span.h"

class CL_PageHeap
{
private:
	Span m_AlignSpanList;
	Span m_RecycleSpanList;
	Huge* m_pHugeList;
	unsigned int m_HugeNumber;
	unsigned int m_PageNumber;
	
private:
	Span* SearchAlignSpanList(size_t n);
	Span* SearchRecycleSpanList(size_t n);
	bool GrowHeap(size_t n);
	int MergeSpan(Span* pElement);
	bool EnsureLimit(size_t n);
	
public:
	CL_PageHeap();
	int Init();
	Span* NewPage(size_t n);
	Huge* AllocHuge(size_t size);
	int FreeHuge(void* pElement);
	int ReleaseToRecycle(Span* pElement);
	int RemoveFromCentralCache(Span* pElement);
};

#endif