#ifndef _CL_EVENT_H_
#define _CL_EVENT_H_
#define CREATE_EVENT_INITIAL_SET 0x00000002

#include<windows.h>

class CL_Event
{
private:
	HANDLE m_event;
	
public:
	CL_Event();
	virtual ~CL_Event();
	HANDLE CreateEv(PSECURITY_ATTRIBUTES psa, BOOL bManualReset = FALSE, BOOL bInitialState = FALSE, LPCTSTR pszName = NULL);
	HANDLE CreateEv(PSECURITY_ATTRIBUTES psa = NULL, LPCTSTR pszName = NULL, DWORD dwFlags = CREATE_EVENT_INITIAL_SET, DWORD dwDesiredAccess = 0);
	int WaitEv(DWORD dwMilliseconds = INFINITE);
	int SetEv();
	int ResetEv();
};

#endif