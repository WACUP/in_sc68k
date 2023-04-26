dnl# -*- mode:sh; sh-basic-offset:2; indent-tabs-mode:nil -*-
dnl#
dnl# autoconf macros
dnl#
dnl# (C) 2009-2016 Benjamin Gerard <http://sourceforge.net/users/benjihan>
dnl#
dnl# Distributed under the term of the GPL
dnl#

# serial 20160824 sc68_with_module.m4

# SC68_PKG_CONFIG(prefix,mod,var,pkg-options)
# ---------------
# $1: prefix (package prefix)
# $2: module-name (pkgconfig module name)
# $3: VARIABLE suffix to receive result (var is $1_$3)
# $4: pkgconfig options (--cflags ...)
#
AC_DEFUN([SC68_PKG_CONFIG],
  []dnl # INDENTATION
  [AC_REQUIRE([PKG_PROG_PKG_CONFIG])
   AS_UNSET($1_PKG_ERRORS)
   AC_MSG_CHECKING([for pkg-config $2 $4])
   $1_$3=`$PKG_CONFIG $4 "$2" 2>/dev/null`
   AS_IF(
     [test "X$?" = "X0"],
     [$1_PKG_EXISTS=yes; AC_MSG_RESULT([yes])],
     [AS_UNSET($1_$3)
      $1_PKG_ERRORS=`$PKG_CONFIG --short-errors $4 "$2" 2>&1`
      AC_MSG_RESULT([no ($[]$1_PKG_ERRORS)])
      AS_ECHO("$$1[]_PKG_ERRORS") >&AS_MESSAGE_LOG_FD])
  ])


# SC68_CHECK_HEADERS(prefix,header-files,[if-found],[if-not-found],[includes])
# ------------------
# $1: prefix
# $2: headers-files
# $3: action-if-found
# $4: action-if-not-found
# $5: includes
#
AC_DEFUN([SC68_CHECK_HEADERS],
  []dnl # INDENTATION
  [save_$1_CPPFLAGS="$CPPFLAGS"
   SC68_CPPFLAGS([CPPFLAGS],[CPPFLAGS CFLAGS $1_CFLAGS])
   AC_CHECK_HEADERS([$2],[$3],[$4],[$5])
   CPPFLAGS="$save_$1_CPPFLAGS"
   AS_UNSET(save_$1_CPPFLAGS)])


# SC68_CHECK_FUNCS(prefix,funcs,[if-found],[if-not-found])
# ----------------
# $1: prefix
# $2: functions
# $3: action-if-found
# $4: action-if-not-found
#
AC_DEFUN([SC68_CHECK_FUNCS],
  []dnl # INDENTATION
  [save_$1_LIBS="$LIBS"
   save_$1_CFLAGS="$CFLAGS"
   LIBS="$LIBS [$]$1_LIBS"
   CFLAGS="$CFLAGS [$]$1_CFLAGS $gb_CFLAGS"
   AC_CHECK_FUNCS([$2],[$3],[$4])
   LIBS="$save_$1_LIBS"
   CFLAGS="$save_$1_CFLAGS"
   AS_UNSET(save_$1_LIBS)
   AS_UNSET(save_$1_CFLAGS)])


# _SC68_WITH_DUMP(prefix,[text])
# ---------------
# $1: prefix
# $2: text
m4_define([_SC68_WITH_DUMP],
  []dnl # INDENTATION
  [
    cat<<__EOF

===============
 $1[]m4_ifnblank([$2],[ (m4_normalize([$2]))])
---------------
 has_$1        : ${has_$1-<unset>}
 org_$1        : ${org_$1-<unset>}
--
 with_$1       : ${with_$1-<unset>}
 with_$1_src   : ${with_$1_srcdir-<unset>}
...............
 $1_VERSION    : ${$1_VERSION-<unset>}
 $1_CFLAGS     : ${$1_CFLAGS-<unset>}
 $1_LIBS       : ${$1_LIBS-<unset>}
 $1_PKG_EXISTS : ${$1_PKG_EXISTS-<unset>}
 $1_PKG_ERRORS : ${$1_PKG_ERRORS-<unset>}
 $1_builddir   : ${$1_builddir-<unset>}
 $1_srcdir     : ${$1_srcdir-<unset>}
 $1_prefix     : ${$1_prefix-<unset>}
 $1_REQUIRED   : ${$1_REQUIRED-<unset>}
 $1_lname      : ${$1_lname-<unset>}
...............
 PAC_REQUIRES=${PAC_REQUIRES-<unset>}
 PAC_LIBS=${PAC_LIBS-<unset>}
 PAC_PRIV_LIBS=${PRIV_LIBS-<unset>}
--------
 srcdir : ${srcdir}
 PKG_CONFIG: ${PKG_CONFIG-<unset>}
--------
 LIBS   : $LIBS
 CFLAGS : $CFLAGS
--------

__EOF
  ])

# _SC68_WITH_INIT(prefix,module-or-package-name)
# ---------------
# $1: prefix
# $2: module or package name
#
m4_define([_SC68_WITH_INIT],
  []dnl # INDENTATION
  [
    dnl # --without-<prefix>
    dnl # __________________

    AC_ARG_WITH(
      [$1],
      [AS_HELP_STRING(
          [--without-$1],
          [Disable $2 support @<:@default is to check@:>@])],
      [with_$1="$withval"],[with_$1=check])

    dnl # Declare persistent ENVVARs
    dnl # __________________________

    AC_ARG_VAR(
      $1[_VERSION],
      [$1 version; overriding pkg-config.])
    AC_ARG_VAR(
      $1[_CFLAGS],
      [$1 CFLAGS; overriding pkg-config.])
    AC_ARG_VAR(
      $1[_LIBS],
      [$1 linker flags; overriding pkg-config.])

    dnl # Init interface variables
    dnl # ________________________

    AS_UNSET([$1_PKG_ERRORS])
    AS_UNSET([$1_PKG_EXISTS])
    AS_UNSET([$1_REQUIRED])
    AS_UNSET([has_$1])
    AS_UNSET([org_$1])
    AS_UNSET([$1_builddir])
    AS_UNSET([$1_srcdir])
    AS_UNSET([$1_prefix])
  ])

# _SC68_WITH_MODULE(prefix,mod,[headers],[funcs])
# -----------------
m4_define([_SC68_WITH_MODULE],
  []dnl # INDENTATION
  [
    dnl # handles --with-$1 option
    AS_CASE(
      ["X[$]with_$1"],
      [Xno],[has_$1=no],
      [Xyes],[has_$1=yes; org_$1=user],
      [Xcheck],
      [org_$1=pkgconfig
       AS_IF(
         [test "A${$1_PKG_ERRORS+B}X${$1_VERSION+Y}" = AX],
         [SC68_PKG_CONFIG($1,$2,[VERSION],[--modversion])])
       AS_IF(
         [test "A${$1_PKG_ERRORS+B}X${$1_CFLAGS+Y}" = AX],
         [SC68_PKG_CONFIG($1,$2,[CFLAGS],[--cflags])])
       AS_IF(
         [test "A${$1_PKG_ERRORS+B}X${$1_LIBS+Y}" = AX],
         [SC68_PKG_CONFIG($1,$2,[LIBS],[--libs])])
       AS_IF(
         [test "A${$1_PKG_ERRORS+B}X${$1_prefix+Y}" = AX],
         [SC68_PKG_CONFIG($1,$2,[prefix],[--variable=prefix])
          AS_UNSET([$1_PKG_ERRORS])])
       AS_IF([test "A${$1_PKG_ERRORS+B}" = AB],[has_$1=no],[has_$1=check])
      ],
      [_SC68_WITH_DUMP([$1],[internal error])
       AC_MSG_ERROR([Invalid argument --with-$1='$with_$1'])])
    AS_IF(
      [test "$has_$1/${$1_VERSION-unset}" = no/unset],
      [has_$1=check
       org_$1=system
       AS_IF(
         [test X${$1_LIBS+Y}${$1_lname+Z} = X],
         [$1_lname=`echo $2 |sed 's/^lib\(.\{1,\}\)\|\(.\{1,\}\)lib$/\1\2/'`])
      ],[AS_UNSET([$1_lname])])
    AS_IF(
      [test "X$has_$1" = Xcheck],
      [m4_ifnblank([$3],[SC68_CHECK_HEADERS([$1],[$3],,[has_$1=no])])])
    AS_IF(
      [test X${$1_LIBS+Y}${$1_lname+Z} = XZ],
      [$1_LIBS=-l${$1_lname}],
      [AS_UNSET([$1_lname])])
    AS_IF(
      [test "X$has_$1" = Xcheck],
      [m4_ifnblank([$4],[SC68_CHECK_FUNCS([$1],[$4],,[has_$1=no])])])
    AS_IF(
      [test "X$has_$1" = Xcheck],
      [has_$1=yes],
      [AS_IF([test X${$1_lname+Z} = XZ],[AS_UNSET([$1_LIBS])])])
  ])

# _SC68_WITH_CLOSE(prefix,pkg-or-mod,[required])
# ----------------
# $1: prefix
# $2: package or module name
# $3: required (if non-blank)
#
m4_define([_SC68_WITH_CLOSE],
  []dnl # INDENTATION
  [
    AS_CASE(
      ["X$has_$1"],
      [Xno|Xyes],[],
      [AC_MSG_WARN([INTERNAL -- invalid value for has_$1 ($has_$1)])])
    AS_IF(
      [test "X$has_$1" != Xyes],
      [has_$1=no
       AS_UNSET([$1_VERSION])
       AS_UNSET([$1_builddir])
       AS_UNSET([org_$1])])
    AS_IF(
      [test "X${$1_VERSION+set}" != Xset],[$1_VERSION=n/a])

    dnl # Things to do if the package has been configured
    AS_IF(
      [test "X$has_$1" = Xyes],
      [
        AS_CASE(
          ["X$org_$1"],
          [Xsource|Xpkgconfig],[$1_REQUIRED="$org_$1"],
          [Xuser|Xsystem],[],
          [_SC68_WITH_DUMP([$1],[internal error])
           AC_MSG_ERROR([Unexpected $1 value -- '$org_$1'])])

        AS_IF(
          [test "X${$1_REQUIRED+set}" = Xset],
          [PAC_REQUIRES="${PAC_REQUIRES}${PAC_REQUIRES+ }$2"])
        AC_DEFINE_UNQUOTED(AS_TR_CPP([AC_PACKAGE_NAME[]_$1]),
                           ["$[]$1_VERSION"],
                           [Defined if $1 is supported])
      ])

    dnl # Check if required by package
    m4_ifnblank(
      [$3],
      [AS_IF([test "X$has_$1" != Xyes],
             [_SC68_WITH_DUMP([$1],[error required])
              AC_MSG_ERROR([missing required module -- $2])])
      ])

    dnl # Check if requested by user
    AS_IF(
      [test "$with_$1/$has_$1" = yes/no],
      [_SC68_WITH_DUMP([$1],[error requested])
       AC_MSG_ERROR([unable to configure requested module -- $2])
      ])

    AC_SUBST([$1_builddir])
    AC_SUBST([$1_srcdir])
    _SC68_WITH_DUMP([$1],[finally])
  ])

# _SC68_WITH_BUILD(prefix,pkg,to-source,headers)
# ----------------
# $1: prefix
# $2: package-name
# $3: headers (if non-blank) (not implemented yet)
#
m4_define([_SC68_WITH_BUILD],
  []dnl # INDENTATION
  [
    AS_IF(
      [test "X${$1_builddir+set}" = Xset],
      [has_$1=no; org_$1=source
       AC_MSG_CHECKING([whether $1 builddir ([$]$1_builddir) exists])
       AS_IF(
         [test -d "[$]$1_builddir"],
         [AC_MSG_RESULT([yes])
          AC_MSG_CHECKING([whether $1 builddir seems configured])
          AS_IF(
            [grep '[PACKAGE_STRING]' \
                  "[$]$1_builddir/config.h" >/dev/null 2>/dev/null],
            [has_$1=maybe; AC_MSG_RESULT([yes])],
            [AC_MSG_RESULT([no])])],
         [AC_MSG_RESULT([no])])])

    AS_IF(
      [test "$has_$1/${$1_builddir+set}" = maybe/set],
      [has_$1=no; org_$1=source
       AC_MSG_CHECKING([whether $1 builddir is configured])
       set -- `sed -ne 's/^#define PACKAGE_STRING "$2 \(@<:@^"@:>@\{1,\}\)".*/\1/p' "${$1_builddir}/config.h"`
       AS_IF(
         [test "X[$]#" != X1 || test "X[$1]" = X],
         [AC_MSG_RESULT([no (PACKAGE_STRING not found)])
          AS_UNSET([$1_builddir])],
         [AS_IF(
            [test "X${$1_VERSION+Y}" = XY && test "X[$]$1_VERSION" != "X[$]1"],
            [AC_MSG_RESULT([no (version mismatch)])],
            [$1_VERSION="[$]1"; has_$1=yes
             AC_MSG_RESULT([yes (${$1_VERSION})])])])],
      [AS_UNSET([$1_builddir])])

  ])

m4_define([_SC68_WITH_SOURCE],
  []dnl # INDENTATION
  [
    AC_ARG_WITH(
      [$1-srcdir],
      [AS_HELP_STRING(
          [--with-$1-srcdir],
          [Locate or disable $2 source @<:@default is to check@:>@])],
      [with_$1_srcdir="$withval"],[with_$1_srcdir='yes'])

    AS_CASE(
      ["X$with_$1_srcdir"],
      [Xno],[AS_UNSET([$1_builddir]); has_$1=no],
      [X|Xyes],[$1_srcdir="$srcdir/$3"],
      [$1_srcdir="$with_$1_srcdir"])

    AS_IF(
      [test X${$1_srcdir+set} = Xset],
      [AC_MSG_CHECKING([whether $1 srcdir ([$]$1_srcdir) exists])
       AS_IF([test -d "${$1_srcdir}"],
             [AC_MSG_RESULT([yes])
              $1_srcdir=`cd "${$1_srcdir}"; pwd`],
             [AC_MSG_RESULT([no])
              AS_UNSET([$1_srcdir])])])

    m4_foreach_w(
      [xHdr],[$4],
      [AS_IF(
          [test X${$1_srcdir+set} = Xset],
          [AC_MSG_CHECKING([for xHdr presence in $1 srcdir])
           AS_IF([test -r "${$1_srcdir}/xHdr"],
                 [AC_MSG_RESULT([yes])],
                 [AC_MSG_RESULT([no])
                  AS_UNSET([$1_srcdir])])])])
  ])

# SC68_WITH_SOURCE(prefix,pkg,to-source,headers)
# ----------------
# $1: prefix
# $2: package-name
# $3: relative path to package source
# $4: headers (if non-blank)
#
AC_DEFUN([SC68_WITH_SOURCE],
  []dnl # INDENTATION
  [
    AS_UNSET([$1_srcdir])
    _SC68_WITH_SOURCE($@)

    dnl # only package that generate header files in their build
    dnl # directory needs to locate that build directory. Currently
    dnl # only file68
    m4_if(
      [$2],[file68],
      [$1_builddir="./m4_default([$3],[../$2])"
       AC_ARG_WITH(
         [$1],
         [AS_HELP_STRING(
            [--with-$1],
            [path to $2 build @<:@]m4_default([$3],[../$2])[@:>@])],
         [with_$1="$withval"],[with_$1="${$1_builddir}"])
       AS_CASE(
         ["X$with_$1"],
         [Xno],[has_$1=no; org_$1=user; AS_UNSET([$1_builddir])],
         [X|Xyes],[has_$1=yes; org_$1=user],
         [$1_builddir="$with_$1"])
       AS_IF([test X${$1_builddir+set} = Xset],
             [save_$1_CPPFLAGS="$CPPFLAGS"
              CPPFLAGS="$CPPFLAGS -I${$1_builddir}/sc68"
              AC_CHECK_HEADERS([file68_features.h],,
                               [AS_UNSET([$1_builddir])])
              CPPFLAGS="$save_$1_CPPFLAGS"])
      ])
    AS_IF(
      [test X${$1_srcdir+set} = Xset],
      [$1_CFLAGS="-I${$1_srcdir}${$1_CFLAGS+:}${$1_CFLAGS}"])
    AS_IF(
      [test X${$1_builddir+set} = Xset],
      [$1_CFLAGS="-I${$1_builddir}${$1_CFLAGS:+ }${$1_CFLAGS}"])
    AC_SUBST([$1_srcdir])
    AC_SUBST([$1_builddir])
    AC_SUBST([$1_CFLAGS])
  ])


# SC68_WITH_MODULE(prefix,mod,[headers],[funcs],[req])
# ----------------
# $1: prefix
# $2: module-name
# $3: headers (if non-blank)
# $4: functions (if non-blank)
# $5: required (if non-blank)
#
AC_DEFUN([SC68_WITH_MODULE],
  []dnl # INDENTATION
  [
    AC_REQUIRE([PKG_PROG_PKG_CONFIG])
    _SC68_WITH_INIT([$1],[$2])
    _SC68_WITH_MODULE($@)
    _SC68_WITH_CLOSE([$1],[$2],[$5])
  ])

# SC68_WITH_PACKAGE(prefix,pkg,mod,to-source,[headers],[funcs],[req])
# -----------------
# $1: prefix
# $2: package-name
# $3: module-name
# $4: relative path to package source
# $5: headers (if non-blank)
# $6: functions (if non-blank)
# $7: required (if non-blank)
#
AC_DEFUN([SC68_WITH_PACKAGE],
  []dnl # INDENTATION
  [
    _SC68_WITH_INIT([$1],[$2])

    AS_CASE(
      ["X$with_$1"],
      [Xno],[has_$1=no; org_$1=user],
      [X|Xyes],[has_$1=yes; org_$1=user],
      [Xcheck],[$1_builddir="./$4"],
      [$1_builddir="$with_$1"])

    _SC68_WITH_SOURCE([$1],[$2],[$4],[$5])
    _SC68_WITH_BUILD([$1],[$2],[$5])

    AS_IF(
      [test "$has_$1/${$1_srcdir+set}" = yes/set],
      [AC_MSG_CHECKING([whether $1 source is at '$4'])
       AS_IF(
         [test X"${$1_srcdir}" = X`cd $srcdir/$4 2>/dev/null && pwd`],
         [AC_MSG_RESULT([yes])
          $1_CFLAGS="-I\$(top_srcdir)/$4${$1_CFLAGS+:}${$1_CFLAGS}"],
         [AC_MSG_RESULT([no])
          $1_CFLAGS="-I${$1_srcdir}${$1_CFLAGS+:}${$1_CFLAGS}"])])

    AS_IF(
      [test "$has_$1/$with_$1" = no/check],
      [_SC68_WITH_MODULE([$1],[$3],[$5],[$6])])

    _SC68_WITH_CLOSE([$1],[$2],[$7])

    AM_CONDITIONAL(AS_TR_CPP([SOURCE_$2]),
                   [test "$has_$1/${$1_builddir+set}" = yes/set])
  ])

dnl# ----------------------------------------------------------------------
dnl#
dnl# End Of sc68_with_module.m4
dnl#
dnl# ----------------------------------------------------------------------
