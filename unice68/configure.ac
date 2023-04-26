dnl# -*- mode:sh; sh-basic-offset:2; indent-tabs-mode:nil -*-
dnl#
dnl# the "configure" script is made from this by running GNU "autoconf"
dnl#
dnl# Configuration script for unice68
dnl#
dnl# by Benjamin Gerard <http://sourceforge.net/users/benjihan>
dnl#

# ,----------------------------------------------------------------------.
# | Autotools and Package                                                |
# `----------------------------------------------------------------------'

dnl # This library does not use interface versioning
dnl # m4_define([LIBCUR],2)
dnl # m4_define([LIBREV],0)
dnl # m4_define([LIBAGE],0)

m4_define([VERSION_MAJOR],2)
m4_define([VERSION_MINOR],0)
m4_define([VERSION_PATCH],0)
m4_define([VERSION_TWEAK],esyscmd([sh vcversion.sh]))dnl

AC_INIT(
  [unice68],
  VERSION_MAJOR[.]VERSION_MINOR[.]VERSION_PATCH[.]VERSION_TWEAK,
  [http://sourceforge.net/projects/sc68/],
  [unice68],
  [http://sc68.atari.org])
AC_CONFIG_SRCDIR([unice68.c])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign 1.11 silent-rules dist-xz
                  no-installman no-installinfo])
AC_CONFIG_HEADERS([config.h])
AC_USE_SYSTEM_EXTENSIONS
LT_INIT([win32-dll])

# ,----------------------------------------------------------------------.
# | sc68                                                                 |
# `-------------------------------------------------------------------

SC68_PACKAGE(
  [

    Ice packer/depacker program and library.

    A small library and a command line program to pack (ice) and
    unpack (melt) Atari-ST ICE! files.

  ])

SC68_CC
AC_PROG_INSTALL

# ,----------------------------------------------------------------------.
# | Optional features                                                    |
# `----------------------------------------------------------------------'

# Command line depacker.
AC_ARG_ENABLE(
  [unice68-cli],
  [AS_HELP_STRING([--disable-unice68-cli],
      [discard the cli program; only build the library])],
  [],[enable_unice68_cli="yes"])
AM_CONDITIONAL([UNICE68_CLI],[test "X$enable_unice68_cli" = Xyes])

# ,----------------------------------------------------------------------.
# | Functions and headers                                                |
# `----------------------------------------------------------------------'

AC_CHECK_HEADERS([limits.h assert.h])
AC_HEADER_ASSERT
AC_CHECK_HEADERS([stdint.h stdio.h stdlib.h string.h stdarg.h])
AC_CHECK_HEADERS([unistd.h fcntl.h errno.h libgen.h io.h])
AC_CHECK_FUNCS([basename])
AC_CHECK_FUNCS([_fileno fileno _setmode])

# ,----------------------------------------------------------------------.
# | Output                                                               |
# `----------------------------------------------------------------------'

AC_CONFIG_FILES([unice68.pc Makefile])
SC68_OUTPUT

# ,----------------------------------------------------------------------.
# | Summary                                                              |
# `----------------------------------------------------------------------'

AC_MSG_NOTICE([])
AC_MSG_NOTICE([,-----------------------])
AC_MSG_NOTICE([| $PACKAGE_NAME ($PACKAGE_VERSION) for $host])
AC_MSG_NOTICE([|-----------------------])
AC_MSG_NOTICE([|   build cli           : $enable_unice68_cli])
AC_MSG_NOTICE([+-----------------------])
AC_MSG_NOTICE([])
