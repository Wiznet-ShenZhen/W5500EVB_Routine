/* time.h standard header */
#ifndef _TIME
#define _TIME

//#ifndef _SYSTEM_BUILD
//  #pragma system_include
//#endif

#include <ycheck.h>
#include <yvals.h>
#include <ysizet.h>

_C_STD_BEGIN

#if __AEABI_PORTABILITY_INTERNAL_LEVEL
  #ifndef __AEABI_PORTABLE
    #define __AEABI_PORTABLE
  #endif

  #pragma language=save
  #pragma language=extended
  extern _DLIB_DATA_ATTR int const __aeabi_CLOCKS_PER_SEC;
  #pragma language=restore
  #define CLOCKS_PER_SEC  (__aeabi_CLOCKS_PER_SEC)

#else /* __AEABI_PORTABILITY_INTERNAL_LEVEL */

  #define CLOCKS_PER_SEC  1

#endif /* __AEABI_PORTABILITY_INTERNAL_LEVEL */


#if  _DLIB_SUPPORT_FOR_AEABI

  typedef unsigned int time_t;
  typedef unsigned int clock_t;

#else /* _DLIB_SUPPORT_FOR_AEABI */

  typedef signed long time_t;
  typedef signed long clock_t;

#endif /* _DLIB_SUPPORT_FOR_AEABI */

struct tm
{       /* date and time components */
  int tm_sec;
  int tm_min;
  int tm_hour;
  int tm_mday;
  int tm_mon;
  int tm_year;
  int tm_wday;
  int tm_yday;
  int tm_isdst;
#if  _DLIB_SUPPORT_FOR_AEABI
  int __BSD_bug_filler1;
  int __BSD_bug_filler2;
#endif /* _DLIB_SUPPORT_FOR_AEABI */
};

#ifndef NULL
  #define NULL   _NULL
#endif /* NULL */


_EXTERN_C       /* low-level functions */
__ATTRIBUTES time_t time(time_t *);
_END_EXTERN_C

_C_LIB_DECL     /* declarations */
__ATTRIBUTES char * asctime(const struct tm *);
__ATTRIBUTES clock_t clock(void);
__ATTRIBUTES char * ctime(const time_t *);
__ATTRIBUTES double difftime(time_t, time_t);
__ATTRIBUTES struct tm * gmtime(const time_t *);
__ATTRIBUTES struct tm * localtime(const time_t *);
__ATTRIBUTES time_t mktime(struct tm *);
__ATTRIBUTES size_t strftime(char *, size_t, const char *,
        const struct tm *);
_END_C_LIB_DECL
_C_STD_END
#endif /* _TIME */

#if defined(_STD_USING) && defined(__cplusplus)
  using _CSTD clock_t;
  using _CSTD time_t;
  using _CSTD tm;
  using _CSTD asctime; using _CSTD clock; using _CSTD ctime;
  using _CSTD difftime; using _CSTD gmtime; using _CSTD localtime;
  using _CSTD mktime; using _CSTD strftime; using _CSTD time;
#endif /* defined(_STD_USING) && defined(__cplusplus) */

/*
 * Copyright (c) 1992-2002 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
V3.12:0576 */
