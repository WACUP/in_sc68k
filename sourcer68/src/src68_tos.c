/*
 * @file    src68_tos.c
 * @brief   Atari TOS executable format.
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

#include "src68_tos.h"
#include "src68_msg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DRI_SYMBOL_SIZE 14u

static inline uint_t readW(const uint8_t * b) {
  return (b[0]<<8) | b[1];
}

static inline uint_t readL(const uint8_t * b) {
  return (b[0]<<24) | (b[1]<<16) | (b[2]<<8) | b[3];
}

static inline void writeL(uint8_t * b, uint_t v) {
  b[0] = v>>24; b[1] = v>>16; b[2] = v>>8; b[3] = v;
}

int tos_reloc(tosexec_t * tosexec, uint_t base, toscall_f toscall)
{
  const uint_t end = tosexec->txt.len + tosexec->dat.len - 4;
  uint_t idx,off,add;

  if (!tosexec->rel.len) {
    DMSG("TOS relocation table is empty\n");
    return 0;
  }

  if (!tosexec->reltbl) {
    tosexec->reltbl = tosexec->img + tosexec->rel.off;
    DMSG("TOS set reloc table pointer to %p\n", tosexec->reltbl);
  }

  for (idx=4, off=0, add=readL(tosexec->reltbl);
       add; add = tosexec->reltbl[idx++]) {
    off += add;
    if (add == 1)
      off += 253;
    else {
      if (off & 1) {
        DMSG("TOS reloc odd offset at idx:$%06x offset:$%06x\n",
             idx, off);
        return TOS_ERR_REL_ODD;
      }
      if (off > end) {
        DMSG("TOS reloc offset out of range at idx:$%06x offset:$%06x\n",
             idx, off);
        return TOS_ERR_REL_OOR;
      }
      /* DMSG("TOS reloc longword idx:$%06x off:$%06x\n",idx,off); */
      if (base)
        writeL(tosexec->img+off, readL(tosexec->img+off) + base);
      if (toscall)
        toscall(tosexec, base, off);
    }
    if (idx >= tosexec->rel.len) {
      DMSG("TOS reloc index out of range at idx:$%06x offset:$%06x\n",
           idx,off);
      return TOS_ERR_REL_IDX;
    }
  }
  if (idx != tosexec->rel.len) {
    DMSG("TOS reloc prematured end ? %u != %u (%d)\n",
         idx, tosexec->rel.len, tosexec->rel.len-idx);
  }
  return 0;
}

int tos_header(tosexec_t * tosexec, tosfile_t * tosfile, uint_t filesize)
{
  const uint_t off_max = filesize - TOS_HEADER_SIZE;
  int rel_size;

  memset(tosexec, 0, sizeof(*tosexec));

  /* Need at least the header plus a 68 instruction for a valid
   * executable. Then check for the magic bra.s instruction. */
  if (filesize < TOS_HEADER_SIZE+2 ||
      (tosfile->bras[0] != 0x60 || tosfile->bras[1] != 0x1A)) {
    DMSG("not TOS (%s) %0x02%0X02 != 601A\n",
         "no magic", tosfile->bras[0],tosfile->bras[1]);
    return TOS_ERR_NOT_TOS;
  }

  /* currently points to file data, later will point on TPA */
  tosexec->img     = tosfile->xt.jump;
  tosexec->start   = 0;

  /* sections lengths */
  tosexec->txt.len = readL(tosfile->txt_sz);
  tosexec->dat.len = readL(tosfile->dat_sz);
  tosexec->bss.len = readL(tosfile->bss_sz);
  tosexec->sym.len = readL(tosfile->sym_sz);

  /* sections offsets */
  tosexec->txt.off = 0;
  tosexec->dat.off = tosexec->txt.off + tosexec->txt.len;
  tosexec->bss.off = tosexec->dat.off + tosexec->dat.len;
  tosexec->sym.off = tosexec->bss.off;
  tosexec->rel.off = tosexec->sym.off + tosexec->sym.len;

  DMSG("TOS-HEADER:\n"
       " MAGIC %02X%02x\n"
       " TEXT  %02X%02X%02X%02X\n"
       " DATA  %02X%02X%02X%02X\n"
       " BSS   %02X%02X%02X%02X\n"
       " SYMB  %02X%02X%02X%02X\n"
       " RESV  %02X%02X%02X%02X\n"
       " FLAG  %02X%02X%02X%02X\n"
       " ABS   %02X%02X\n",
       tosfile->bras[ 0],tosfile->bras[ 1],
       tosfile->bras[ 2],tosfile->bras[ 3],tosfile->bras[ 4],tosfile->bras[ 5],
       tosfile->bras[ 6],tosfile->bras[ 7],tosfile->bras[ 8],tosfile->bras[ 9],
       tosfile->bras[10],tosfile->bras[11],tosfile->bras[12],tosfile->bras[13],
       tosfile->bras[14],tosfile->bras[15],tosfile->bras[16],tosfile->bras[17],
       tosfile->bras[18],tosfile->bras[19],tosfile->bras[20],tosfile->bras[21],
       tosfile->bras[22],tosfile->bras[23],tosfile->bras[24],tosfile->bras[25],
       tosfile->bras[26],tosfile->bras[27]);

  /* DMSG("TOS-HEADER:\n" */
  /*      " MAGIC %02X%02x\n" */
  /*      " TEXT  %06X +%u\n" */
  /*      " DATA  %06X +%u\n" */
  /*      " BSS   %06X +%u\n" */
  /*      " REL   %06X +%u\n" */
  /*      " RESERVED %02X-%02X-%02X-%02X\n" */
  /*      " FLAGS    %02X-%02X\n" */

  /*      "END-HEADER\n", */
  /*      tosfile->bras[0],tosfile->bras[1], */
  /*      tosexec->txt.off,tosexec->txt.len, */
  /*      tosexec->dat.off,tosexec->dat.len, */
  /*      tosexec->bss.off,tosexec->bss.len, */
  /*      tosexec->rel.off,tosexec->rel.len, */
  /* ); */

  /* no section should points outside of data range. */
  if (tosexec->dat.off >= off_max || tosexec->sym.off >= off_max ||
      tosexec->rel.off >= off_max) {
    DMSG("not TOS (%s)\n","section out of range");
    return TOS_ERR_SEC_OOR;
  }

  /* no section should points to an odd address */
  if ((tosexec->dat.off | tosexec->sym.off | tosexec->rel.off)&1) {
    DMSG("not TOS (%s)\n","section odd address");
    return TOS_ERR_SEC_ODD;
  }

  /* Atari MiNT extensions ? */
  tosexec->has.mint = !memcmp(tosfile->mint, "MiNT", 4);
  DMSG("TOS with%s MiNT\n", tosexec->has.mint?"":"out");

  if (tosexec->has.mint) {
    static const uint8_t mintcode[8] = {
      0x20,0x3a,0x00,0x1a,0x4e,0xfb,0X08,0xfa
    };

    if (memcmp(tosfile->xt.jump,mintcode,8)) {
      DMSG("TOS MiNT with invalid jump entry "
           "(%02X%02X-%02X%02X-%02X%02X%02X%02X)\n",
           tosfile->xt.jump[0],tosfile->xt.jump[1],
           tosfile->xt.jump[2],tosfile->xt.jump[3],
           tosfile->xt.jump[4],tosfile->xt.jump[5],
           tosfile->xt.jump[6],tosfile->xt.jump[7]);
    }
    tosexec->flags   = readL(tosfile->flags);
    tosexec->has.abs = !!readW(tosfile->noreloc);
    tosexec->start   = readL(tosfile->xt.entry);
    if (tosexec->start != TOS_MINT_STD_SIZE-TOS_HEADER_SIZE) {
      DMSG("TOS MiNT extension unexpected entry +$%08X\n", tosexec->start);
    }

    if (tosexec->start > tosexec->txt.len-2) {
      DMSG("TOS MiNT extension jump out of range +$%08X\n", tosexec->start);
      return TOS_ERR_JMP_OOR;
    }
  }
  DMSG("TOS real entry point at +$%08X\n", tosexec->start);

  if (!tosexec->has.abs) {
    rel_size = off_max - tosexec->rel.off;
    if (rel_size < 4)
      return TOS_ERR_REL_IDX;
    tosexec->rel.len = rel_size;
  }

  /* set symbol table format value (either DRI or GNU) */
  tosexec->has.symfmt = !tosexec->has.mint
    ? TOS_MINT_SYMB_DRI
    : readL(tosfile->xt.symbol_format)
    ;
  DMSG("TOS symbol format is %s\n",
       tosexec->has.symfmt == TOS_MINT_SYMB_DRI ? "DRI" : "GNU");

  /* runtime checks for the symbols structure size */
  assert(sizeof(tossymb_t) == DRI_SYMBOL_SIZE);
  if (sizeof(tossymb_t) != DRI_SYMBOL_SIZE)
    return TOS_ERR_INTERNAL;

  /* Paranoiac test: the length of the symbol section shoould be a
   * multiple of the symbol struct size. Remove this test if it
   * happens for a legit reason I can't think of ATM.
   */
  if (tosexec->has.symfmt == TOS_MINT_SYMB_DRI &&
      tosexec->sym.len % DRI_SYMBOL_SIZE) {
    DMSG("TOS DRI symbol inconsistent section size %u (+%u)\n",
         tosexec->sym.len, tosexec->sym.len % DRI_SYMBOL_SIZE);
    return TOS_ERR_SYM_SIZ;
  }

  assert(sizeof(tossymb_t) == DRI_SYMBOL_SIZE);
  if (sizeof(tossymb_t) != DRI_SYMBOL_SIZE)
    return 666;
  /* else { */
  /*   uint_t si, nsi = tosexec->sym.len / sizeof(tossymb_t); */
  /*   for (si=0; si<nsi; ++si) { */
  /*     tossymb_t * symb = (tossymb_t *)(tosexec->img+tosexec->sym.off)+si; */
  /*     char name[9]; */
  /*     uint_t flag = readW(symb->type); */
  /*     strncpy(name, symb->name,8); */
  /*     name[8]=0; */
  /*     printf("[%d] \"%s\" %04x (%s) %08x\n", */
  /*            si, name, flag, "", readL(symb->addr)); */
  /*   } */
  /* } */

  return tosexec->rel.len
    ? tos_reloc(tosexec, 0, 0)
    : 0
    ;
}

int tos_symbol(tosxymb_t * xymb,
               tosexec_t * tosexec,  tosfile_t * tosfile, int walk)
{
  tossymb_t * symb;

  if (tosexec->has.symfmt != TOS_MINT_SYMB_DRI) {
    DMSG("TOS unsupported symbol format (GNU)\n");
    return 0;
  }

  xymb->name[0] = 0; xymb->type = 0; xymb->addr = 0;

  if (walk == tosexec->sym.len)
    return 0;                           /* the end */

  if (walk % DRI_SYMBOL_SIZE || walk >= tosexec->sym.len) {
    DMSG("TOS symbol table invalid walk offfset (%u)\n", walk);
    return -1;
  }

  assert( sizeof(symb->name)+DRI_SYMBOL_SIZE < sizeof(xymb->name) );

  symb = (tossymb_t *) (tosfile->xt.jump + tosexec->sym.off + walk);
  walk += DRI_SYMBOL_SIZE;
  xymb->type = readW(symb->type);
  xymb->addr = readL(symb->addr);
  strncpy(xymb->name, symb->name, sizeof(symb->name));
  xymb->name[sizeof(symb->name)] = 0;
  if ((xymb->type & TOS_SYMB_LNAM) == TOS_SYMB_LNAM) {
    if (walk >= tosexec->sym.len) {
      DMSG("TOS symbol table long name overflow\n");
      return -1;
    }
    strncpy(xymb->name+sizeof(symb->name), symb[1].name, DRI_SYMBOL_SIZE);
    xymb->name[sizeof(symb->name)+DRI_SYMBOL_SIZE] = 0;
    walk += DRI_SYMBOL_SIZE;
  }
  return walk;
}
