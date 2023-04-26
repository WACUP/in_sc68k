dnl# -*- mode:sh; sh-basic-offset:2; indent-tabs-mode:nil -*-
dnl#
dnl# autoconf macros
dnl#
dnl# (C) 2009-2016 Benjamin Gerard <http://sourceforge.net/users/benjihan>
dnl#
dnl# Distribued under the term of the GPL3+

# serial 20160825 sc68_package.m4

# SC68_PACKAGE([DESC])
# ------------
# Common package info for sc68 related packages.
AC_DEFUN_ONCE([SC68_PACKAGE],
  []dnl # INDENTATION
  [
    dnl # Declare persistant ENVVARS
    dnl # --------------------------

    AC_ARG_VAR(
      [gb_LDFLAGS],
      [Supplemental libtool LDFLAGS])

    AC_ARG_VAR(
      [gb_CFLAGS],
      [Supplemental libtool CFLAGS])

    dnl # Disable assert by default
    dnl # --enable-assert we'll trigger debug mode
    AS_IF([test X${enable_assert+set} != Xset],[enable_assert=no],
          [AS_IF([test X${enable_assert} = Xyes],
                 [set -- $CPPFLAGS $gb_CFLAGS $CFLAGS $CXXFLAGS
                  while test [$]# -ne 0; do
                    AS_CASE(["X[$]1"],
                            [X-UDEBUG | X-DDEBUG | X-DDEBUG=*],
                            [break])
                    shift
                  done
                  AS_IF([test [$]# -eq 0],
                        [gb_CFLAGS="${gb_CFLAGS}${gb_CFLAGS+ }-DDEBUG=0"])
                 ])
          ])

    SC68_CPPFLAGS([gb_CPPFLAGS],[gb_CFLAGS])


    dnl # Define PACKAGE description
    dnl # --------------------------

    m4_define(
      [AX_PACKAGE_NORMDESC],
      [m4_normalize(
          m4_quote(
            [$1]
            AC_PACKAGE_NAME is part of the sc68 project <AC_PACKAGE_URL>.))])

    m4_define([AX_PACKAGE_SHORTDESC],
              [m4_bpatsubst(AX_PACKAGE_NORMDESC,[\..*])])

    AC_SUBST([PACKAGE_SHORTDESC],['AX_PACKAGE_SHORTDESC'])
    AS_UNSET([PAC_CFLAGS])
    AS_UNSET([PAC_LIBS])
    AS_UNSET([PAC_PRIV_LIBS])
    AS_UNSET([PAC_REQUIRES])
  ])


# SC68_OUTPUT()
# -----------
# Produces finale output
AC_DEFUN_ONCE([SC68_OUTPUT],
  []dnl # INDENTATION
  [
    AS_IF([test "X${gb_LDFLAGS+set}" != Xset],
          [AS_CASE(["X$host_os"],
                   [*cygwin* | *mingw*],[gb_LDFLAGS=-no-undefined])])

    dnl # libtool interface versioning
    dnl # ----------------------------
    AC_SUBST([LIB_CUR],m4_ifset([LIBCUR],LIBCUR,0))
    AC_SUBST([LIB_REV],m4_ifset([LIBREV],LIBREV,0))
    AC_SUBST([LIB_AGE],m4_ifset([LIBAGE],LIBAGE,0))

    dnl # pkgconfig subsitution
    dnl # ---------------------
    AC_SUBST([PAC_CFLAGS])
    AC_SUBST([PAC_LIBS])
    AC_SUBST([PAC_PRIV_LIBS])
    AC_SUBST([PAC_REQUIRES])


    dnl # !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    dnl # !!! EVIL DIRTY UGLY HACK !!!
    dnl # !!!!!!!!!!!!!!!!!!!!!!!!!!!!
    dnl #
    dnl # https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=702737
    dnl #
    dnl # Because Debian patch libtool to disable link_all_deplibs
    dnl # this project won't link properly when compiling the whole
    dnl # tree as ies Makefile.am files rely on dependencies being
    dnl # inherited accross .la files.
    dnl #
    dnl # While I'm sure Debian developpers have excellent reasons to
    dnl # modify what is documented as libtool default behavior it's
    dnl # yet another querk in the autotool build system.
    dnl #
    dnl # This should force libtool to link all dependencies.
    AC_CONFIG_COMMANDS_PRE([link_all_deplibs=yes])

    AC_OUTPUT
  ])


dnl# ----------------------------------------------------------------------
dnl#
dnl# End Of sc68_package.m4
dnl#
dnl# ----------------------------------------------------------------------
