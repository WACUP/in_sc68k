dnl# -*- mode:sh; sh-basic-offset:2; indent-tabs-mode:nil -*-
dnl#
dnl# the "configure" script is made from this by running GNU "autoconf"
dnl#
dnl# Configuration script for desa68
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
  [desa68],
  VERSION_MAJOR[.]VERSION_MINOR[.]VERSION_PATCH[.]VERSION_TWEAK,
  [http://sourceforge.net/projects/sc68/],
  [desa68],
  [http://sc68.atari.org])
AC_CONFIG_SRCDIR([desa68.c])
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

    Motoral MC68000 disassembler library.

    A tiny library to disassemble Motorola 68000 code. The library is
    specially designed to help follow the instruction flow. It
    supports absolute and symbolic disassembly.

  ])

SC68_CC
AC_PROG_INSTALL

# ,----------------------------------------------------------------------.
# | Functions and Headers
# `----------------------------------------------------------------------'

AC_CHECK_HEADERS([limits.h assert.h stdint.h])
AC_HEADER_ASSERT

# ,----------------------------------------------------------------------.
# | Output                                                               |
# `----------------------------------------------------------------------'

AC_CONFIG_FILES([desa68.pc Makefile])
SC68_OUTPUT

# ,----------------------------------------------------------------------.
# | Summary                                                              |
# `----------------------------------------------------------------------'

AC_MSG_NOTICE([])
AC_MSG_NOTICE([,-----------------------])
AC_MSG_NOTICE([| $PACKAGE_NAME ($PACKAGE_VERSION) for $host])
AC_MSG_NOTICE([+-----------------------])
AC_MSG_NOTICE([])
