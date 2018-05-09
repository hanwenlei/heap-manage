#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include<stddef.h>

void* SL_Next(void* t);

int SL_SetNext(void* t, void* n);

int SL_Push(void** list, void* element);

void* SL_Pop(void** list);

int SL_PopRange(void** head, int number, void** start, void** end);

int SL_PushRange(void** head, void** start, void** end);

bool CheckMemory(void* pElement);

size_t SL_GetSize(void* head);

#endif