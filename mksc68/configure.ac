dnl# -*- mode:sh; sh-basic-offset:2; indent-tabs-mode:nil -*-
dnl#
dnl# the "configure" script is made from this by running GNU "autoconf"
dnl#
dnl# Configuration script for mksc68
dnl#
dnl# by Benjamin Gerard <http://sourceforge.net/users/benjihan>
dnl#

# ,----------------------------------------------------------------------.
# | Autotools and Package                                                |
# `----------------------------------------------------------------------'

m4_define([LIBCUR],1)
m4_define([LIBREV],0)
m4_define([LIBAGE],0)
define([sc_version], esyscmd([sh vcversion.sh LIBCUR.LIBREV.LIBAGE]))dnl

AC_INIT(
  [mksc68],
  sc_version,
  [http://sourceforge.net/projects/sc68/],
  [mksc68],
  [http://sc68.atari.org])
AC_CONFIG_SRCDIR([src/mksc68.c])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign 1.11 silent-rules dist-xz subdir-objects
                  no-installman no-installinfo])
AC_CONFIG_HEADERS([config.h])
AC_USE_SYSTEM_EXTENSIONS
LT_INIT([win32-dll])

# ,----------------------------------------------------------------------.
# | sc68                                                                 |
# `----------------------------------------------------------------------'

SC68_PACKAGE(
  [

    sc68 toolbox.

    A shell program to create, modify, play, debug and more sc68/sndh
    files.

  ])
SC68_CC
AC_PROG_INSTALL

SC68_WITH_PACKAGE([libsc68],[libsc68],[sc68],[../libsc68],
                  [sc68/sc68.h],[sc68_version],[req])

SC68_WITH_SOURCE([file68],[file68],[../file68],[sc68/file68.h])
SC68_WITH_SOURCE([unice68],[unice68],[../unice68],[unice68.h])


[SAVE_CPPFLAGS="$CPPFLAGS"]
SC68_CPPFLAGS([CPPFLAGS],[CPPFLAGS libsc68_CFLAGS])
AC_CHECK_HEADERS([emu68/emu68.h])
AS_IF(
  [test ${libsc68_srcdir+set}/$ac_cv_header_emu68_emu68_h = set/no],
  [AC_MSG_CHECKING([for emu68/emu68.h presence in libsc68 srcdir])
   AS_IF([test -e "$libsc68_srcdir/emu68/emu68.h"],
         [AC_MSG_RESULT([yes])
          CPPFLAGS="${CPPFLAGS} -I$libsc68_srcdir"
          AS_UNSET([ac_cv_header_emu68_emu68_h])
          AC_CHECK_HEADERS(
            [emu68/emu68.h],
            [libsc68_CFLAGS="${libsc68_CFLAGS} -I$libsc68_srcdir"],
            [AC_MSG_RESULT([no])])
         ])
  ])
[CPPFLAGS="$SAVE_CPPFLAGS"]

AS_IF([test "X$ac_cv_header_emu68_emu68_h" != Xyes],
      [AC_MSG_ERROR([unable to locate emu68 headers])])

# ,----------------------------------------------------------------------.
# | Standard headers and functions                                       |
# `----------------------------------------------------------------------'

AC_CHECK_HEADERS([limits.h assert.h])
AC_HEADER_ASSERT
AC_CHECK_HEADERS([stdio.h stdlib.h stdarg.h stdint.h string.h])
AC_CHECK_HEADERS([unistd.h io.h fcntl.h time.h])
AC_CHECK_HEADERS([sys/socket.h sys/ioctl.h netdb.h])

# ,----------------------------------------------------------------------.
# | Extra libraries                                                      |
# `----------------------------------------------------------------------'

SC68_READLINE

# AS_UNSET([has_edit])
# AS_CASE(
#   ["${host_vendor}-${host_os}"],
#   [w64-mingw32 | apple-darwin*],
#   [SC68_WITH_MODULE([edit],[libedit],[readline],[readline.h])]
#   [AC_SEARCH_LIBS([readline],[readline],[has_edit=maybe])])
# AS_IF([test "X${has_edit-no}" != Xno],
#       [AC_CHECK_HEADERS([readline/readline.h readline/history.h])])

SC68_THREADS
AS_IF([test "X$ac_cv_header_sys_socket_h" != Xyes],
      [AC_CHECK_HEADERS([winsock2.h winsock.h],[break])
       AS_IF([test "X$ac_cv_header_winsock2_h" = Xyes],
             [LIBS="${LIBS-}${LIBS+ }-lws2_32"],
             [AS_IF([test "X$ac_cv_header_winsock_h" = Xyes],
                    [LIBS="${LIBS-}${LIBS+ }-lwsock32"])])])

AC_CHECK_FUNCS([malloc free])
AC_CHECK_FUNCS([strdup strncasecmp stpcpy])
AC_CHECK_FUNCS([vsprintf vsnprintf vasprintf])
AC_CHECK_FUNCS([hstrerror])
AC_CHECK_FUNCS([_isatty isatty _fileno fileno])

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
AC_MSG_NOTICE([|   readline            : $has_readline ($readline_VERSION)])
AC_MSG_NOTICE([+-----------------------])
