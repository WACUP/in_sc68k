dnl# -*- mode:sh; sh-basic-offset:2; indent-tabs-mode:nil -*-
dnl#
dnl# autoconf macros
dnl#
dnl# (C) 2009-2016 Benjamin Gerard <http://sourceforge.net/users/benjihan>
dnl#
dnl# Distribued under the term of the GPL3+

# serial 20160824 sc68_cc.m4

# SC68_CC()
# -------
# Check C compiler presence and features.
AC_DEFUN_ONCE([SC68_CC],
  [
    AC_LANG([C])
    AC_PROG_CC
dnl AM_PROG_CC_STDC
    AM_PROG_CC_C_O
    AC_C_CONST
    AC_C_INLINE
    AC_C_VOLATILE
    AC_C_RESTRICT
  ])

# SC68_CXX()
# ---------
# Check C++ compiler presence and features.
AC_DEFUN_ONCE([SC68_CXX],
  [
    AC_LANG([C++])
    AC_PROG_CXX
    AC_C_CONST
    AC_C_INLINE
    AC_C_VOLATILE
    AC_C_RESTRICT
  ])


dnl# ----------------------------------------------------------------------
dnl#
dnl# End Of sc68_cc.m4
dnl#
dnl# ----------------------------------------------------------------------
