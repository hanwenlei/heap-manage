#ifndef _CL_MANAGER_H_
#define _CL_MANAGER_H_

#include"CL_ManagerFunction.h"

class CL_Manager
{
private:
	CL_Manager(const CL_Manager&);
	CL_Manager& operator= (const CL_Manager&);
	
protected:
	CL_ManagerFunction* m_pManagerFunction;

public:
	CL_Manager(CL_ManagerFunction* pManagerFunction);
	virtual ~CL_Manager();
	virtual int Run(void* pParam) = 0;
	virtual int PrepareToTerminate() = 0;
};

#endif