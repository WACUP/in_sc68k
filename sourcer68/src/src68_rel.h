/**
 * @ingroup  sourcer68_prg
 * @file     src68_relr.h
 * @author   Benjamin Gerard
 * @date     2015-03-18
 * @brief    relocations
 */

/* Copyright (c) 1998-2016 Benjamin Gerard */

#ifndef SOURCER68_REL_H
#define SOURCER68_REL_H

#include "src68_vec.h"

typedef struct {
  obj_t  obj;                     /**< interface (must be first). */
  uint_t adr;                     /**< address of long to relocate. */
} rel_t;

/**
 * Create a partition container.
 *
 * @param  max  Initial maximum number of partitions
 * @retval 0 on error
 * @return partition container.
 */
vec_t * relocs_new(uint_t max);

/**
 * Get section at index.
 * @par rel  relocation container.
 * @par idx  index
 * @retval 0 on error
 * @return relocation entry
 */
rel_t * reloc_get(vec_t * rel, int idx);

/**
 * Create and add a section.
 *
 * @param  rel  the relocations container.
 * @param  org  origin of section.
 * @param  off  offset in section.
 *
 * @return added relocation index
 * @retval -1 on error
 */
int reloc_add(vec_t * rel, uint_t org, uint_t off);

/**
 * Find if there is a relocation at a given address.
 */
int reloc_exists(vec_t * rel, uint_t adr);

#endif
