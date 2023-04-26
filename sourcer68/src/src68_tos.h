/**
 * @ingroup  sourcer68_prg
 * @file     src68_tos.h
 * @author   Benjamin Gerard
 * @date     2015-03-18
 * @brief    Atari TOS executable format.
 */

/* Copyright (c) 1998-2016 Benjamin Gerard */

#ifndef SOURCER68_TOS_H
#define SOURCER68_TOS_H

#include "src68_def.h"

/**
 * TOS executable file structure.
 */
struct tosfile {
  uint8_t bras[2];                      /* 00 60 1a */
  uint8_t txt_sz[4];                    /* 02 */
  uint8_t dat_sz[4];                    /* 06 */
  uint8_t bss_sz[4];                    /* 0A */
  uint8_t sym_sz[4];                    /* 0E */
  uint8_t mint[4];                      /* 12 "MiNT" 0x4d694e54 */
  uint8_t flags[4];                     /* 16 Atari special flags */
  uint8_t noreloc[2];                   /* 18 True if absolute */
  /* 1C TEXT section starts here.  in case of MiNT it's a sequance of
   *    m68k instructions to jump to the real entry point
   *    203a-001a-4efb08fa
   */

  /**
   * mint extended header (rip from prg-mint.c)
   */
  struct mint_xt {
    uint8_t jump[8];           /* jump instructions (203a-001a-4efb08fa) */
    uint8_t info[4];           /* Magic number and stuff.  */
    uint8_t text[4];           /* Length of text section in bytes. */
    uint8_t data[4];           /* Length of data section.  */
    uint8_t bss[4];            /* Length of standard symbol table. */
    uint8_t syms[4];           /* Length of symbol table. */
    uint8_t entry[4];          /* Start address. */
    uint8_t trsize[4];         /* Length of text relocation info. */
    uint8_t drsize[4];         /* Length of data relocation info. */
    uint8_t tparel_pos[4]; /* File position of TPA relative relocation
                            * info.
                            */
    uint8_t tparel_size[4]; /* Length of TPA relative relocation info. */

    /* This is for extensions.
     *
     * If stacksize is hardcoded into the executable you will find it
     * at file offset g_stkpos. If not this is nil.
     */
    uint8_t stkpos[4];

    /* Format of the symbol table. See definitions for
      * _MINT_SYMBOL_FORMAT above.
      */
    uint8_t symbol_format[4];

    /* Pad with zeros. */
    uint8_t pad0[172];
  } xt;
};
typedef struct tosfile tosfile_t;

/**
 * TOS constants.
 */
enum {
  TOS_HEADER_SIZE = 28,               /**< Classic TOS header size. */
  TOS_MINT_STD_SIZE = 256 /**< MiNT TOS header size. @see tosfile_t::mint */
};

/**
 * TOS program flags (tosfile_t::flags)
 * @see http://toshyp.atari.org/en/005005.html
 */
enum {

   /**
    * Fastload flag. If this bit is set, then the stack will not be
    * cleared, but only the BSS segment preinitialized.
    */
  TOS_FLG_FASTLOAD = 1,

   /**
    * The program may be loaded into the fast alternate RAM.
    */
  TOS_FLG_FASTRAM = 2,

   /**
    * Memory requests via Malloc may be allocated from alternate
    * RAM.
    */
  TOS_FLG_ALTRAM = 4,

  /**
   * Reserved for shared library (must be clear).
   */
  TOS_FLG_SHL = 8,

  TOS_FLG_MEM_MASK = 0x70,

   /**
    * Only the process itself, and the operating system, may access
    * the memory.
    */
  TOS_FLG_MEM_PRIVATE = 0x00,

  /**
   * The memory is completely unprotected and hence all programs can
   * access it.
   */
  TOS_FLG_MEM_GLOBAL = 0x10,

  /**
   * The memory can be accessed by all processes that run in
   * supervisor-mode
   */
  TOS_FLG_MEM_SUPER = 0x20,

 /**
  * Any process can read from the memory; but writing is only
  * permitted by the process itself, as well as the operating
  * system.
  */
  TOS_FLG_MEM_RO = 0x30,

  /**
   * Reserved (must be 0).
   */
  TOS_FLG_RES8_MASK = 0xF00,

  /**
   * TEXT section may be shared.
   */
  TOS_FLG_SHTEXT = 4096,

  /**
   * Reserved (must be 0).
   */
  TOS_FLG_RES13_MASK = 0x0FFFC000,

  /**
   * Maximum amount of memory to be allocated by the program from
   * alternate RAM if the machine has more ST-RAM than alternate
   * RAM.
   */
  TOS_FLG_TPASIZE_MASK = 0xF0000000,

};

/**
 * TOS symbols (aka DRI symbols).
 */
struct tossymb {
  char    name[8];
  uint8_t type[2];
  uint8_t addr[4];
};
typedef struct tossymb tossymb_t;

/**
 * Extended symbols
 */
typedef struct {
  char   name[24];
  uint_t type;
  uint_t addr;
} tosxymb_t;

/**
 * TOS symbol type.
 */
enum {
  TOS_SYMB_UNDF   = 0x0000,

  /* TOS_SYMB_AMASK  = 0xF000, */
  TOS_SYMB_DEF    = 0x8000,             /* Defined */
  TOS_SYMB_EQU    = 0x4000,
  TOS_SYMB_GLOBAL = 0x2000,
  TOS_SYMB_REG    = 0x1000,

  TOS_SYMB_0F00   = 0x0F00,
  TOS_SYMB_XREF   = 0x0800,
  TOS_SYMB_DATA   = 0x0400,             /* DATA section */
  TOS_SYMB_TEXT   = 0x0200,             /* TEXT section */
  TOS_SYMB_BSS    = 0x0100,             /* BSS section  */

  /* TOS_SYMB_CMASK  = 0x00FF, */
  TOS_SYMB_LNAM   = 0x0048, /* GST compatible long name.                 */
  TOS_SYMB_FILE   = 0x0280, /* Text file corresponding to object module. */
  TOS_SYMB_FARC   = 0x02C0, /* Text file archive (conflicts).            */
};

/**
 * TOS MiNT symbol format (mint_xt::symbol_format).
 */
enum {
  TOS_MINT_SYMB_GNU,
  TOS_MINT_SYMB_DRI
};

/**
 * Decoded TOS executable header.
 */
struct tosexec {
  void * user;                          /**< user data (cookie). */
  uint8_t * img;
  uint8_t * reltbl;

  struct tossection {
    uint_t off,len;
  } txt, dat, bss, sym, rel;
  uint_t flags;
  struct tosflags {
    uint_t mint   : 1;
    uint_t abs    : 1;
    uint_t symfmt : 1;
  } has;
  uint_t start;
};
typedef struct tosexec tosexec_t;

/**
 * Tos relocation callback.
 */
typedef void (*toscall_f)(tosexec_t *, uint_t, uint_t);

/**
 * Error codes.
 */
enum {
  TOS_ERR_SUCCESS = 0,          /**< no error */
  TOS_ERR_GENERIC,              /**< generic error */
  TOS_ERR_NOT_TOS,              /**< not a tos file */
  TOS_ERR_SEC_OOR,              /**< section out of range */
  TOS_ERR_SEC_ODD,              /**< section odd address */
  TOS_ERR_JMP_OOR,              /**< start address out of range */
  TOS_ERR_SYM_SIZ,              /**< symbol section size not multiple */
  TOS_ERR_REL_OOR,              /**< relocation addr out of range  */
  TOS_ERR_REL_ODD,              /**< relocation addr is not even */
  TOS_ERR_REL_IDX,              /**< relocation table ran out of data */
  TOS_ERR_INTERNAL = 66
};

/**
 * Reloc TOS loaded executable (aka TPA).
 *
 * @param  tosexec  decoded and loaded executable
 * @param  base     base or origin
 * @param  toscall  user callback for each relocated long word.
 *
 * @return error code
 * @retval 0 on success
 */
int tos_reloc(tosexec_t * tosexec, uint_t base, toscall_f toscall);

/**
 * Decode TOS header.
 *
 * @param  tosexec   decode and check TOS header
 * @param  tosfile   pointer to the TOS file
 * @param  filesize  size in byte of the tosfile
 *
 * @return error code
 * @retval 0 on success
 */
int tos_header(tosexec_t * tosexec, tosfile_t * tosfile, uint_t filesize);

/**
 * Walk tos symbols
 *
 * @return next walk
 * @retval 0  on end
 * @retval -1 on error
 */
int tos_symbol(tosxymb_t * xymb,
               tosexec_t * tosexec,  tosfile_t * tosfile, int walk);

#endif
