#ifndef _LIBCOVERRIDEGLIBC_H_
#define _LIBCOVERRIDEGLIBC_H_

#include<config.h>
#include<features.h>
#ifdef HAVE_SYS_CDEFS_H
#include <sys/cdefs.h>
#endif

#ifndef __GLIBC__
#error LibcOverrideGlibc.h is for glibc distributions only.
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