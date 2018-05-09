#include"LinkedList.h"
#include<Windows.h>

void* SL_Next(void* t)
{
	return *(reinterpret_cast<void**>(t));
}

int SL_SetNext(void* t, void* n)
{
	if (n == NULL)
	{
		return -1;
	}
	*(reinterpret_cast<void**>(t)) = n;
	return 0;
}

int SL_Push(void** list, void* element)
{
	SL_SetNext(element, *list);
	*list = element;
	return 0;
}

void* SL_Pop(void** list)
{
	void *result = *list;
	void* temp = SL_Next(*list);
	*list = temp;
	return result;
}

int SL_PopRange(void** head, int number, void** start, void** end)
{
	if(number <= 0)
	{
		*start = NULL;
    	*end = NULL;
    	return 0;
	}

	void* tmp = *head;
	for(int i = 1; i < number; i++)
	{
		tmp = SL_Next(tmp);
	}

	*start = *head;
	*end = tmp;
	*head = SL_Next(tmp);
	SL_SetNext(tmp, NULL);
	return 0;
}

int SL_PushRange(void** head, void** start, void** end)
{
	if(!start)
	{
		return -1;
	}
	SL_SetNext(end, *head);
	*head = start;
	return 0;
}

bool CheckMemory(void* pElement)
{
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetCurrentProcessId());
	MEMORY_BASIC_INFORMATION mbi;
	VirtualQueryEx(hProcess, pElement, &mbi, sizeof(mbi));
	if (mbi.State == MEM_FREE)
	{
		return false;
	}
	return true;
}

size_t SL_GetSize(void* head)
{
	int count = 0;
	while(head)
	{
		count++;
    	head = SL_Next(head);
	}
	return count;
}