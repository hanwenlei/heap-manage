#include"CL_Event.h"

CL_Event::CL_Event(){}

CL_Event::~CL_Event()
{
	CloseHandle(m_event);
}

HANDLE CL_Event::CreateEv(PSECURITY_ATTRIBUTES psa, BOOL bManualReset, BOOL bInitialState, LPCTSTR pszName)
{
	m_event = (HANDLE)CreateEvent(psa, bManualReset, bInitialState, pszName);
	return m_event;
}

HANDLE CL_Event::CreateEv(PSECURITY_ATTRIBUTES psa, LPCTSTR pszName, DWORD dwFlags, DWORD dwDesiredAccess)
{
	m_event = (HANDLE)CreateEventEx(psa, pszName, dwFlags, dwDesiredAccess);
	return m_event;
}

int CL_Event::SetEv()
{
	return SetEvent(m_event);
}

int CL_Event::ResetEv()
{
	return ResetEvent(m_event);
}

int CL_Event::WaitEv(DWORD dwMilliseconds)
{
	WaitForSingleObject(m_event, dwMilliseconds);
	return 0;
}