#ifndef _CL_THREAD_H_
#define _CL_THREAD_H_

#include"CL_Manager.h"
#include"CL_Event.h"

class CL_Thread : public CL_Manager
{
private:
	bool m_AutoDelete;
	bool m_run;
	HANDLE m_thread;
	DWORD m_ThreadId;
	void* m_pvParam;
	
	CL_Event m_WaitForNewThread;
	CL_Event m_WaitForOldThread;
	static unsigned int RunThreadFunc(void* pParam);

public:
	CL_Thread(CL_ManagerFunction* pManagerFunction, bool pAutoDelete);
	virtual ~CL_Thread();
	virtual int Run(void* pParam);
	virtual int PrepareToTerminate();
	
public:
	int GetThreadPrio();
    int SetThreadPrio(int pPriority);
    HANDLE GetHandle();
    DWORD GetID();
	void* GetParam();
    int SetParam(void* pParam);
};

#endif