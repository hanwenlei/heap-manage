#ifndef _LIBCOVERRIDEREDEFINE_H_
#define _LIBCOVERRIDEREDEFINE_H_

#define WIN32_DO_PATCHING

#ifdef HAVE_SYS_CDEFS_H
#include<sys/cdefs.h>
#endif

#ifndef __THROW
#define __THROW
#endif

#ifndef WIN32_DO_PATCHING
extern "C" 
{
	void* malloc(size_t s) __THROW;
	
	void  free(void* p) __THROW;
}
#endif

#endif