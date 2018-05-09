#ifndef _CL_FREELIST_H_
#define _CL_FREELIST_H_

#include"LinkedList.h"

class CL_FreeList
{
private:
	void* m_pList;
	unsigned int m_length;
	unsigned int m_MaxLength;
	
public:
	CL_FreeList();
	int Init();
	size_t Length() const {return m_length;}
	size_t MaxLength() const {return m_MaxLength;}
	int SetMaxLength(size_t NewMax);
	bool IsEmpty() const {return (m_pList == NULL);}
	int Push(void* ptr);
	void* Pop();
	void* Next();
	int PushRange(int count, void* start, void* end);
	int PopRange(int count, void** start, void** end);
	
friend class CL_ThreadCache;
friend class CL_CentralCache;
};

#endif