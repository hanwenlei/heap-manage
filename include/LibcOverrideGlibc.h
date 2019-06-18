#ifndef _LIBCOVERRIDEGLIBC_H_
#define _LIBCOVERRIDEGLIBC_H_

#include"MyMalloc.h"
#ifdef HAVE_FEATURES_H
#include<config.h>
#include<features.h>
#endif
#ifdef HAVE_SYS_CDEFS_H
#include <sys/cdefs.h>
#endif

#ifndef __GLIBC__
#endif

#if !defined(__GNUC__)
#include"LibcOverrideRedefine.h"

#else
#include"LibcOverrideGcc.h"

#define ALIAS(my_fn)   __attribute__ ((alias (#my_fn)))

extern "C"
{
	void* __libc_malloc(size_t size)                ALIAS(my_malloc);
	void __libc_free(void* ptr)                     ALIAS(my_free);
}
#undef ALIAS

#endif

extern "C"
{
	static void* glibc_override_malloc(size_t size, const void *caller)
	{
		return my_malloc(size);
	}

	static void glibc_override_free(void *ptr, const void *caller)
	{
		my_free(ptr);
	}
}

#endif
