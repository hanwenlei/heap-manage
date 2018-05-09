#ifndef _LIBCOVERRIDEGCC_H_
#define _LIBCOVERRIDEGCC_H_

#ifdef HAVE_SYS_CDEFS_H
#include<sys/cdefs.h>
#endif

#ifndef __THROW
#define __THROW
#endif

#ifndef __GNUC__
#error LibcOverrideGcc.h is for gcc distributions only.
#endif

#define ALIAS(my_fn)   __attribute__ ((alias (#my_fn)))

extern "C"
{
	void* malloc(size_t size) __THROW               ALIAS(my_malloc);
	void free(void* ptr) __THROW                    ALIAS(my_free);
}

#undef ALIAS

#endif