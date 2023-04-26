dnl# -*- mode:sh; sh-basic-offset:2; indent-tabs-mode:nil -*-
dnl#
dnl# autoconf macros
dnl#
dnl# (C) 2009-2016 Benjamin Gerard <http://sourceforge.net/users/benjihan>
dnl#
dnl# Distribued under the term of the GPL3+

# serial 20161105 sc68_readline.m4

# SC68_READLINE()
# -------------
# Find readline flavor
AC_DEFUN([SC68_READLINE],
  []dnl # INDENTATION
  [_SC68_WITH_INIT([readline],[readline])
   has_readline=maybe
   AS_IF(
     [test X$with_readline = Xno],
     [has_readline=no; org_readline=user],
     [has_readline=maybe
      RL_SAVE_LIBS="$LIBS"
      RL_SAVE_CPPFLAGS="$CPPFLAGS"
      AS_IF(
        [test X${readline_LIBS+set} = Xset],
        [LIBS="$LIBS $readline_LIBS"
         AC_CHECK_FUNC(
           [readline],
           [has_readline=user
            org_readline=system])],
        [AS_CASE(
           [$host_os],
           [mingw*],[],
           [LIBS="$LIBS -lreadline"
            AC_CHECK_FUNC(
              [readline],
              [readline_LIBS=-lreadline
               has_readline=system
               org_readline=system])])
         AS_IF(
           [test $has_readline = maybe],
           [SC68_PKG_CONFIG([readline],[libedit],[VERSION],[--modversion])
            SC68_PKG_CONFIG([readline],[libedit],[LIBS],[--libs])
            LIBS="$LIBS $readline_LIBS"
            AC_CHECK_FUNC(
              [readline],
              [has_readline=edit
               org_readline=pkgconfig])])
        ])

      AS_CASE(
        [$has_readline],
        [no|maybe],[has_readline=no],
        [edit],
        [AS_IF(
           [test X${readline_CFLAGS+set} = X],
           [SC68_PKG_CONFIG([readline],[libedit],[CFLAGS],[--cflags])])
         SC68_CPPFLAGS([CPPFLAGS],[readline_CFLAGS])
         AC_CHECK_HEADERS([readline.h],
                          [has_readline=yes])],
        [SC68_CPPFLAGS([CPPFLAGS],[readline_CFLAGS])
         AC_CHECK_HEADERS([readline/readline.h readline.h],
                          [has_readline=yes])])
      AS_IF(
        [test $has_readline != yes],
        [has_readline=no
         LIBS="$RL_SAVE_LIBS"
         CPPFLAGS="$RL_SAVE_CPPFLAGS"])
     ])
   _SC68_WITH_CLOSE([readline],[readline])
  ])

dnl# ----------------------------------------------------------------------
dnl#
dnl# End of sc68_readline.m4
dnl#
dnl# ----------------------------------------------------------------------
