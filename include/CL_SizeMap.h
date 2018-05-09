#ifndef _CL_SIZEMAP_H_
#define _CL_SIZEMAP_H_

#include<stddef.h>
#include<stdint.h>
#include<vector>
#include<windows.h>

using namespace std;

#if __WORDSIZE == 64
# ifndef __intptr_t_defined
typedef long int               intptr_t;
#  define __intptr_t_defined
typedef unsigned long int    uintptr_t;
# endif
#else
# ifndef __intptr_t_defined
typedef int                    intptr_t;
#  define __intptr_t_defined
typedef unsigned int        uintptr_t;
# endif
#endif

static const int kNumClasses = 51;
static const int SizeOfPage = 4096;
static const int kMaxSize = 32*1024;

class CL_SizeMap
{
private:
	vector<uintptr_t> m_SpansStartAddress;
	int m_SizeClasses[kNumClasses];
	
public:
	static HANDLE s_Heap;
	CL_SizeMap();
	int Init();
	int GetClass(int size);
	int GetSize(size_t cl);
};

#endif