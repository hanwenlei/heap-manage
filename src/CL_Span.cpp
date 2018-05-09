#include<string.h>
#include"CL_Static.h"

Span* Span::GetNext()
{
	Span* temp = next;
	return temp;
}

Span* Span::GetPrev()
{
	Span* temp = prev;
	return temp;
}

Span* NewSpan()
{
	Span* result = CL_Static::SpanAllocator()->New();
	memset(result, 0, sizeof(result));
	result->StartAddress = result->CurentPoint = (uintptr_t)result;
	result->ObjectsNumber = 0;
	result->ReferenceCount = 0;
	return result;
}

int CS_Prepend(Span* list, Span* span)
{
	if (span->GetNext() == NULL || span->GetPrev() == NULL)
	{
		return -1;
	}
	span->next = list->GetNext();
	span->prev = list;
	list->GetNext()->prev = span;
	list->next = span;
	return 0;
}

int DeleteSpan(Span* span)
{
	CL_Static::SpanAllocator()->Delete(span);
	return 0;
}

int SP_Init(Span* list)
{
	if(list != NULL)
	{
		list->next = list;
		list->prev = list;
	}
	return 0;
}

int SP_Remove(Span* list, Span* span)
{
	Span *RightNodeNext;
	do
	{
		if (span == NULL)
		{
			return -1;
		}
		span->flag = true;
		RightNodeNext = span->next;
		if (RightNodeNext == NULL || span == span->next)
		{
			list = NULL;
			return 0;
		}
		if(!RightNodeNext->flag)
		{
			if(atomic_compare_exchange_weak(&(span->next), &RightNodeNext, GetUnmarked(RightNodeNext)))
			{
				break;
			}
		}
		span = span->next;
	}while(true);
	
	if(atomic_compare_exchange_weak(&(list->next), &span, RightNodeNext))
	{
		RightNodeNext->prev = list;
	}
	return 0;
}

Span* GetUnmarked(Span* span)
{
	Span* temp = span;
	if (temp == NULL)
	{
		return NULL;
	}
	if(!temp->flag)
	{
		temp = temp->next;
		if (temp == NULL || temp == temp->next)
		{
			return NULL;
		}
	}
	return temp;
}

bool SP_IsEmpty(const Span* list)
{
	return (list->next == list);
}

Span& Span::operator = (Span& pElenent)
{
	StartAddress = pElenent.StartAddress;
	CurentPoint = pElenent.CurentPoint;
	prev = pElenent.GetPrev();
	next = pElenent.GetNext();
	objects = pElenent.objects;
	ObjectsNumber = pElenent.ObjectsNumber;
	location = pElenent.location;
	int inttemp = pElenent.ReferenceCount;
	ReferenceCount = inttemp;
	bool booltemp = pElenent.flag;
	flag = booltemp;
	return *this;
}

int SP_Prepend(Span* list, Span* span)
{
	Span* RightNode;
	if (span->GetNext() == NULL || span->GetPrev() == NULL)
	{
		return -1;
	}
	do
	{
		RightNode = Search(list);
		if(RightNode == NULL)
		{
			return 0;
		}
		
		span->next = RightNode;
		span->prev = list;
		if(atomic_compare_exchange_weak(&(list->next), &RightNode, span))
		{
			RightNode->prev = span;
			return 0;
		}
	}while(true);
}

int SP_Length(const Span* list)
{
	int result = 0;
	for(Span* s = list->next; s != list; s = s->next)
	{
    	result++;
	}
	return result;
}

Span* Search(Span* list)
{
	Span *LeftNodeNext, *RightNode;
	search_again:
		do
		{
			Span* t = list;
			if (t == NULL)
			{
				return NULL;
			}
			Span* t_next = list->next;
			if (t_next == NULL)
			{
				return NULL;
			}
			
			do
			{
				if(!t_next->flag)
				{
					LeftNodeNext = t_next;
				}
				else
				{
					LeftNodeNext = list->next;
				}
				t = GetUnmarked(t_next);
				
				if(t == NULL || t == t_next)
				{
					return t;
				}
				t_next = t->next;
				if (t_next == NULL)
				{
					return t;
				}
			}while(t_next->flag);
			
			RightNode = t;
			if(LeftNodeNext == RightNode)
			{
				if((RightNode != NULL) && RightNode->flag)
				{
					goto search_again;
				}
				else
				{
					return RightNode;
				}
			}
			
			if(atomic_compare_exchange_weak(&(list->next), &LeftNodeNext, RightNode))
			{
				if((RightNode != NULL) && RightNode->flag)
				{
					goto search_again;
				}
				else
				{
					return RightNode;
				}
			}
		}while(true);
}