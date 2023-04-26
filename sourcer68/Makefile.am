# ,----------------------------------------------------------------------.
# |                                                                      |
# | sourcer68 -- Makefile.am -- Use with automake to produce configure.in|
# |                                                                      |
# |               Copyright (c) 2006-2016 Benjamin Gerard                |
# |                                                                      |
# |               <http://sourceforge.net/users/benjihan>                |
# |                                                                      |
# `----------------------------------------------------------------------'
# @configure_input@

ACLOCAL_AMFLAGS = -I m4 --install

# ,----------------------------------------------------------------------.
# | Libtool program                                                      |
# `----------------------------------------------------------------------'

bin_PROGRAMS = sourcer68
sourcer68_SOURCES = src/sourcer68.c src/src68_adr.c src/src68_adr.h     \
 src/src68_def.h src/src68_dis.c src/src68_dis.h src/src68_eva.c        \
 src/src68_eva.h src/src68_exe.c src/src68_exe.h src/src68_fmt.c        \
 src/src68_fmt.h src/src68_mbk.c src/src68_mbk.h src/src68_msg.c        \
 src/src68_msg.h src/src68_opt.c src/src68_opt.h src/src68_rel.c        \
 src/src68_rel.h src/src68_sec.c src/src68_sec.h src/src68_src.c        \
 src/src68_src.h src/src68_sym.c src/src68_sym.h src/src68_tos.c        \
 src/src68_tos.h src/src68_vec.c src/src68_vec.h
sourcer68_CFLAGS  = $(desa68_CFLAGS) $(file68_CFLAGS) $(gb_CFLAGS)
sourcer68_LDFLAGS = $(gb_LDFLAGS)
sourcer68_LDADD   = $(FILE68_LA) $(file68_LIBS) $(DESA68_LA) $(desa68_LIBS)

if SOURCE_DESA68
DESA68_LA = $(desa68_builddir)/libdesa68.la
@SET_MAKE@
$(DESA68_LA):
	cd "$(desa68_builddir)" && $(MAKE) $(AM_MAKEFLAGS)
else
DESA68_LA=
endif

if SOURCE_FILE68
FILE68_LA = $(file68_builddir)/libfile68.la
@SET_MAKE@
$(FILE68_LA):
	 cd "$(file68_builddir)" && $(MAKE) $(AM_MAKEFLAGS)
else
FILE68_LA =
endif

# Force early build
BUILT_SOURCES = $(DESA68_LA) $(FILE68_LA)
