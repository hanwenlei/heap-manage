#include"CL_SizeMap.h"

HANDLE CL_SizeMap::s_Heap;

CL_SizeMap::CL_SizeMap(){}

int CL_SizeMap::Init()
{
	int step = 0;
	CL_SizeMap::s_Heap = HeapCreate(0, 64*SizeOfPage, 0);
	for(int i = 0; i < kNumClasses; i++)
	{
		if(i < 16)
		{
			step += 8;
			m_SizeClasses[i] = step;
		}
		else if(i >= 16 && i < 45)
		{
			step += 32;
			m_SizeClasses[i] = step;
		}
		else
		{
			step += 512;
			m_SizeClasses[i] = step;
		}
	}
	return 0;
}

int CL_SizeMap::GetClass(int size)
{
	int result = 0;
	if (size >= m_SizeClasses[kNumClasses - 1])
	{
		return 0;
	}
	else if(size <= m_SizeClasses[1])
	{
		return 1;
	}
	
	for(int i = 2; i < kNumClasses; i++)
	{
		if(size > m_SizeClasses[(i-1)%kNumClasses] && size <= m_SizeClasses[i%kNumClasses])
		{
			result = i;
			break;
		}
	}
	return result;
}

int CL_SizeMap::GetSize(size_t cl)
{
	return m_SizeClasses[cl];
}