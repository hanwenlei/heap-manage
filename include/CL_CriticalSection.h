#ifndef _CL_CRITICALSECTION_H_
#define _CL_CRITICALSECTION_H_

#include<windows.h>

class CL_CriticalSection
{
private:
	PCRITICAL_SECTION m_pCS;
	CL_CriticalSection(const CL_CriticalSection&);
	CL_CriticalSection& operator= (const CL_CriticalSection&);

public:
	CL_CriticalSection();
	virtual ~CL_CriticalSection();
};

#endif