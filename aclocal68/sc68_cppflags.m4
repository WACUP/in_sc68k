dnl# -*- mode:sh; sh-basic-offset:2; indent-tabs-mode:nil -*-
dnl#
dnl# autoconf macros
dnl#
dnl# (C) 2009-2016 Benjamin Gerard <http://sourceforge.net/users/benjihan>
dnl#
dnl# Distribued under the term of the GPL3+

# serial 20160902 sc68_cppflags.m4

# SC68_CPPFLAGS([CPPFLAGS-VAR],[CFLAGS-VARS ...])
# -------------
# Filter preprocessor flags out of CFLAGS into CPPFLAGS
AC_DEFUN([SC68_CPPFLAGS],
  []dnl # INDENTATION
  [
    set --m4_foreach_w(VAR,[$2],[ $[]VAR])
    while test [$]# -gt 0; do
      AS_CASE(
        ["[$]1"],
        [-D?*|-U?*|-I?*|-undef|-nostdinc|-nostdinc++|-Wp,?*|-trigraph],
        [$1="${$1-}${$1+ }[$]1"],
        [-Xpreprocessor|dnl
         -D|-U|-I|dnl
         -x|dnl
         -include|-imacros|dnl
         -isystem|-imultilib|-isysroot|dnl
         -idirafter|dnl
         -iprefix|-iwithprefix|-iwithprefixbefore],
        [$1="${$1-}${$1+ }[$]1 [$]2"; shift])
      shift
    done
  ])

dnl# ----------------------------------------------------------------------
dnl#
dnl# End Of sc68_cppflags.m4
dnl#
dnl# ----------------------------------------------------------------------
