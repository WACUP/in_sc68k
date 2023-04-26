dnl# -*- mode:sh; sh-basic-offset:2; indent-tabs-mode:nil -*-
dnl#
dnl# the "configure" script is made from this by running GNU "autoconf"
dnl#
dnl# Configuration script for info68
dnl#
dnl# by Benjamin Gerard <http://sourceforge.net/users/benjihan>
dnl#

# ,----------------------------------------------------------------------.
# | Autotools and Package                                                |
# `----------------------------------------------------------------------'

m4_define([VERSION_MAJOR],2)
m4_define([VERSION_MINOR],0)
m4_define([VERSION_PATCH],0)
m4_define([VERSION_TWEAK],esyscmd([sh vcversion.sh]))dnl

AC_INIT(
  [info68],
  VERSION_MAJOR[.]VERSION_MINOR[.]VERSION_PATCH[.]VERSION_TWEAK,
  [http://sourceforge.net/projects/sc68/],
  [info68],
  [http://sc68.atari.org])
AC_CONFIG_SRCDIR([info68.c])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign 1.11 silent-rules dist-xz
                  no-installman no-installinfo])
AC_CONFIG_HEADERS([config.h])
AC_USE_SYSTEM_EXTENSIONS
LT_INIT([win32-dll])

# ,----------------------------------------------------------------------.
# | sc68                                                                 |
# `----------------------------------------------------------------------'

SC68_PACKAGE(
  [

    format sc68 files meta-tags.

    info68 programs allow to retrieve and display meta-tags information
    from sc68 compatible files and to print them on a formatted
    fashion.

  ])

SC68_CC
AC_PROG_INSTALL

SC68_WITH_PACKAGE([file68],[file68],[file68],[../file68],
                  [sc68/file68.h],[file68_version],[req])

# ,----------------------------------------------------------------------.
# | Headers and Functions                                                |
# `----------------------------------------------------------------------'

AC_CHECK_HEADERS([limits.h assert.h])
AC_HEADER_ASSERT
AC_CHECK_HEADERS([stdarg.h stdio.h stdlib.h string.h])
AC_CHECK_HEADERS([ctype.h errno.h libgen.h getopt.h])

AC_CHECK_FUNCS([basename getopt getopt_long])

# ,----------------------------------------------------------------------.
# | Output                                                               |
# `----------------------------------------------------------------------'

AC_CONFIG_FILES([Makefile])
SC68_OUTPUT

# ,----------------------------------------------------------------------.
# | Summary                                                              |
# `----------------------------------------------------------------------'

AC_MSG_NOTICE([])
AC_MSG_NOTICE([,----------------------])
AC_MSG_NOTICE([| $PACKAGE_NAME ($PACKAGE_VERSION) for $host])
AC_MSG_NOTICE([|----------------------])
AC_MSG_NOTICE([|   file68              : $has_file68 ($file68_VERSION)])
AC_MSG_NOTICE([+----------------------])
AC_MSG_NOTICE([])
