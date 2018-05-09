#ifndef _CL_QUEUE_H_
#define _CL_QUEUE_H_

#include"CL_SizeMap.h"
#include"CL_StorageStruct.h"

static const int g_SizeOfQueue = 2546;

class CL_Queue
{
private:
	atomic_long m_front;
	atomic_long m_rear;
	Node m_ElementPool[g_SizeOfQueue];
	
public:
	CL_Queue();
	int InitQueue();
	int Enqueue(void* pElement);
	void* Dequeue();
	int GetFront();
	int GetRear();
	bool IsEmpty();
};

#endif