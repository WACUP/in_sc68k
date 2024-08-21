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
   has_readline=maybe; RL_SAVE_LIBS="$LIBS"; RL_SAVE_CPPFLAGS="$CPPFLAGS"
   AS_IF([test X$with_readline = Xno],
         [has_readline=no; org_readline=user],
         AS_IF([test X$with_readline = Xlibedit],
               [has_readline=maybe; mod_readline=libedit],
               [mod_readline=readline])
         AS_IF(
           [test X${readline_LIBS+set} = Xset],
           [LIBS="$LIBS $readline_LIBS"
             org_readline=system
             AC_CHECK_FUNC([readline],[has_readline=user],[has_readline=no])])
         
         AS_IF(
           [test $has_readline = maybe],
           [SC68_PKG_CONFIG([readline],[$]mod_readline,[VERSION],[--modversion])
            SC68_PKG_CONFIG([readline],[$]mod_readline,[LIBS],[--libs])
            LIBS="$LIBS $readline_LIBS"
            AC_CHECK_FUNC(
              [readline],
              [has_readline=[$]mod_readline; org_readline=pkgconfig])])
         
         AS_IF(
           [test $has_readline = maybe],
           [LIBS="$LIBS -lreadline"
            AC_CHECK_FUNC(
              [readline],
              [readline_LIBS=-lreadline
               has_readline=system; org_readline=system],
              [has_readline=no])]))
   
   AS_CASE(
     [$has_readline],
     [no|maybe],[has_readline=no],
     [AS_IF([test X${readline_CFLAGS+set} = X && test X[$]org_readline = Xpkgconfig],
            [SC68_PKG_CONFIG([readline],[$]mod_readline,[CFLAGS],[--cflags])])
      SC68_CPPFLAGS([CPPFLAGS],[readline_CFLAGS])
      has_readline=no
      AC_CHECK_HEADERS([readline/readline.h readline.h],
                       [has_readline=yes])])
   AS_IF(
     [test $has_readline != yes],
     [has_readline=no; LIBS="$RL_SAVE_LIBS"; CPPFLAGS="$RL_SAVE_CPPFLAGS"])
   _SC68_WITH_CLOSE([readline],[readline])
])

dnl# ----------------------------------------------------------------------
dnl#
dnl# End of sc68_readline.m4
dnl#
dnl# ----------------------------------------------------------------------
