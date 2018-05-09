#ifndef _CL_MANAGERFUNCTION_H_
#define _CL_MANAGERFUNCTION_H_

class CL_ManagerFunction
{
public:
	CL_ManagerFunction();
	virtual ~CL_ManagerFunction();
	virtual unsigned RunFunction(void* pParam) = 0;
	
private:
	CL_ManagerFunction(const CL_ManagerFunction&);
	CL_ManagerFunction& operator= (const CL_ManagerFunction&);
};

#endif