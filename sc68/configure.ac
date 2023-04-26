dnl# -*- mode:sh; sh-basic-offset:2; indent-tabs-mode:nil -*-
dnl#
dnl# the "configure" script is made from this by running GNU "autoconf"
dnl#
dnl# Configuration script for sc68 (cli)
dnl#
dnl# by Benjamin Gerard <http://sourceforge.net/users/benjihan>
dnl#

# ,----------------------------------------------------------------------.
# | Autotools and Package                                                |
# `----------------------------------------------------------------------'

m4_define([VERSION_MAJOR],3)
m4_define([VERSION_MINOR],0)
m4_define([VERSION_PATCH],0)
m4_define([VERSION_TWEAK],esyscmd([sh vcversion.sh]))dnl

AC_INIT(
  [sc68],
  VERSION_MAJOR[.]VERSION_MINOR[.]VERSION_PATCH[.]VERSION_TWEAK,
  [http://sourceforge.net/projects/sc68/],
  [sc68-cli],
  [http://sc68.atari.org])
AC_CONFIG_SRCDIR([sc68.c])
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

    Atari ST and Amiga command line player.

    The command line program to play sc68 compatible music files or record
    them into audio files.

  ])

SC68_CC
AC_PROG_INSTALL

SC68_WITH_PACKAGE([libsc68],[libsc68],[sc68],[../libsc68],
                  [sc68/sc68.h],[sc68_version],[req])

SC68_WITH_SOURCE([file68],[file68],[../file68],[sc68/file68_vfs.h])

# ,----------------------------------------------------------------------.
# | Some headers, types and functions                                    |
# `----------------------------------------------------------------------'

# ,----------------------------------------------------------------------.
# | Functions and Headers                                                |
# `----------------------------------------------------------------------'

AC_CHECK_HEADERS([limits.h assert.h])
AC_HEADER_ASSERT
AC_CHECK_HEADERS([stdarg.h stdint.h stdio.h stdlib.h string.h])
AC_CHECK_HEADERS([ctype.h errno.h unistd.h getopt.h])

AC_CHECK_FUNCS([getopt getopt_long])

AC_CHECK_TYPES(
  [struct option],[],[],[
AC_INCLUDES_DEFAULT
[#]ifdef HAVE_GETOPT_H
[#] include <getopt.h>
[#]endif
])

# ,----------------------------------------------------------------------.
# | Output                                                               |
# `----------------------------------------------------------------------'

AC_CONFIG_FILES([Makefile])
SC68_OUTPUT

# ,----------------------------------------------------------------------.
# | Summary                                                              |
# `----------------------------------------------------------------------'

AC_MSG_NOTICE([])
AC_MSG_NOTICE([,-----------------------])
AC_MSG_NOTICE([| $PACKAGE_NAME ($PACKAGE_VERSION) for $host])
AC_MSG_NOTICE([|-----------------------])
AC_MSG_NOTICE([|   libsc68             : $has_libsc68 ($libsc68_VERSION)])
AC_MSG_NOTICE([+-----------------------])
