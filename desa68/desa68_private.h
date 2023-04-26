/**
 * @ingroup  lib_desa68
 * @file     desa68_private.h
 * @author   Benjamin Gerard
 * @date     2016/07/30
 * @brief    desa68 library private header.
 *
 *   Put anything here needed to compile this project. It's possible
 *   short circuit or add new things by defining PRIVATE_H a la
 *   config.h.
 *
 */

/* Copyright (c) 1998-2016 Benjamin Gerard */

#define BUILD_DESA68 1

#ifdef DESA68_H
# error desa68_private.h must be include before desa68.h
#endif

#ifdef DESA68_PRIVATE_H
# error desa68_private.h must be include once
#endif
#define DESA68_PRIVATE_H 1

#ifdef HAVE_PRIVATE_H
# include "private.h"
#endif

/* ---------------------------------------------------------------- */

#if defined(DEBUG) && !defined(_DEBUG)
# define _DEBUG DEBUG
#elif !defined(DEBUG) && defined(_DEBUG)
# define DEBUG _DEBUG
#endif

#ifndef DESA68_WIN32
# if defined(WIN32) || defined(_WIN32) || defined(__SYMBIAN32__)
#  define DESA68_WIN32 1
# endif
#endif

#if defined(DLL_EXPORT) && !defined(desa68_lib_EXPORTS)
# define desa68_lib_EXPORTS DLL_EXPORT
#endif

#ifndef DESA68_API
# if defined(DESA68_WIN32) && defined(desa68_lib_EXPORTS)
#  define DESA68_API __declspec(dllexport)
# endif
#endif

#if defined(__GNUC__) || defined(__clang__)
# pragma GCC diagnostic ignored "-Wmultichar"
#endif
