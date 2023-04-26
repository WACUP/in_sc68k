# ,----------------------------------------------------------------------.
# |                                                                      |
# |  mksc68 -- Makefile.am -- Use with automake to produce configure.in  |
# |                                                                      |
# |               Copyright (c) 2009-2016 Benjamin Gerard                |
# |                                                                      |
# |               <http://sourceforge.net/users/benjihan>                |
# |                                                                      |
# `----------------------------------------------------------------------'
# @configure_input@

ACLOCAL_AMFLAGS = -I m4 --install

# ,----------------------------------------------------------------------.
# | Libtool program                                                      |
# `----------------------------------------------------------------------'

bin_PROGRAMS = mksc68

mksc68_SOURCES = src/mksc68.c src/mksc68_cli.c src/mksc68_cli.h		\
 src/mksc68_cmd.c src/mksc68_cmd.h src/mksc68_cmd_extract.c		\
 src/mksc68_cmd_gdb.c src/mksc68_cmd_info.c src/mksc68_cmd_load.c	\
 src/mksc68_cmd_msg.c src/mksc68_cmd_new.c src/mksc68_cmd_play.c	\
 src/mksc68_cmd_save.c src/mksc68_cmd_tag.c src/mksc68_cmd_time.c	\
 src/mksc68_def.h src/mksc68_dsk.c src/mksc68_dsk.h src/mksc68_eva.h	\
 src/mksc68_gdb.c src/mksc68_gdb.h src/mksc68_msg.c src/mksc68_msg.h	\
 src/mksc68_oplen.h src/mksc68_opt.c src/mksc68_opt.h			\
 src/mksc68_snd.c src/mksc68_snd.h src/mksc68_str.c src/mksc68_str.h	\
 src/mksc68_tag.c src/mksc68_tag.h

mksc68_CFLAGS   = $(libsc68_CFLAGS) $(gb_CFLAGS)
mksc68_CPPFLAGS = $(unice68_CFLAGS) $(file68_CFLAGS) $(edit_CFLAGS)
mksc68_LDFLAGS  = $(gb_LDFLAGS)
mksc68_LDADD    = $(LIBSC68_LA) $(libsc68_LIBS) $(edit_LIBS)

if SOURCE_LIBSC68

LIBSC68_LA = $(libsc68_builddir)/libsc68.la
BUILT_SOURCES = $(LIBSC68_LA)

@SET_MAKE@
$(LIBSC68_LA):
	cd "$(libsc68_builddir)" && $(MAKE) $(AM_MAKEFLAGS)

endif
