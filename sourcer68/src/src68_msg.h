/**
 * @ingroup  sourcer68_prg
 * @file     src68_msg.h
 * @author   Benjamin Gerard
 * @date     2015-03-22
 * @brief    print messages.
 *
 */

/* Copyright (c) 1998-2016 Benjamin Gerard */

#ifndef SRC68_MSG_H
#define SRC68_MSG_H

#include "src68_def.h"
#include <stdarg.h>

/**
 * Defines FMT23 to declare printf-like function for some compilers
 * that able to check format error at compile time.
 */
#ifndef DECL12
# if defined(__GNUC__) || defined(__clang__)
/* Syntax for GCC (assuming recent enough) and clang */
#  define DECL12(T,N,A)   T N(A,...)   __attribute__((format(printf,1,2)))
#  define DECL23(T,N,A,B) T N(A,B,...) __attribute__((format(printf,2,3)))
# elif defined(_MSC_VER) && (_MSC_VER >= 1400)
/* Syntax for MsVC (not tested) */
#  if _MSC_VER > 1400
#   define DECL12(T,N,A)   T N(_Printf_format_string_ A,...)
#   define DECL23(T,N,A,B) T N(_Printf_format_string_ A,B,...)
#  else
#   define DECL12(T,N,A)   T N(__format_string A,...)
#   define DECL23(T,N,A,B) T N(__format_string A,B,...)
#  endif
/* Fallback to no special declaration */
# else
#  define DECL12(T,N,A)   T N(A,...)
#  define DECL23(T,N,A,B) T N(A,B,...)
# endif
#endif

void emsg_va(int addr, const char * fmt, va_list list);
DECL23(void,emsg,int addr,const char * fmt);

void wmsg_va(int addr, const char * fmt, va_list list);
DECL23(void,wmsg,int addr, const char * fmt);

void dmsg_va(const char * fmt, va_list list);
DECL12(void,dmsg,const char * fmt);

#ifndef DMSG

# ifndef CPP_HAS_VAMACROS
#  if defined(__GNUC__) || defined(__clang__) || defined(_MSC_VER)
#   define CPP_HAS_VAMACROS 1
#  else
#   define CPP_HAS_VAMACROS 0
#  endif
# endif

# ifdef DEBUG
#  define DMSG dmsg
# elif CPP_HAS_VAMACROS
#  define DMSG(fmt,...) do{}while(0)
# else
#  define DMSG fake_dmsg
DECL12(static void,fake_dmsg,const char * fmt) {}
# endif

#endif  /* ifndef DMSG */

#endif  /* ifndef SRC68_MSG_H */
