#!/bin/bash
#
# Fix/Update copyright my notices
#
# Copyright (c) 2016 Benjamin Gerard
#

set -e
year=$(date +%Y)
echo "Doing copyright session [$$.$year]"

find . -type f \
     -not -path '*/.*/*' \
     -not -path '*/autom4te.cache/*' \
     -not -name aclocal.m4 \
     \( -name '*.[ch]' \
     -o -name '*.sh' \
     -o -name '*.m4' \
     -o -name '*.in' \
     -o -name '*.1' \
     -o -name '*.info' \
     -o -name 'COPYING' \
     -o -name 'NEWS' \
     -o -name 'README*' \
     -o -name 'Makefile.am' \
     -o -name 'configure.ac' \) |
    while read file; do
	# echo "Testing -- [$file]"
	grep -qim1 'copyright (c) \([0-9]\{4\}-\)\?[0-9]\{4\} ben' "$file" ||
	    continue
	echo "Parsing -- $file"
 	sed -i~$$.$year -e '
s/copyright \+(c) \+\([0-9]\{4\}-\)[0-9]\{4\} \+\(ben.* \+gerard\)/Copyright (c) \1'"$year Benjamin Gerard/i" "$file" # | grep -wi copyright

    done

