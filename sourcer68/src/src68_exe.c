/*
 * @file    src68_exe.c
 * @brief   load executable.
 * @author  http://sourceforge.net/users/benjihan
 *
 * Copyright (c) 1998-2016 Benjamin Gerard
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.
 *
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "src68_exe.h"
#include "src68_msg.h"
#include "src68_tos.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef SOURCER68_FILE68

# include <sc68/file68.h>
# include <sc68/file68_err.h>
# include <sc68/file68_uri.h>
# include <sc68/file68_msg.h>
# include <sc68/file68_opt.h>
# include <sc68/file68_zip.h>
# include <sc68/file68_str.h>

static void * myopen(char * uri)
{
  vfs68_t *is =
    uri68_vfs( (uri[0]=='-' && !uri[1]) ? "stdin://sourcer68" : uri, 1, 0);
  if (vfs68_open(is) == -1) {
    vfs68_destroy(is);
    is = 0;
  }
  return is;
}

static void myclose(void * file)
{
  if (file) {
    vfs68_close(file);
    vfs68_destroy(file);
  }
}

static int mylength(void * file)
{
  return vfs68_length(file);
}

static int myread(void * file, void * buf, int bytes)
{
  return vfs68_read(file, buf, bytes);
}

#else

#include <stdio.h>

static void * myopen(char * uri)
{
  FILE * f;
  f = (uri[0]=='-' && !uri[1]) ? stdin : fopen(uri,"rb");
  return f;
}

static void myclose(void * file)
{
  FILE * f = (FILE*) file;
  if (f && f != stdin)
    fclose(f);
}

static int mylength(void * file)
{
  FILE * f = (FILE*) file;
  int size = -1;
  if (fseek(f,0,SEEK_END) != -1)
    size = ftell(f);
  if (fseek(f,0L,SEEK_SET) == -1)
    size = -1;
  return size;
}

static int myread(void * file, void * buf, int bytes)
{
  FILE * f = (FILE*) file;
  return fread(buf,1,bytes,f);
}

#endif

static int load_bin(char * uri, uint8_t ** pbuf)
{
  uint8_t * b = 0;
  int size, byte = -1;
  void * file = myopen(uri);

  if (!file)
    goto error;

  size = mylength(file);
  if (!size) {
    byte = 0;
  } else if (size > 0) {
    b = malloc(size);
    if (!b)
      goto error;
    if (myread(file,b,size) != size)
      goto error;
    byte = size;
  } else {
    int cnt;
    /* Don't have a size just load till no more data */
    for (cnt = size = 0;;) {
      char tmp[256];
      int n;
      n = myread(file, tmp, sizeof(tmp));
      if (n == -1)
        goto error;
      if (n == 0)
        break;
      if (n+cnt > size) {
        uint8_t * newbuf;
        int need = size > 0 ? size << 1 : 0x10000;
        assert (need >= n+cnt);
        newbuf = realloc(b,need);
        if (!newbuf)
          goto error;
        size = need;
        b = newbuf;
      }
      memcpy(b+cnt, tmp, n);
      cnt += n;
    }
    byte = cnt;
  }

error:
  myclose(file);
  if (byte < 0) {
    free(b);
    b = 0;
  }

  if (pbuf)
    *pbuf = b;

  dmsg("loaded binary \"%s\" (%d)\n", uri, byte);
  return byte;
}

void exe_del(exe_t * exe)
{
  if (exe) {
    free(exe->uri);
    vec_del(exe->sections);
    vec_del(exe->symbols);
    free(exe);
  }
}

static exe_t * exe_new(char * uri)
{
  exe_t * exe;

  if (exe = calloc(1, sizeof(exe_t)), !exe)
    goto error;
  exe->loadas = LOAD_AS_AUTO;
  if (exe->uri = strdup(uri), !exe->uri)
    goto error;
  if (exe->sections = sections_new(8), !exe->sections)
    goto error;
  if (exe->symbols = symbols_new(256), !exe->symbols)
    goto error;
  if (exe->relocs = address_new(256), !exe->relocs)
    goto error;
  if (exe->entries = address_new(256), !exe->entries)
    goto error;
  goto done;

error:
  exe_del(exe);
  exe = 0;
  emsg(-1, "%s -- %s\n", strerror(errno), uri);

done:
  return exe;
}

static int myishex(int c) {
  if (c>='0' && c<='9') return c-'0';
  if (c>='a' && c<='f') return c-'a'+10;
  if (c>='A' && c<='F') return c-'A'+10;
  return -1 - (c=='.' || c==0);         /* -1 or -2 for valid term */
}

static uint_t address_from_name(char *uri)
{
  uint_t org = EXE_DEFAULT, adr; int c;
  char *sl1 = strrchr(uri,'/');
  char *sl2 = 0;
#ifdef WIN32
  sl2 = strrchr(uri,'\\');
#endif
  if (sl2 > sl1)
    sl1 = sl2;                          /* get which ever is last */
  if (sl1)
    ++sl1;                              /* skip '/' */
  else
    sl1 = uri;
  for (sl2=sl1, adr=0; (c = myishex(*sl2)) >= 0; ++sl2)
    adr = adr*16 + c;
  if (sl2-sl1 >= 4 && c == -2)
    org = adr;
  return org;
}

static exe_t * load_as_bin(char * uri, uint_t org, uint8_t * data, int size)
{
  exe_t *exe = 0, *e = 0;
  int bss_size = 0;

  if (org == (uint_t) EXE_DEFAULT)
    org = address_from_name(uri);

  if (e = exe_new(uri), !e)
    goto error;

  e->loadas = LOAD_AS_BIN;
  e->ispic  = org == (uint_t) EXE_DEFAULT;
  if (e->ispic)
    org = EXE_ORIGIN;
  if (e->mbk = mbk_new(org, size), !e->mbk)
    goto error;
  memcpy(e->mbk->mem, data, size);

  /* $$$ TODO: optional auto BSS finder */
  if (1) {
    int i;
    for (i = size-1; i >= 0 && !data[i]; --i)
      ;
    if (++i < size)
      bss_size = size - i;
  }

  symbol_add(e->symbols, "Start", org, 0);
  symbol_add(e->symbols, "End", org+size, 0);

  if (size - bss_size > 0) {
    section_add(e->sections, "TEXT", org, size-bss_size, SECTION_X);
    addr_add(e->entries,org);
  }
  if (bss_size > 0) {
    uint_t bss_org = org + size - bss_size;
    section_add(e->sections, "BSS", bss_org, bss_size, SECTION_0);
    symbol_add(e->symbols, "Bss", bss_org, 0);
  }

  exe = e;
  e = 0;
error:
  exe_del(e);

  return exe;
}

static exe_t * load_as_sc68(char * uri, uint_t org, uint8_t * data, int size)
{
  exe_t *exe = 0, *e = 0;
#ifndef SOURCER68_FILE68
  emsg("sc68 not supported by this build\n");
#else
  disk68_t * disk = 0;
  uint8_t * replay_buf = 0;
  int replay_len = 0, data_off = 0, full_len;

  if (disk = file68_load_mem(data, size), disk) {
    int track = disk->force_track ? disk->force_track : disk->def_mus+1;
    music68_t * mus = disk->mus + track - 1;

    dmsg("using track #%d\n", track);

    if (e = exe_new(uri), !e)
      goto error;

    e->loadas = LOAD_AS_SC68;
    e->ispic  = mus->has.pic;

    dmsg("replay ? %s\n", mus->replay?mus->replay:"");

    if (mus->replay) {
      char replay[64];
      strcpy(replay,"sc68://replay/");
      strcat68(replay,mus->replay, sizeof(replay));
      replay[sizeof(replay)-1] = 0;
      replay_len = load_bin(replay, &replay_buf);
      dmsg("replay '%s'  loaded as %d bytes\n", replay, replay_len);
      if (replay_len < 0)
        goto error;
    }

    org = (org == EXE_DEFAULT) ? mus->a0 : org;

    data_off = (replay_len+1) & -2;
    full_len = data_off + mus->datasz;
    e->mbk = mbk_new(org, full_len);
    if (!e->mbk)
      goto error;

    if (replay_len)
      memcpy(e->mbk->mem, replay_buf, replay_len);
    memcpy(e->mbk->mem+data_off, mus->data, mus->datasz);

    /* symbols */
    symbol_add(e->symbols, "sc68_init", org+0, /* SYM_INST| */SYM_FUNC);
    symbol_add(e->symbols, "sc68_kill", org+4, /* SYM_INST| */SYM_FUNC);
    symbol_add(e->symbols, "sc68_play", org+8, /* SYM_INST| */SYM_FUNC);
    if (data_off) {
      symbol_add(e->symbols, "sc68_data", org+data_off, SYM_DATA);
      symbol_add(e->symbols, "sc68_datasz", mus->datasz, 0);
    }

    /* sections */
    if (!data_off) {
      section_add(e->sections, "TEXT", org, mus->datasz, SECTION_X);
    } else {
      section_add(e->sections, "TEXT", org, data_off, SECTION_X);
      section_add(e->sections, "DATA", org+data_off, mus->datasz, 0);
    }

    addr_add(e->entries,e->mbk->org+0);
    addr_add(e->entries,e->mbk->org+4);
    addr_add(e->entries,e->mbk->org+8);
  }

  exe = e;
  e = 0;
error:
  exe_del(e);
  file68_free(disk);
  free(replay_buf);
#endif
  return exe;
}


static void reloc_callback(tosexec_t * e, unsigned int org, unsigned int off)
{
  exe_t * const exe = e->user;
  vec_t * const rel = exe->relocs;

  assert(rel);
  /* DMSG("relocating org:%06x off:$%06x\n", org, off); */
  addr_add(rel, org+off);
  assert(exe->mbk);
  mbk_setmib(exe->mbk, org+off, 0, MIB_RELOC);
}


#include <stdio.h>
static const char * dri_flagstr(uint_t flags)
{
  static char temp[256];
  const int m = sizeof(temp)-1;
  int i = 0;

  assert( (flags & 0xFFFF) == flags );
  flags &= 0xFFFF;

  if (!flags)
    i += snprintf(temp+i,m-i,"<NDEF>");

  if ( flags & TOS_SYMB_DEF ) {
    i += snprintf(temp+i,m-i,",DEF"+!i);
    flags &= ~TOS_SYMB_DEF;
  }

  if ( flags & TOS_SYMB_EQU ) {
    i += snprintf(temp+i,m-i,",EQU"+!i);
    flags &= ~TOS_SYMB_EQU;
  }

  if ( flags & TOS_SYMB_GLOBAL ) {
    i += snprintf(temp+i,m-i,",GBL"+!i);
    flags &= ~TOS_SYMB_GLOBAL;
  }

  if ( flags & TOS_SYMB_REG ) {
    i += snprintf(temp+i,m-i,",REG"+!i);
    flags &= ~TOS_SYMB_REG;
  }

  if ( flags & TOS_SYMB_XREF ) {
    i += snprintf(temp+i,m-i,",XRF"+!i);
    flags &= ~TOS_SYMB_XREF;
  }

  if ( flags & TOS_SYMB_DATA ) {
    i += snprintf(temp+i,m-i,",DAT"+!i);
    flags &= ~TOS_SYMB_DATA;
  }
  if ( (flags & (TOS_SYMB_TEXT|0x80)) == TOS_SYMB_TEXT) {
    i += snprintf(temp+i,m-i,",TXT"+!i);
    flags &= ~TOS_SYMB_TEXT;
  }

  if ( flags & TOS_SYMB_BSS ) {
    i += snprintf(temp+i,m-i,",BSS"+!i);
    flags &= ~TOS_SYMB_BSS;
  }

  if ( (flags & TOS_SYMB_LNAM) == TOS_SYMB_LNAM ) {
    i += snprintf(temp+i,m-i,",LNG"+!i);
    flags &= ~TOS_SYMB_LNAM;
  }

  if ( (flags & TOS_SYMB_FARC) == TOS_SYMB_FILE ) {
    i += snprintf(temp+i,m-i,",FIL"+!i);
    flags &= ~TOS_SYMB_FILE;
  } else if ( (flags & TOS_SYMB_FARC) == TOS_SYMB_FARC ) {
    i += snprintf(temp+i,m-i,",ARC"+!i);
    flags &= ~TOS_SYMB_FARC;
  }
  if (flags)
    i += snprintf(temp+i,m-i,(",{%x}"+!i), flags);
  temp[m] = 0;
  return temp;
}

static exe_t * load_as_tos(char * uri, uint_t org, uint8_t * data, int size)
{
  exe_t * exe = 0, *e;
  tosfile_t * tosfile = (tosfile_t *)data;
  tosexec_t tosexec;
  uint_t off;

  if (org == (uint_t)EXE_DEFAULT)
    org = EXE_ORIGIN;

  if (e = exe_new(uri), !e)
    goto error;

  if (tos_header(&tosexec, tosfile, size))
    goto error;
  e->ispic = !tosexec.rel.len;

  e->mbk = mbk_new(org, tosexec.txt.len+tosexec.dat.len+tosexec.bss.len);
  assert(e->mbk);
  if (!e->mbk)
    goto error;

  if (tosexec.txt.len) {
    off = tosexec.txt.off;
    memcpy(e->mbk->mem+off, tosexec.img+off, tosexec.txt.len);
    section_add(e->sections, "TEXT", org+off, tosexec.txt.len, SECTION_X);
  }
  if (tosexec.dat.len) {
    off = tosexec.dat.off;
    memcpy(e->mbk->mem+off, tosexec.img+off, tosexec.dat.len);
    section_add(e->sections, "DATA", org+off, tosexec.dat.len, 0);
  }
  if (tosexec.bss.len) {
    off = tosexec.bss.off;
    memset(e->mbk->mem+off, 0, tosexec.bss.len);
    section_add(e->sections, "BSS", org+off, tosexec.bss.len, SECTION_0);
  }

  /* Hook (hack) tosexec::img so that relocations occur in the memory
   * block instead of the tos file image. Alternatively we could use
   * the reloc_callback() function for this task.
   */
  tosexec.user = e;
  tosexec.img  = e->mbk->mem;
  tos_reloc(&tosexec, org, reloc_callback);
  tosexec.img  = tosfile->xt.jump;

  assert(e->mbk->org == org);

  if (tosexec.sym.len) {
    const uint_t top = org;
    const uint_t bot = top + e->mbk->len;
    sym_t * symb;
    tosxymb_t xymb;
    int walk;

    DMSG("TOS image<$%08X-$%08X> +%u\n",top,bot-1,bot-top);

    for (walk=0; (walk = tos_symbol(&xymb,&tosexec,tosfile,walk)) > 0;) {
      int flags = 0, type = xymb.type, idx;
      struct tossection * tos_section = 0;
      uint_t addr;

      /* DMSG("TOS: got a symbol, next at %u\n", walk); */
      DMSG("SYMB<$%08x $%04x> \"%s\" [%s]\n",
           xymb.addr, xymb.type, xymb.name, dri_flagstr(xymb.type) );

      if ( (type & TOS_SYMB_LNAM) == TOS_SYMB_LNAM )
        type &= ~TOS_SYMB_LNAM;
      if ( (type & TOS_SYMB_FILE) == TOS_SYMB_FILE )
        continue;
      switch (type & TOS_SYMB_0F00) {

      case TOS_SYMB_TEXT:
        tos_section = &tosexec.txt;


        if (tosexec.has.mint && xymb.name[0] == '_')
          /* HACK: probably a "C" function */
          /* flags |= SYM_FUNC */;

        break;

      case TOS_SYMB_DATA:
        tos_section = &tosexec.dat;
        flags |= SYM_DATA;
        break;

      case TOS_SYMB_BSS:
        tos_section = &tosexec.bss;
        flags |= SYM_DATA;
        break;

      default:
        wmsg(-1,
             "TOS unexpected symbol type\n"
             "SYM<$%08X,$%04X,\"%s\">\n",
             xymb.addr,xymb.type,xymb.name);
        break;
      }

      if (!tos_section)
        continue;

      addr = xymb.addr;
      if (addr >= tos_section->len) {
        wmsg(-1,
             "TOS symbol out of section range [$%08X-$%08X] +%u\n"
             "SYM<$%08X,$%04X,\"%s\">\n",
             tos_section->off,
             tos_section->off+tos_section->len-1,
             tos_section->len,
             xymb.addr,xymb.type,xymb.name);
      }

      addr += org + tos_section->off;
      if (addr < top || addr > bot) {
        wmsg(-1,
             "TOS symbol out of range\n"
             "SYM<$%08X,$%04X,\"%s\">\n",
             addr,xymb.type,xymb.name);
      }
      else if (flags & SYM_FUNC) {
        addr_add(e->entries,addr);
      }

      idx = symbol_add(e->symbols, xymb.name, addr, flags);
      if (idx < 0) {
        emsg(-1,
             "TOS failed to add symbol\n"
             "SYM<$%08X,$%04X,\"%s\">\n",
             xymb.addr,xymb.type,xymb.name);
        /* goto error; */
      } else {
        symb = symbol_get(e->symbols,idx);
        assert(symb);
        dmsg("TOS symb #%05d "
             "SYM<$%08X,$%04X,\"%s\">\n",
             idx,xymb.addr,xymb.type,xymb.name);
      }
    }

    if (walk < 0)
      wmsg(-1, "failed to walk TOS symbol table (%d bytes)\n",
           tosexec.sym.len);
  }

  if (e->symbols->cnt) {
    sym_t * symb;
    int idx;
    symbol_sort_byname(e->symbols);
    idx = 0;
    DMSG("%d symbols sorted by name\n",e->symbols->cnt);
    while (symb = symbol_get(e->symbols,idx++), symb)
      DMSG("symb #%04d %p $%08x $%04x \"%s\"\n", idx, symb,
           symb->addr, symb->flag, symb->name);

    symbol_sort_byaddr(e->symbols);
    idx = 0;
    DMSG("%d symbols sorted by address\n",e->symbols->cnt);
    while (symb = symbol_get(e->symbols,idx++), symb)
      DMSG("symb #%04d %p $%08x $%04x \"%s\"\n", idx, symb,
           symb->addr, symb->flag, symb->name);
  }

  mbk_setmib(e->mbk, e->mbk->org+0, 0, MIB_ENTRY);
  if (tosexec.start)
    mbk_setmib(e->mbk, e->mbk->org+tosexec.start, 0, MIB_ENTRY);

  exe = e;
  e = 0;
error:
  exe_del(e);
  return exe;
}

static int can_be_ice(uint8_t * data) {
  return (data[0] == 'I' && (data[1]|32) == 'c' &&
          (data[2]|32) == 'e' && data[3] == '!');
}

static int can_be_sc68_v(uint8_t * data, unsigned int size, int version) {
  const char * id = file68_identifier(version); unsigned int l;
  return (id && size > (l=strlen(id)) && !memcmp(id,data,l));
}

static int can_be_sndh(uint8_t * data, int size) {
  int i;
  for (i=0; i<12; i+=4)
    if (! (data[i] == 0x60 || (data[i] == 0x4e && data[i+1] == 0x75)))
      return 0;
  for ( ; i <= size-4; ++i)
    if (!memcmp(data+i,"SNDH",4))
      return 1;
  return 0;
}

static int can_be_sc68(uint8_t * data, int size)
{
#ifdef SOURCER68_FILE68
  const int min = 32;
  if (size > min) {
    if (can_be_sc68_v(data,size,1))
      return 1;                         /* sc68 v1 */
    if (can_be_sc68_v(data,size,2))
      return 2;                         /* sc68 v2 */
    if (can_be_ice(data))
      return 3;                         /* ice! sndh */
    if (can_be_sndh(data,min))
      return 4;                         /* sndh */
    if (gzip68_is_magic(data))
      return 5;                         /* gzip (sc68?) */
  }
#endif
  return 0;
}

static exe_t * load_as_auto(char * uri, uint_t org, uint8_t * data, int size)
{
  exe_t * exe = 0;

  if (!exe && can_be_sc68(data,size)) {
    dmsg("trying for sc68 -- %s\n", uri);
    exe = load_as_sc68(uri, org, data, size);
  }

  if (!exe && size >= 30 && data[0] == 0x60 && data[1] == 0x1a) {
    dmsg("trying for tos -- %s\n", uri);
    exe = load_as_tos(uri, org, data, size);
  }

  if (!exe) {
    dmsg("trying for binary -- %s\n", uri);
    exe = load_as_bin(uri, org, data, size);
  }

  return exe;
}

exe_t * exe_load(char * uri, uint_t org, int loadas)
{
  exe_t * exe = 0;
  uint8_t * data = 0;
  int size;

  /* Load as a binary */
  size = load_bin(uri,&data);
  if (size < 0) {
    emsg(-1,"could not load -- %s\n", uri);
  } else
    switch (loadas) {

    case LOAD_AS_AUTO:
      exe = load_as_auto(uri, org, data, size);
      break;

    case LOAD_AS_SC68:
      exe = load_as_sc68(uri, org, data, size);
      break;

    case LOAD_AS_TOS:
      exe = load_as_tos(uri, org, data, size);
      break;

    case LOAD_AS_BIN:
      exe = load_as_bin(uri, org, data, size);
      break;

    default:
      assert(!"should not happen");
      emsg(-1,"internal error (load as type) -- %d\n", loadas);
      break;
    }

  free(data);

  if (exe) {
    if (exe->sections)
      vec_sort(exe->sections,0);
    /* $$$ XXX TBC */
  }

  return exe;
}
