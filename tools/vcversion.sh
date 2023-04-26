#!/bin/sh
#
# by Benjamin Gerard
#
# How it works:
#  - svn revision via svnversion binary
#  - svn revision behind git-svn
#  - git revision
#  - fallbacks to current date YYYY-MM-DD
#
# TODO:
#
#   May be try to determine which source control is in used rather than
#   running the command one by one.
#

me=vcversion.sh
export LC_ALL=C
cleanexpr='s/^[[:space:]]*\([0-9][-_0-9A-Za-z]*\).*/\1/p'
base=
rev=

while test $# -gt 0; do
    case "$1" in
	-h | --help | --usage)
	    cat <<EOF
Usage: $me [PREFIX]
 
  Print a version number based on source control revision.
EOF
	    exit 0;;
	*)
	    base="$1" ;;
    esac
    shift
done

if test X"$base" != X ; then
    base="${base}."
fi

# Check and cache for a program
have_program()
{
    set -x
    if  eval test X'${'have_$1'+set}' != Xset; then
	if which $1 2>/dev/null >/dev/null; then
	    eval have_$1=yes
	else
	    eval have_$1=no
	    echo "$me: WARNING could not locate '$1' in your PATH" >&2
	fi
    fi
    eval test X'"${have_'$1'}"' = Xyes
}

# svn ? (default sc68 source control)
if test X"$rev" = X && have_program svnversion; then
    rev=`svnversion -nq | sed -ne "$cleanexpr"`
fi 2>/dev/null

# git-svn ?
if test X"$rev" = X && have_program git; then
   rev=`git svn find-rev HEAD | sed -ne "$cleanexpr"`
fi 2>/dev/null

# git ?
if test X"$rev" = X && have_program git; then
    rev=`git rev-list --count HEAD | sed -ne "$cleanexpr"`
fi 2>/dev/null

if test "X$rev" != X; then
    # Have something lets print it
    printf '%s' "${base}${rev}"
else
    # Else use date as default
    today='0'
    if have_program date; then
	today=`date -u "+%Y%m%d" 2>/dev/null` || today='0'
    fi
    printf '%s' "${base}${today}"
fi 2>/dev/null
