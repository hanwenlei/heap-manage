#ifndef _LIBCOVERRIDE_H_
#define _LIBCOVERRIDE_H_

#ifdef HAVE_FEATURES_H
#include<features.h>
#endif

#include"CL_Malloc.h"

#if defined(_WIN32)
#include"LibcOverrideRedefine.h"

#elif defined(__GLIBC__)
#include"LibcOverrideGlibc.h"

#elif defined(__GNUC__)
#include"LibcOverrideGcc.h"

#else
#error Need to add support to your libc here

#endif

#endif