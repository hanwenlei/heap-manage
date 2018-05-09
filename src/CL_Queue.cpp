#include"CL_Queue.h"

CL_Queue::CL_Queue(){}

int CL_Queue::InitQueue()
{
	m_front = 0;
	m_rear = 0;
	for(int i = 0; i < g_SizeOfQueue; i++)
	{
		m_ElementPool[i].Element = NULL;
		m_ElementPool[i].IsEmpty = true;
	}
	return 0;
}

int CL_Queue::Enqueue(void* pElement)
{
	bool expected = true;
	long temp = m_rear;
	do
    {
        if((m_rear+1) % g_SizeOfQueue == m_front)
        {
            return -1;
        }
	}while(!m_ElementPool[m_rear%g_SizeOfQueue].IsEmpty.compare_exchange_weak(expected, false));
    m_ElementPool[m_rear%g_SizeOfQueue].Element = pElement;
    m_rear.compare_exchange_weak(temp, (m_rear + 1) % g_SizeOfQueue);
    return 0;
}

void* CL_Queue::Dequeue()
{
	bool expected = false;
	long temp = m_front;
	do
    {
        if(m_rear == m_front)
        {
        	#if _DEBUG
			printf("error in CL_Queue::Dequeue\n");
			#endif
            return NULL;
        }
    }while(!m_ElementPool[m_rear%g_SizeOfQueue].IsEmpty.compare_exchange_weak(expected, true));
	
    void* pElement = m_ElementPool[m_front%g_SizeOfQueue].Element;
	m_front.compare_exchange_weak(temp, (m_front + 1) % g_SizeOfQueue);
    return pElement;
}

bool CL_Queue::IsEmpty()
{
	if(m_rear == m_front)
	{
		return true;
	}
	return false;
}

int CL_Queue::GetFront()
{
	return m_front;
}

int CL_Queue::GetRear()
{
	return m_rear;
}