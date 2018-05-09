#include"CL_PageHeap.h"
#include"CL_Static.h"
#include"CL_PageHeapAllocator.h"
#include"CL_ThreadCache.h"

CL_PageHeap::CL_PageHeap(){}

int CL_PageHeap::Init()
{
	m_HugeNumber = 0;
	m_PageNumber = 0;
	m_pHugeList = NULL;
	SP_Init(&m_AlignSpanList);
	SP_Init(&m_RecycleSpanList);

	for (int i = 0; i < 4; i++)
	{
		void* p = HeapAlloc(CL_SizeMap::s_Heap, HEAP_ZERO_MEMORY, SizeOfPage * 16);
		Span* NewSpan = CL_Static::SpanAllocator()->New();
		NewSpan->StartAddress = NewSpan->CurentPoint = reinterpret_cast<uintptr_t>(p);
		NewSpan->objects = NULL;
		NewSpan->flag = false;

		Span* temp = &m_AlignSpanList;
		if (temp->next == temp)
		{
			m_AlignSpanList = *NewSpan;
			NewSpan->next = NULL;
			NewSpan->prev = NULL;
			NewSpan->location = NewSpan->ON_ALIGN_FREELIST;
		}
		else
		{
			NewSpan->next = temp->GetNext();
			NewSpan->prev = temp;
			temp = NewSpan;
			NewSpan->location = NewSpan->ON_ALIGN_FREELIST;
		}
	}
	return 0;
}

Span* CL_PageHeap::NewPage(size_t n)
{
	Span* result = SearchAlignSpanList(n);
	if(result != NULL)
	{
		result->location = result->IN_USE;
		return result;
	}
	
	result = SearchRecycleSpanList(n);
	if(result != NULL)
	{
		result->location = result->IN_USE;
		return result;
	}
	
	if(!GrowHeap(n))
	{
		#if _DEBUG
		printf("error in CL_PageHeap::NewPage\n");
		#endif
		return NULL;
	}
	
	result = SearchAlignSpanList(n);
	if(result != NULL)
	{
		result->location = result->IN_USE;
	}
	return result;
}

Huge* CL_PageHeap::AllocHuge(size_t size)
{
	Span* pSpan = SearchRecycleSpanList(size/SizeOfPage+1);
	Huge* result = reinterpret_cast<Huge*>(pSpan->StartAddress);
	if(result == NULL)
	{
		return NULL;
	}
	
	m_HugeNumber++;
	Huge* temp = m_pHugeList;
	while(temp->NextPtr != NULL)
	{
		temp = temp->NextPtr;
	}
	temp->NextPtr = result;
	result->NextPtr = NULL;
	
	return result;
}

int CL_PageHeap::FreeHuge(void* pElement)
{
	Huge* Element = (Huge*)pElement;
	Span* temp = &m_RecycleSpanList;
	uintptr_t start = reinterpret_cast<uintptr_t>(pElement);
	
	while(temp != NULL)
	{
		if(temp->StartAddress >= start)
		{
			break;
		}
		temp = temp->next;
		if (temp == temp->next)
		{
			temp = NULL;
		}
	}
	start = start + (uintptr_t)Element->size;
	while(temp != NULL)
	{
		if(temp->StartAddress >= start)
		{
			break;
		}
		temp->location = temp->ON_RECYCLE_FREELIST;
		temp = temp->next;
	}
	
	Huge* ptr = m_pHugeList;
	while(ptr != NULL)
	{
		if(ptr->NextPtr != Element)
		{
			ptr = ptr->NextPtr;
		}
		else
		{
			ptr->NextPtr = Element->NextPtr;
			Element->NextPtr->PrevPtr = ptr;
			Element->NextPtr = NULL;
			Element->PrevPtr = NULL;
		}
	}
	return 0;
}

Span* CL_PageHeap::SearchAlignSpanList(size_t n)
{
	Span* temp = &m_AlignSpanList;
	if(temp->next == temp)
	{
		return NULL;
	}
	
	while(temp != NULL)
	{
		uintptr_t stay = temp->StartAddress + (uintptr_t)(SizeOfPage*16) - temp->CurentPoint;
		if(stay >= (uintptr_t)(n*SizeOfPage))
		{
			break;
		}
		else
		{
			temp = temp->next;
		}
	}

	if (temp != NULL)
	{
		m_PageNumber++;
		temp->location = temp->IN_USE;
	}
	return temp;
}

Span* CL_PageHeap::SearchRecycleSpanList(size_t n)
{
	Span* temp = &m_RecycleSpanList;
	if(temp->next == temp)
	{
		return NULL;
	}
	
	unsigned int step = SizeOfPage*16;
	int i = 1;
	while(temp != NULL)
	{
		if(temp->location == temp->IN_USE)
		{
			step = SizeOfPage*16;
			i = 1;
			temp = temp->next;
			continue;
		}
		
		if(step >= n)
		{
			break;
		}
		else
		{
			if(temp->StartAddress + (uintptr_t)step != temp->GetNext()->StartAddress)
			{
				step = SizeOfPage*16;
				i = 1;
			}
			else
			{
				i++;
				step *= i;
			}
			temp = temp->next;
		}
	}
	
	if(temp != NULL)
	{
		while(i--)
		{
			temp = temp->prev;
			temp->location = temp->IN_USE;
			m_PageNumber++;
		}
	}
	return temp;
}

bool CL_PageHeap::GrowHeap(size_t n)
{
	if(!EnsureLimit(n))
	{
		return false;
	}
	void* p = HeapAlloc(CL_SizeMap::s_Heap, HEAP_ZERO_MEMORY, SizeOfPage*16);
	if(p == NULL)
	{
		return false;
	}
	
	Span* NewSpan = CL_Static::SpanAllocator()->New();
	NewSpan->StartAddress = NewSpan->CurentPoint = reinterpret_cast<uintptr_t>(p);
	NewSpan->objects = NULL;
	NewSpan->flag = false;
	
	Span* temp = &m_AlignSpanList;
	if(temp->next == temp)
	{
		m_AlignSpanList = *NewSpan;
		NewSpan->next = NULL;
		NewSpan->prev = NULL;
		NewSpan->location = NewSpan->ON_ALIGN_FREELIST;
		return true;
	}
	
	while(temp != NULL)
	{
		if(NewSpan->StartAddress <= temp->StartAddress)
		{
			NewSpan->next = temp;
			NewSpan->prev = temp->GetPrev();
			temp->GetPrev()->next = NewSpan;
			temp->prev = NewSpan;
			NewSpan->location = NewSpan->ON_ALIGN_FREELIST;
			break;
		}
		else
		{
			temp = temp->next;
		}
	}

	return true;
}

int CL_PageHeap::ReleaseToRecycle(Span* pElement)
{
	Span* result = pElement;
	SP_Remove(&m_AlignSpanList, pElement);
	
	Span* temp = &m_RecycleSpanList;
	while((result->StartAddress > temp->StartAddress) && temp != NULL)
	{
		temp = temp->next;
	}
	
	if(temp != NULL)
	{
		result->prev = temp->GetPrev();
		result->next = temp;
		result->GetPrev()->next = result;
		temp->prev = result;
	}
	else
	{
		temp->next = result;
		result->prev = temp;
		result->next = NULL;
	}
	
	result->location = result->ON_RECYCLE_FREELIST;
	return 0;
}

bool CL_PageHeap::EnsureLimit(size_t n)
{
	if(n > 128)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int CL_PageHeap::RemoveFromCentralCache(Span* pElement)
{
	Span* temp = &m_AlignSpanList;
	while((pElement->StartAddress > temp->StartAddress) && temp != NULL)
	{
		temp = temp->next;
	}
	
	if(temp->GetNext() != NULL)
	{
		pElement->prev = temp->GetPrev();
		pElement->next = temp;
		temp->prev = pElement;
		temp->GetPrev()->next = pElement;
	}
	else
	{
		pElement->prev = temp;
		pElement->next = NULL;
		temp->next = pElement;
	}
	
	pElement->location = pElement->ON_ALIGN_FREELIST;
	MergeSpan(pElement);
	return 0;
}

int CL_PageHeap::MergeSpan(Span* pElement)
{
	Span* temp1 = pElement->prev;
	Span* temp2 = pElement->next;
	if((temp1 == NULL) || (temp2 == NULL))
	{
		return -1;
	}
	
	if((temp1->CurentPoint == temp1->StartAddress) && (temp2->CurentPoint == temp2->StartAddress))
	{
		if(((temp1->StartAddress + (uintptr_t)(SizeOfPage*16)) == pElement->StartAddress) &&
		((pElement->StartAddress + (uintptr_t)(SizeOfPage*16)) == temp2->StartAddress))
		{
			ReleaseToRecycle(temp1);
			ReleaseToRecycle(pElement);
			ReleaseToRecycle(temp2);
		}
	}
	return 0;
}