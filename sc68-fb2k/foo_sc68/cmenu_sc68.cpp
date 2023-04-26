/*
 * @file    cmenu_sc68.cpp
 * @brief   sc68 foobar200 - implement context menu
 * @author  http://sourceforge.net/users/benjihan
 *
 * Copyright (C) 2013-2014 Benjamin Gerard
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

#include "stdafx.h"

extern volatile int g_ym_asid;
extern volatile int g_ym_engine;

// Identifier of our context menu group.
static const GUID guid_cmenu  = { 0x37edb196, 0xf010, 0x446f, { 0x92, 0x45, 0xf0, 0xe4, 0x4d, 0xac, 0x17, 0x0e } };

static const GUID guid_asid   = { 0x6cfd2437, 0xe8cb, 0x4e66, { 0x91, 0xb6, 0xe4, 0x0f, 0xa5, 0x5d, 0xd3, 0x26 } };
static const GUID guid_aon    = { 0x4a5125cd, 0x8b6c, 0x45e1, { 0xa2, 0x29, 0xc6, 0x16, 0x3e, 0x11, 0x41, 0x54 } };
static const GUID guid_aoff   = { 0x34151979, 0x9646, 0x42eb, { 0xa6, 0x01, 0x40, 0x3c, 0xcf, 0x61, 0x93, 0x04 } };
static const GUID guid_aforce = { 0x646f2140, 0x882b, 0x42af, { 0x9a, 0x6d, 0x97, 0x6c, 0x9f, 0xb9, 0x44, 0x6f } };

static const GUID guid_engine = { 0x76fa907e, 0x0fac, 0x456b, { 0x9e, 0x23, 0xac, 0x16, 0xa7, 0x12, 0x2d, 0xf8 } };
static const GUID guid_blep   = { 0x3e3cd262, 0x6b21, 0x4287, { 0x88, 0x3d, 0x92, 0xe4, 0xe2, 0xbc, 0xbc, 0x16 } };
static const GUID guid_pulse  = { 0x7b26813b, 0x06bd, 0x41b7, { 0xa8, 0xfa, 0x0e, 0x8b, 0x95, 0xb2, 0x69, 0x0b } };

static const GUID guid_config = { 0x72815b85, 0x0904, 0x4d11, { 0xB3, 0xF0, 0x2C, 0x62, 0x0F, 0xFD, 0x9F, 0xC1 } };

// static const GUID guid_filter = { 0x876288c0, 0x7923, 0x4328, { 0x8f, 0x2c, 0x29, 0xf5, 0x16, 0xeb, 0xdf, 0x3c } };

enum {
  SC68_MENU_PRIO = -200
};

// Create a group containing our menu items.
#if 0
static contextmenu_group_factory g_cmenu(guid_cmenu, contextmenu_groups::root, SC68_MENU_PRIO);
#else
static contextmenu_group_popup_factory g_cmenu (guid_cmenu,  contextmenu_groups::root, "SC68",         SC68_MENU_PRIO);
static contextmenu_group_popup_factory g_asid  (guid_asid,   guid_cmenu,               "aSIDifier",    SC68_MENU_PRIO);
static contextmenu_group_popup_factory g_engine(guid_engine, guid_cmenu,               "YM Simulator", SC68_MENU_PRIO);
static contextmenu_group_popup_factory g_config(guid_config, guid_cmenu,               "Configure",    SC68_MENU_PRIO);
#endif
const

struct cmenu_def asid_defs[] = {
  { &guid_aoff,   "Off",   "Don't aSIDify sc68 tracks"  },
  { &guid_aon,    "On",    "aSIDify sc68 tracks only if it is safe"},
  { &guid_aforce, "Force", "Force aSIDfy sc68 tracks" }
};

const
struct cmenu_def engine_defs[] = {
  { &guid_blep,   "Blep",  "YM simulator using Band-Limitied-stEP synthesys (best)" },
  { &guid_pulse,  "Pulse", "YM simulator using Pulse (sc68 legacy)" },
};

// Simple context menu item class.
class cmenu_array_item : public contextmenu_item_simple {

protected:
  unsigned int * ptr, cur, n;
  const struct cmenu_def * def;
  const GUID * parent;

public:
  //cmenu_array_item() : n(0), cur(0), def(0) { }
  cmenu_array_item(const GUID * p_parent, int p_n, const struct cmenu_def * p_def, int * p_ptr = 0)
    : parent(p_parent), n(p_n), def(p_def) {
      if (p_ptr) {
        cur = -1;
        ptr = (unsigned int *) p_ptr;
      } else {
        cur = 0;
        ptr = &cur;
      }
  }
  GUID get_parent() { return *parent; }
  unsigned get_num_items() { return n; }

  void get_item_name(unsigned p_index, pfc::string_base & p_out) {
    if (p_index < n) p_out = def[p_index].name;
  }

  GUID get_item_guid(unsigned p_index) {
    return (p_index < n) ? *def[p_index].guid : pfc::guid_null;
  }

  bool get_item_description(unsigned p_index, pfc::string_base & p_out) {
    if (p_index < n && def[p_index].desc) {
      p_out = def[p_index].desc;
      return true;
    }
    return false;
  }

  void context_command(unsigned p_index,metadb_handle_list_cref p_data,const GUID& p_caller) {
    if (p_index < n) *ptr = p_index;
  }

  // Overriding this is not mandatory. We're overriding it just to demonstrate stuff that you can do such as context-sensitive menu item labels.
  bool context_get_display(
    unsigned p_index, metadb_handle_list_cref p_data,
    pfc::string_base & p_out, unsigned & p_displayflags,
    const GUID & p_caller)
  {
    const int flags = FLAG_CHECKED/*|FLAG_RADIOCHECKED*/;
    if (!__super::context_get_display(p_index, p_data, p_out, p_displayflags, p_caller)) return false;
    if (p_index == *ptr)
      p_displayflags |= flags;
    else
      p_displayflags &= ~flags;
    return true;
  }
};

// Simple context menu item class.
class cmenu_asid_item : public cmenu_array_item  {
public:
  cmenu_asid_item()
    : cmenu_array_item(&guid_asid, sizeof(asid_defs)/sizeof(*asid_defs),asid_defs, (int*)&g_ym_asid) { }

  void context_command(unsigned p_index,metadb_handle_list_cref p_data,const GUID& p_caller) {
    if (p_index < n) {
      DBG("ASID change to <%d>", n);
      if (*ptr == p_index)
        p_index = (p_index+1) & 1;
      *ptr = p_index;
    }
  }

};

class cmenu_engine_item : public cmenu_array_item  {
public:
  cmenu_engine_item()
    : cmenu_array_item(&guid_engine, sizeof(engine_defs)/sizeof(*engine_defs), engine_defs, (int*)&g_ym_engine) { }
};

//class cmenu_config_item : public cmenu_array_item {
//  int * n;
//public:
//  cmenu_config_item()
//    : cmenu_array_item(&guid_config, 0, 0, 0) { }
//};

class cmenu_config_item : public contextmenu_item_simple  {
public:

  unsigned get_num_items() {
    return 1;
  }

  GUID get_item_guid(unsigned p_index) { return guid_config; }
  void get_item_name(unsigned p_index, pfc::string_base & p_out) {
      p_out = "Configure";
  }
  bool get_item_description(unsigned p_index, pfc::string_base & p_out) {
      p_out = "sc68 configuration";
      return true;
  }

  static int conf_f(void * data, const char * key, int op, sc68_dialval_t * v)
{
  //DBG("%s %d %s\n", __FUNCTION__, op, key);
  //DBG("data: %p key:%s op:%d\n",(const char *) data, key, op);
  if (op != SC68_DIAL_CALL)
    return 1;
  if (!strcmp(key,SC68_DIAL_WAIT))
    v->i = 1;
  else if (!strcmp(key,SC68_DIAL_HELLO))
    v->s = "config";
  else if (!strcmp(key,"instance")) {
    HINSTANCE hinst = core_api::get_my_instance();
    const char * fn = core_api::get_my_file_name();
    DBG("hinst=%08x \"%s\"\n", (unsigned) hinst, fn);
    v->s = (const char *)(HINSTANCE) hinst;
  } else if (!strcmp(key,"parent"))
    v->s = (const char *) (HWND) core_api::get_main_window();
  else
    return 1;                           /* continue */
  return 0;                             /* taken */
}

  void context_command(unsigned p_index,metadb_handle_list_cref p_data,const GUID& p_caller) {
    if (p_index == 0) {
      DBG("%s\n","OPEN DIALOG CONFIG DIALOG");
      sc68_cntl(0,SC68_DIAL, 0, conf_f);
    }
  }

  GUID get_parent() { return guid_cmenu; }
};

static contextmenu_item_factory_t<cmenu_asid_item> g_asid_factory;
static contextmenu_item_factory_t<cmenu_engine_item> g_engine_factory;
static contextmenu_item_factory_t<cmenu_config_item> g_config_factory;
