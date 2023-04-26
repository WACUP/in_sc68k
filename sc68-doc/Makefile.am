# ,----------------------------------------------------------------------.
# |                                                                      |
# | sc68-doc -- Makefile.am -- Use with automake to produce Makefile.in  |
# |                                                                      |
# |               Copyright (c) 2007-2016 Benjamin Gerard                |
# |                                                                      |
# |               <http://sourceforge.net/users/benjihan>                |
# |                                                                      |
# `----------------------------------------------------------------------'
# @configure_input@

ACLOCAL_AMFLAGS      = -I m4 --install

# ,----------------------------------------------------------------------.
# | Doxygen                                                              |
# `----------------------------------------------------------------------'

all: all-am dox-all

include Makefile-doc.in 

# CLEANFILES           =
# DISTCLEANFILES       =
# MAINTAINERCLEANFILES =

clean-local: dox-clean-local

# ,----------------------------------------------------------------------.
# | Extra distrib                                                        |
# `----------------------------------------------------------------------'

EXTRA_DIST           = sc68_doc.h
