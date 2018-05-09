#include"CL_CriticalSection.h"

CL_CriticalSection::CL_CriticalSection()
{
	m_pCS = (PCRITICAL_SECTION)malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(m_pCS);
	EnterCriticalSection(m_pCS);
}

CL_CriticalSection::~CL_CriticalSection()
{
	LeaveCriticalSection(m_pCS);
	DeleteCriticalSection(m_pCS);
	free(m_pCS);
}