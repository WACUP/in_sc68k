/*
 * @file    src68_rel.c
 * @brief   relocations.
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

#if 0

#include "src68_rel.h"
#include <stdlib.h>
#include <string.h>

static obj_t * ireloc_new(void);

static int ireloc_cmp(const obj_t * oa, const obj_t * ob)
{
  const rel_t * sa = (const rel_t *) oa;
  const rel_t * sb = (const rel_t *) ob;
  return sa->adr - sb->adr;
}

static void ireloc_del(obj_t * obj)
{
  rel_t * rel = (rel_t *) obj;
  free(rel);
}

static objif_t if_reloc = {
  sizeof(rel_t), ireloc_new, ireloc_del, ireloc_cmp
};

static obj_t * ireloc_new(void)
{
  return obj_new(&if_reloc);
}

vec_t * relocs_new(unsigned int max)
{
  return vec_new(max,&if_reloc);
}

int reloc_add(vec_t * rel, uint_t org, uint_t off)
{
  rel_t * rel = (rel_t *)ireloc_new();
  if (rel) {
    rel->adr = org + off;
  }
  return rel;
}

rel_t * reloc_get(vec_t * rel, int idx)
{
  return (rel_t *)vec_get(rel, idx);
}

int reloc_exists(vec_t * rel, uint_t adr)
{
  rel_t obj;
  obj.obj = rel->iface;
  obj.adr = adr;
  return vec_find(rel, &obj.obj, 0, -1) >= 0;
}

#endif

