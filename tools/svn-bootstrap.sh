#! /bin/sh
#
# Run this script after fresh update. It should do all autotools and
# buddies required before you can configure and compile this package.
#
# this file is part of the sc68 project <http://sc68.atari.org/>
#
# Copyright (c) 2007-2016 Benjamin Gerard
#

set -e

me="svn-bootstrap.sh"
ln_s="ln -sf --"
cp_r="cp -R --"
rm_f="rm -f --"
vifs="-vifs"
linking="Linking"

# Display informationnal message
msg() {
    echo "$@"
}

# Display error message
error() {
    echo "$me: $*" 1>&2
    return 1
}

# Display error message and exit
fatal() {
    error "$@"
    exit 127
}

while [ $# -gt 0 ]; do
    case x"$1" in
	x-h|x--usage|x--help)
	    cat <<EOF
Usage: $me [no-links]

  Setup autotools and others required steps to setup sc68 source tree.

EOF
	    exit 0
	    ;;
	xno-link* | xnl)
	    ln_s="${cp_r}"
	    vifs="-vif"
	    linking="Copying"
	    ;;
	*)
	    fatal "invalid argument -- '$1'"
	    ;;
    esac
    shift
done    

# $1:VARNAME $2:toolname
testautotool() {
    local tool res
    eval tool=\"\$$1\"
    if test X"$tool" != X; then
	msg "Checking for $2 defined by $1 as '$tool'"
	if test -x "$tool"; then
	    eval export $1
	    return 0
	else
	    error "missing preconfigured $2 -- '$tool'"
	    return 1
	fi
    fi

    if testtool "$2"; then
	return 0
    fi

    if test $1 = LIBTOOLIZE; then
	if testtool glibtoolize; then
	    export LIBTOOLIZE="`which glibtoolize`"
	    return 0
	fi
    fi
    return 1
}

# $1:tool  $2:what-if-not
testtool() {
    local tool="$1"
    msg "Checking for tool '${tool}'"
    if ! which "$tool" >/dev/null 2>/dev/null; then
	if [ -n "$2" ]; then
	    error "$2"
	else
	    error "missing '${tool}'"
	fi
	return 1
    fi
}

# $1: file  $2: action
testfile() {
    local file="$1" action="$2"
    msg "Checking for file '${file}'"
    if ! test -f "${file}"; then
	${action:-error} "Missing file '${file}'"
	return 1
    fi
    return 0
}

# $1: file  $2: action
testlink() {
    local file="$1" action="$2"
    msg "Checking for symbolic link '${file}'"
    if ! test -f "${file}" && ! test -h "${file}"; then
	${action:-error} "Missing symbolic link '${file}'"
	return 1
    fi
    return 0
}

# $1: dir  $2: action
testdir() {
    local dir="$1" action="$2"
    msg "Checking for directory '${dir}'"
    if ! (cd "${dir}"); then
	${action:-error} "Invalid directory '${dir}'"
	return 1
    fi
    return 0
}

rm_if_exists() {
    if [ -e "$1" ]; then
	msg "Removing $1"
	$rm_f "$1"
    fi
}

# $1: source  $2: destination
ln_or_cp() {
    rm_if_exists "$2"
    msg "${linking} '$1' -> '$2'"
    $ln_s "$1" "$2" ||
    $cp_r "$1" "$2"
}

# $1: source  $2: destination
ln_or_cp_x() {
    rm_if_exists "$2"
    msg "${linking} executable '$1' -> '$2'"
    $ln_s "$1" "$2" || {
	$cp_r "$1" "$2";
	chmod u+x "$2";
    }
}

# $1: dir
vcversion_dir() {
    local dir="$1"
    ( cd "${dir}" &&
	  ln_or_cp_x ../tools/vcversion.sh vcversion.sh )
}

# $1: dir  $2: --force
bootstrap_dir() {
    local dir="$1" m4file=sc68_package.m4 force='' 
    local test skip obj src lnk dst

    msg "Bootstrapping directory '${dir}'"

    test "$2" = '--force' && force='yes'

    testfile "${dir}/../m4/${m4file}" || return 1
    testfile "${dir}/configure.ac"    || return 1
    testfile "${dir}/Makefile.am"     || return 1
    testfile "${dir}/../AUTHORS"      || return 1

    vcversion_dir "${dir}"
    
    for obj in m4 AUTHORS README; do
	test=false
	dst="${dir}/${obj}"
	if test -e "${dir}/${obj}-${dir}"; then
	    src="${dir}/${obj}-${dir}"
	    lnk="${obj}-${dir}"
	elif test -f "${dir}/${obj}"; then
	    msg "keeping ${dir}/${obj}"
	    continue
	elif test -e "${obj}"; then
	    src="${obj}"
	    lnk="../${obj}"
	else
	    fatal "'${obj}' is missing in action"
	fi
	
	if test -e "${dst}"; then
	    if test -d "${src}"; then
		test=testdir
		if ! test -d "${dst}"; then
		    fatal "'${dst}' exists but is not a directory"
		fi
	    elif test -f "${src}"; then
		test=testfile
		if ! test -f "${dst}"; then
		    fatal "'${dst}' exists but is not a file"
		elif ! diff "${dst}" "${src}" >/dev/null 2>/dev/null
		then
		    if test x$force = xyes; then
			rm -- "${dst}"
		    else
			fatal "'${dst}' exists but differs (--force to overwrite)"
		    fi
		fi
	    else
		fatal "'${src}' exists but is neither a file or a directory"
	    fi
	fi

	test -e "${dst}" || ( cd "${dir}" && ln_or_cp "${lnk}" "${obj}" )
	$test "${dst}" fatal
    done
    return 0;
}

######################################################################

# Test some required tools
# ------------------------

# Added test for autotools trying to honor user defined environment
# variables
set -e
testautotool AUTOCONF autoconf
testautotool AUTOM4TE autom4te
testautotool AUTOCONF autoconf
testautotool AUTOHEADER autoheader
testautotool AUTOMAKE automake
testautotool ACLOCAL aclocal
# testautotool AUTOPOINT autopoint # Don't need autopoint for now
testautotool LIBTOOLIZE libtoolize
testautotool M4 m4
testautotool MAKE make 

testtool pkg-config

# ben (2016-08-29): not required anymore since doc is disable
#testtool help2man    "missing help2man -- install package 'help2man'"
#testtool texinfo2man "missing texinfo2man -- compile and install 'tools/texinfo2man.c'"

# ben (2016-08-29): command used to convert 68000 binary to C includes 
#                   is using hexdump instead of od
# testtool od
testtool hexdump

# Test for aclocal68 directory
testdir aclocal68 fatal

# Test for required file
testfile ./tools/vcversion.sh

# Install vcversion script in the top source dir
ln_or_cp_x ./tools/vcversion.sh vcversion.sh

# In the top source dir create a m4 directory filled with invidual
# symbolic links to aclocal68 files. All sub directories only link
# this m4 directory.
mkdir -p m4 && testdir m4 fatal
(
    cd m4 &&
    for l in ../aclocal68/*.m4; do
	ln_or_cp "$l" $(basename "$l")
    done
)

# Bootstrap all sub-directories.
dirs="as68 desa68 unice68 "
dirs="$dirs file68 info68 sourcer68 "
dirs="$dirs libsc68 sc68 mksc68"
edirs=
for dir in ${dirs}; do
    bootstrap_dir ${dir} "$@" || edirs="$edirs $dir"
done

# Install vcversion.sh script in these directories
for dir in sc68-winamp sc68-vlc sc68-audacity
do
    if [ -d ${dir} ] && [ -f ${dir}/configure.ac ]; then
	 vcversion_dir ${dir} "$@" || edirs="$edirs $dir"
    fi
done

if test "X${edirs}" != X; then
    set -- $edirs
    fatal "$# errors ($edirs): Not running autoreconf"
fi

# No error runs autoreconf to create missing files.
msg "Running autoreconf ${vifs}"
autoreconf ${vifs}
