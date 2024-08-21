/*
 * @file    in_sc68.c
 * @brief   sc68-ng plugin for winamp 5.5 - main
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

/* generated config header */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* winamp sc68 declarations */
#include "wasc68.h"

/* libc */
#include <stdio.h>
#include <ctype.h>
#include <assert.h>

/* sc68 */
#include <sc68/sc68.h>
#include <sc68/file68.h>
#include <sc68/file68_features.h>
#include <sc68/file68_str.h>
#include <sc68/file68_msg.h>
#include <sc68/file68_opt.h>

/* windows */
#include <windows.h>

/* winamp 2 */
#include "winamp/in2.h"
//#undef   _MSC_VER                       /* fix intptr_t redefinition */
//#define  _MSC_VER 2000
#include "winamp/wa_ipc.h"
#include "winamp/ipc_pe.h"
#include <strsafe.h>
#define WA_UTILS_SIMPLE
#include <loader/loader/utils.h>

#ifdef __cplusplus
/* winamp 3 */
#include "api/syscb/api_syscb.h"
#include "api/syscb/callbacks/syscb.h"
#include "api/service/waservicefactory.h"
#include "api/service/services.h"
#include "api/service/api_service.h"
#include "api/memmgr/api_memmgr.h"
#include "../Agave/Language/api_language.h"
#define WITH_API_SERVICE
static api_service * g_service;
#endif

/* Fake command line for sc68_init() */
static char appname[] = "winamp";
static char *argv[] = { appname };

/*****************************************************************************
 * Plugin private data.
 ****************************************************************************/

#if 0
int g_usehook = -1;
int g_useufi = -1;
#endif

#ifdef USE_LOCK
static HANDLE        g_lock;       /* mutex handle           */
#endif
static char          g_magic[8] = "wasc68!";

static WNDPROC       g_mwhkproc;   /* main window chain proc */
static HANDLE        g_thdl;       /* thread handle          */
static char        * g_uri;        /* allocated URI          */
static sc68_t      * g_sc68;       /* sc68 emulator instance */
static int           g_code;       /* sc68 process code      */
static int           g_spr;        /* sampling rate in hz    */
static int           g_maxlatency; /* max latency in ms      */
static int           g_trackpos;   /* track position in ms   */
static int           g_allin1;     /* play all tracks as one */
static int           g_track;      /* current playing track  */
static int           g_tracks;     /* number of tracks       */

static volatile LONG g_playing;    /* true while playing     */
static volatile LONG g_stopreq;    /* stop requested         */
static volatile LONG g_paused;     /* pause status           */
static volatile LONG g_settrack;   /* request change track   */

//static BYTE          g_spl[576*8]; /* Sample buffer          */

/*****************************************************************************
 * Declaration
 ****************************************************************************/

/* The decode thread */
static DWORD WINAPI playloop(LPVOID b);
static void GetFileExtensions(void);
static int init(void);
static void quit(void);
static void config(HWND);
static void about(HWND);
static  int infobox(const in_char *, HWND);
static  int isourfile(const in_char *);
static void pause(void);
static void unpause(void);
static  int ispaused(void);
static  int getlength(void);
static  int getoutputtime(void);
static void setoutputtime(const int);
static void setvolume(const int);
static void setpan(const int);
static  int play(const in_char *);
static void stop(void);
static void getfileinfo(const in_char *, in_char *, int *);

EXTERN int fileinfo_dialog(HINSTANCE hinst, HWND hwnd, const char * uri);
EXTERN int config_dialog(HINSTANCE hinst, HWND hwnd);

/*****************************************************************************
 * Debug
 ****************************************************************************/
int wasc68_cat = msg68_NEVER;

/*****************************************************************************
 * LOCKS
 ****************************************************************************/
#ifdef USE_LOCK

static inline int lock(void)
{ return WaitForSingleObject(g_lock, INFINITE) == WAIT_OBJECT_0; }

static inline int lock_noblock(void)
{ return WaitForSingleObject(g_lock, 0) == WAIT_OBJECT_0; }

static inline void unlock(void)
{ ReleaseMutex(g_lock); }

#else

static inline int lock(void) { return 1; }
static inline int lock_noblock(void) { return 1; }
static inline void unlock(void) { }

#endif

static inline LONG atomic_set(LONG volatile * ptr, LONG v)
{ return InterlockedExchange(ptr,v); }

static inline LONG atomic_get(LONG volatile * ptr)
{ return *ptr; }

sc68_t * sc68_lock(void) {
  return lock() ? g_sc68 : 0;
}

void sc68_unlock(sc68_t * sc68) {
  if (sc68 == g_sc68 && g_sc68)
    unlock();
}


/* static */
/*****************************************************************************
 * THE INPUT MODULE
 ****************************************************************************/
In_Module plugin =
{
  IN_VER_WACUP,               /* Input plugin version as defined in in2.h */
  (char*)
  "sc68 (Atari ST & Amiga music) v" PACKAGE_VERSION, /* Description */
  0,                          /* hMainWindow (filled in by winamp)  */
  0,                          /* hDllInstance (filled in by winamp) */
  NULL,                       /* filled in by GetFileExtensions later */
  0,                          /* is_seekable */ // TODO
  1,                          /* uses output plug-in system */

  config,
  about,
  init,
  quit,
  getfileinfo,
  0/*infobox*/,
  isourfile,
  play,
  pause,
  unpause,
  ispaused,
  stop,

  getlength,
  getoutputtime,
  setoutputtime,

  setvolume,
  setpan,

  IN_INIT_VIS_RELATED_CALLS,
  0,0,                   /* dsp calls filled in by winamp */
  //NULL,                  /* set equalizer */
  IN_INIT_WACUP_EQSET_EMPTY
  NULL,                  /* setinfo call filled in by winamp */
  0,                     /* out_mod filled in by winamp */
  NULL,	// api_service
  INPUT_HAS_READ_META | INPUT_USES_UNIFIED_ALT3 |
  INPUT_HAS_FORMAT_CONVERSION_LEGACY/* |
  INPUT_HAS_FORMAT_CONVERSION_SET_TIME_MODE*/,
  GetFileExtensions,	// loading optimisation
  IN_INIT_WACUP_END_STRUCT
};

static void GetFileExtensions(void)
{
    static int loaded_extensions;
    if (!loaded_extensions)
    {
        loaded_extensions = 1;

        // TODO localise
        plugin.FileExtensions = (char*)L"SC68;SC68.GZ\0sc68 File (*.SC68;*.SC68.GZ)\0"
                                              L"SND;SNDH\0sndh File (*.SND;*.SNDH)\0";
    }
}

/*****************************************************************************
 * Message Hook
 ****************************************************************************/

#if 0
/* Process winamp main window messages */
static LRESULT mwproc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  const int isplaying = atomic_get(&g_playing);
  int step = 0;                         /* sub song step */

  switch (Msg) {

  case WM_SYSCOMMAND:
    DBG("WM_SYSCOMMAND #%d w=%d l=%d\n",
        (int)LOWORD(wParam), (int)wParam, (int)lParam);

  case WM_COMMAND:
    if (isplaying)
      switch (LOWORD(wParam)) {
        /* Hook prev/next track messages
         *
         *   When a playing multi-song sc68 file prev/next track button
         *   will change the subsong instead of adjusting the playlist
         *   entry. However pressing shift key brings back original
         *   winamp befavior.
         */
      case 40061:                       /* shift + prev track */
        wParam = MAKEWPARAM(40044,HIWORD(wParam));
        break;
      case 40044:                       /* prev track */
        step = -1;
        break;
      case 40060:                       /* shift + next track */
        wParam = MAKEWPARAM(40048, HIWORD(wParam));
        break;
      case 40048:                       /* next track */
        step = 1;
        break;
      default:
        DBG("WM_COMMAND #%d w=%d l=%d\n",
            (int)LOWORD(wParam), (int)wParam, (int)lParam);
      }
    break;
  }

  if (step) {
    int newtrack = g_track + step;
    if (g_allin1 && newtrack > 0 && newtrack <= g_tracks) {
      DBG("request new track -- %02d\n", newtrack);
      atomic_set(&g_settrack, newtrack);
      return TRUE;
    }
  }

  return g_mwhkproc
    ? CallWindowProc(g_mwhkproc, hWnd, Msg, wParam, lParam)
    : DefWindowProc(hWnd, Msg, wParam, lParam)
    ;
}

static
LRESULT CALLBACK myproc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  if (hWnd == plugin.hMainWindow)
    return mwproc(hWnd, Msg, wParam, lParam);
  else {
    assert(!"unexpected window handler in my hook");
    return DefWindowProc(hWnd, Msg, wParam, lParam);
  }
}

static void mw_hook(void)
{
  if (g_usehook) {
    if (!g_mwhkproc) {
      const HWND hwnd = plugin.hMainWindow;
      char name[512];
      if (GetClassName(hwnd, name, sizeof(name)) <= 0)
        strcpy(name,"<no-name>");
      g_mwhkproc = (WNDPROC)
        SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)myproc);
      DBG("hook winamp main window #%08x \"%s\" -- %s (%08x)\n",
          (unsigned) hwnd, name,
          !g_mwhkproc?"failed":"success", (unsigned)g_mwhkproc);
    }
  } else
    DBG("hook disabled\n");
}

static void mw_unhook(void)
{
  if (!g_mwhkproc)
    DBG("winamp main window not hooked\n");
  else {
    DBG("unhook winamp main window\n");
    SetWindowLongPtr(plugin.hMainWindow, GWLP_WNDPROC, (LONG_PTR)g_mwhkproc);
    g_mwhkproc = 0;
  }
}
#endif

static
/*****************************************************************************
 * CONFIG DIALOG
 ****************************************************************************/
void config(HWND hwnd)
{
  create_sc68();

  if (!config_dialog(DLGHINST, hwnd)) {
    // only save out if there's changes
    lock();
    sc68_cntl(0, SC68_CONFIG_SAVE);
    unlock();
  }
}

static
/*****************************************************************************
 * ABOUT DIALOG
 ****************************************************************************/
void about(HWND hwnd)
{
  wchar_t message[512] = { 0 };
  _snwprintf(message, ARRAYSIZE(message),
             L"%s\n\nsc68 (Atari ST & Amiga) player built\nusing "
             L"%hs & %hs\n© 1998-2016 Benjamin Gerard\n"
#ifdef DEBUG
             L"\n" " !!! DEBUG Build !!! "
#endif
             L"\nWACUP related modifications by\n%s (%s)\n\n"
             L"Build date: %hs", (wchar_t*)plugin.description,
             sc68_versionstr(), file68_versionstr(),
             WACUP_Author(), WACUP_Copyright(), __DATE__);

  AboutMessageBox(hwnd, message, L"sc68 (Atari ST & Amiga) Player");
}

/*static
/*****************************************************************************
 * INFO DIALOG
 ****************************************************************************/
/*int infobox(const in_char *uri, HWND hwnd)
{
#if 0
  fileinfo_dialog(DLGHINST, hwnd, uri);
#endif
  return INFOBOX_UNCHANGED;
}*/

static
/*****************************************************************************
 * FILE DETECTION
 ****************************************************************************/
int isourfile(const in_char * file)
{
  if (file && *file && !IsPathURLA(file)) {
    // to save doing some of the things with making
    // the plug-in fully unicode for now this'll do
    // just enough to get a valid extension to use
    const wchar_t *uri = (LPCWSTR)file;
    if (uri && *uri && SameStrN(uri,L"sc68:",5)) {
      return 1;
    }
  }
  return 0;
}

/*****************************************************************************
 * PAUSE
 ****************************************************************************/
static void pause(void) {
  atomic_set(&g_paused,1);
  plugin.outMod->Pause(1);
}

static void unpause(void) {
  atomic_set(&g_paused,0);
  plugin.outMod->Pause(0);
}

static int ispaused(void) {
  return atomic_get(&g_paused);
}

static
/*****************************************************************************
 * GET LENGTH (MS)
 ****************************************************************************/
int getlength(void)
{
  int ms = 0;
  if (lock()) {
    const int fct = g_allin1 ? SC68_GET_DSKLEN : SC68_GET_LEN;
    int res = sc68_cntl(g_sc68, fct);
    if (ms != -1)
      ms = res;
    unlock();
  }
  return ms;
}

static
/*****************************************************************************
 * GET CURRENT POSITION (MS)
 ****************************************************************************/
int getoutputtime(void)
{
  int ms = 0;
  if (plugin.outMod) {
    if (lock()) {
      ms = g_trackpos + plugin.outMod->GetOutputTime();
      unlock();
    }
  }
  return ms;
}

static
/*****************************************************************************
 * SET CURRENT POSITION (MS)
 ****************************************************************************/
void setoutputtime(const int ms)
{
/* Not supported ATM.
 * It has to signal the play thread it has to seek.
 */
  // SC68_SET_POS seems to be what'd be needed but
  // there's no sign of that being implemented :'(
}

static
/*****************************************************************************
 * SET VOLUME
 ****************************************************************************/
void setvolume(const int volume)
{
  plugin.outMod->SetVolume(volume);
}

static
/*****************************************************************************
 * SET PAN
 ****************************************************************************/
void setpan(const int pan)
{
  plugin.outMod->SetPan(pan);
}

static void clean_close(void)
{
  if (g_thdl) {
    TerminateThread(g_thdl,1);
    CloseHandle(g_thdl);
    g_thdl = 0;
    DBG("%s\n","thread cleaned");
  }
  atomic_set(&g_playing,0);
#if 0
  mw_unhook();
#endif
  if (g_sc68) {
    sc68_destroy(g_sc68);
    g_sc68 = 0;
  }
  if (g_uri) {
    free(g_uri);
    g_uri = 0;
  }
  /* Close output system. */
  if (plugin.outMod && plugin.outMod->Close)
  {
      plugin.outMod->Close();
  }
  /* Deinitialize visualization. */
  plugin.SAVSADeInit();
}

static
/*****************************************************************************
 * STOP
 ****************************************************************************/
void stop(void)
{
  if (lock()) {
    atomic_set(&g_stopreq,1);
    if (g_thdl) {
      switch (WaitForSingleObjectEx(g_thdl,10000,TRUE)) {
      case WAIT_OBJECT_0:
        CloseHandle(g_thdl);
        g_thdl = 0;
        break;
      default:
        DBG("thread did not exit normally\n");
      }
    }
    clean_close();
    unlock();
  }
}

int extract_track_from_uri(const char *uri, char **filename)
{
  if (filename) {
    *filename = ((uri && *uri) ? _strdup(uri) : NULL);
    if (*filename) {
      char* comma = strrchr(*filename, ',');
      if (comma) {
        *comma = 0;
        return AStr2I((comma + 1));
      }
    }
  }
  return 0;
}

static
/*****************************************************************************
 * PLAY
 *
 * @retval  0 on success
 * @reval  -1 on file not found
 * @retval !0 stopping winamp error
 ****************************************************************************/
int play(const in_char *fn)
{
  int err = 1;

  if (!fn || !*fn)
    return -1;

  create_sc68();

  if (!lock_noblock())
    goto cantlock;

  /* Safety net */
  if (g_sc68 || g_thdl || g_uri)
    goto inused;

  /* cleanup */
  g_mwhkproc   = 0;
  g_maxlatency = 0;
  g_trackpos   = 0;
  g_track      = 0;
  g_stopreq    = 0;
  g_paused     = 0;
  g_settrack   = 0;

  /* Create sc68 emulator instance */
  if (g_sc68 = sc68_create(0), !g_sc68)
    goto exit;

  char* filename = 0;
  char uri[MAX_PATH] = { 0 };
  ConvertUnicodeFn(uri, ARRAYSIZE(uri), (wchar_t*)fn, CP_ACP);
  const int settrack = extract_track_from_uri(uri, &filename);
  if (settrack) {
    DBG("got specific track -- %d\n", settrack);
  }

  /* Duplicate URI an */
  // wacup change so it uses what's been
  // created when trying to extract the
  // current track number for the file
  if (g_uri = filename/*/uri/**/, !g_uri)
    goto exit;

  /* Load */
  if (sc68_load_uri(g_sc68, g_uri)) {
    err = -1;                           /* File not found or such */
    goto exit;
  }

  /* Get sampling rate */
  g_spr = sc68_cntl(g_sc68, SC68_GET_SPR);
  if (g_spr <= 0)
    goto exit;

  /* Get track count */
  g_tracks =  sc68_cntl(g_sc68, SC68_GET_TRACKS);
  if (g_tracks <= 0)
    goto exit;
  DBG("tracks=%d\n",g_tracks);

  /* Only mode supported ATM */
  g_allin1 = !settrack;
  DBG("all-in-1: %d\n", g_allin1);

  /* Get disk and track info */
  if (sc68_play(g_sc68, settrack ? settrack : 1, SC68_DEF_LOOP) < 0)
    goto exit;

  /* Run music init code. Ensure everything is okay to really play */
  g_code = sc68_process(g_sc68, 0, 0);
  if (g_code == SC68_ERROR)
    goto exit;

  /* Get current track */
  g_track = sc68_cntl(g_sc68, SC68_GET_TRACK);

  /* Init output module */
  g_maxlatency = (plugin.outMod->Open ? plugin.outMod->Open(g_spr, 2, 16, -1, -1) : -1);
  if (g_maxlatency < 0)
    goto exit;

  /* set default volume */
  plugin.outMod->SetVolume(-666);

  /* Init info and visualization stuff */
  plugin.SetInfo((g_spr * 2 * 16) / 1000, g_spr / 1000, 2, 1);
  plugin.SAVSAInit(g_maxlatency, g_spr);
  plugin.VSASetInfo(g_spr, 2);


  /* Init play thread */
  g_thdl = StartThread(playloop, g_magic, /*plugin.config->
					   GetInt(playbackConfigGroupGUID, L"priority",
						  */THREAD_PRIORITY_HIGHEST/*)*/, 0, NULL);

  if (err = !g_thdl, !err) {
    atomic_set(&g_playing,1);
#if 0
    mw_hook();
#endif
  }
exit:
  if (err)
    clean_close();

inused:
  unlock();

cantlock:
  return err;
}

static
const char * get_tag(const sc68_cinfo_t * const cinfo, const char * const key)
{
  int i;
  for (i=0; i<cinfo->tags; ++i)
    if (!strcmp68(cinfo->tag[i].key, key))
      return cinfo->tag[i].val;
  return 0;
}

static void xfinfo(char *title, int *msptr, sc68_t *sc68, sc68_disk_t disk)
{
  const int max = GETFILEINFO_TITLE_LENGTH;
  sc68_music_info_t tmpmi, * const mi = &tmpmi;

  if (sc68_music_info(sc68, mi,
                      sc68 ? SC68_CUR_TRACK : SC68_DEF_TRACK, disk))
    return;

  if (title) {
    const char * artist = get_tag(&mi->dsk, "aka");
    if (!artist)
      artist = mi->artist;

    if (mi->tracks == 1 /* || !strcmp68(mi->title, mi->album) */)
      snprintf(title, max, "%s - %s",artist, mi->album);
    else
      snprintf(title, max, "%s - %s [%d tracks]",
               artist, mi->album, mi->tracks);
  }
  if (msptr)
    *msptr = mi->dsk.time_ms;
}


static
/*****************************************************************************
 * GET FILE INFO
 ****************************************************************************/
/*
 * this is an odd function. It is used to get the title and/or length
 * of a track.  If filename is either NULL or of length 0, it means
 * you should return the info of lastfn. Otherwise, return the
 * information for the file in filename.  if title is NULL, no title
 * is copied into it.  if msptr is NULL, no length is copied
 * into it.
 ****************************************************************************/
void getfileinfo(const in_char * uri, in_char * title, int * msptr)
{
  if (title)
    *title = 0;
  if (msptr)
    *msptr = 0;

  create_sc68();

  if (!uri || !*uri) {
    /* current disk */
    if (lock()) {
      if (g_sc68)
        xfinfo(title, msptr, g_sc68, 0);
      unlock();
    }
  } else {
    /* some other disk */
    sc68_disk_t disk;
    if (disk = wasc68_cache_get(uri), disk) {
      xfinfo(title, msptr, 0, disk);
      wasc68_cache_release(disk, 0);
    }
  }
}

static
/*****************************************************************************
 * LOOP
 ****************************************************************************/
DWORD WINAPI playloop(LPVOID cookie)
{
  BYTE spl[576 * 8] = { 0 };
  for (;;) {
    int settrack, n = 0, canwrite;

    // TODO would be nice to find a way
    //      to support SC68_SET_POS but
    //      it very likely will require
    //      brute forcing a fake 'seek'

    if (atomic_get(&g_stopreq)) {
      DBG("stop request detected\n");
      break;
    }
    settrack = atomic_set(&g_settrack,0);
    canwrite = plugin.outMod->CanWrite();

    if (settrack) {
      DBG("change track has been requested -- %02d\n", settrack);
      n = 0;
      sc68_play(g_sc68, settrack, SC68_DEF_LOOP);
      g_code = sc68_process(g_sc68, 0, 0);
      DBG("code=%x\n",g_code);
    } else if (canwrite >= (576 << (2+!!plugin.dsp_isactive()))) {
      n = 576;
      g_code = sc68_process(g_sc68, spl, &n);
    } else {
      Sleep(20);                        /* wait a while */
      continue;
    }

    /* Exit on error or legit end */
    if (g_code & SC68_END) {
      DBG("SC68_END detected -- %x\n", g_code);
      break;
    }
    /* Change track detected */
    if (g_code & SC68_CHANGE) {
      DBG("SC68_CHANGE detected -- %x\n", g_code);
      if (!g_allin1) {
        DBG("Not all in 1, exit\n");
        g_code |= SC68_END;
        break;
      }
      g_track = sc68_cntl(g_sc68,SC68_GET_TRACK);
      DBG("change track -- %02d\n", g_track);
    }
    if (settrack) {
      g_trackpos = sc68_cntl(g_sc68, SC68_GET_ORG);
      DBG("flushing audio track pos: -- %d \n", g_trackpos);
      plugin.outMod->Flush(0);
    }

    /* if (g_code & SC68_CHANGE) { */
    /*   PostMessage(plugin.hMainWindow,WM_WA_IPC,0,IPC_UPDTITLE); */
    /* } */

    /* Send audio data to output mod with optionnal DSP processing if
     * it is requested. */
    if (n > 0) {
      int l;
      int vispos = g_trackpos + plugin.outMod->GetOutputTime();

      /* Give the samples to the vis subsystems */
      plugin.SAAddPCMData (spl, 2, 16, vispos);
      /*plugin.VSAAddPCMData(spl, 2, 16, vispos);*/

      /* If we have a DSP plug-in, then call it on our samples */
      l = (
        plugin.dsp_isactive()
        ? plugin.dsp_dosamples((short *)spl, n, 16, 2, g_spr)
        : n ) << 2;

      /* Write the pcm data to the output system */
      plugin.outMod->Write((char*)spl, l);
    }
  }
  atomic_set(&g_playing,0);

  /* Wait buffered output to be processed */
  while (!atomic_get(&g_stopreq)) {
    plugin.outMod->CanWrite();           /* needed by some out mod */
    if (!plugin.outMod->IsPlaying()) {
      /* Done playing: tell Winamp and quit the thread */
      /*PostMessage(plugin.hMainWindow, WM_WA_MPEG_EOF, 0, 0);/*/
      PostEOF(FALSE);/**/
      break;
    } else {
      Sleep(15);              // give a little CPU time back to the system.
    }
  }

  if (g_thdl)
  {
    CloseHandle(g_thdl);
    g_thdl = 0;
  }
  DBG("exit with code -- %x\n", g_code);
  return 0;
}

#if 0
static int onchange_ufi(const option68_t * opt, value68_t * val)
{
  g_useufi = !!val->num;
  return 0;
}

static int onchange_hook(const option68_t * opt, value68_t * val)
{
  g_usehook = !!val->num;
  return 0;
}
#endif

static
/*****************************************************************************
 * PLUGIN INIT
 ****************************************************************************/
int init(void)
{
  // TODO setup localisation / plug-in names, etc
  plugin.description = (char*)L"sc68 (Atari ST & Amiga) Player v" TEXT(PACKAGE_VERSION);
  return IN_INIT_SUCCESS;
}

// instead of doing everything in init(..) it's
// better especially when there's no certainty
// of what format(s) a user might be playing to
// only do the initialisation of things when it
// is actually needed
void create_sc68(void)
{
  /* sc68 init */
#if 0
  static option68_t opts[] = {
    OPT68_BOOL(0,"ufi", "winamp","Unified file info dialog",1,onchange_ufi),
    OPT68_BOOL(0,"hook","winamp","Hook prev/next track",    1,onchange_hook)
  };
#endif

  static sc68_init_t init68;
  if (!init68.argv) {
#ifndef NDEBUG
    const int debug =
#ifdef DEBUG
      1
#else
      0
#endif
      ;
#endif

    //memset(&init68,0,sizeof(init68));
    init68.argv = argv;
    init68.argc = sizeof(argv) / sizeof(*argv);
#ifndef NDEBUG
    wasc68_cat = msg68_cat("winamp", "winamp input plugin", debug);
    init68.debug_set_mask = debug << wasc68_cat;
    init68.debug_clr_mask = 0;
    init68.msg_handler = (sc68_msg_t) msgfct;
#else
    wasc68_cat = msg68_NEVER;
    init68.debug_clr_mask = -1;
#endif
    //init68.flags.no_load_config = 1;      /* disable config load */
    sc68_init(&init68);

    /* sc68 winamp init */
#if 0
    option68_append(opts, sizeof(opts)/sizeof(*opts));
    option68_iset(option68_get("ufi",opt68_ALWAYS),0,opt68_ALWAYS,opt68_CFG);
    option68_iset(option68_get("hook",opt68_ALWAYS),1,opt68_ALWAYS,opt68_CFG);
    sc68_cntl(0,SC68_CONFIG_LOAD);
#endif

    /* clear and init private */
#ifdef USE_LOCK
    g_lock = CreateMutex(NULL, FALSE, NULL);
#endif

    /* clear and init cache */
    wasc68_cache_init();

    /* Hook messages */
    /* mw_hook(); */

#ifdef WITH_API_SERVICE
    /* Get WASABI service */
    if (!g_service) {
      g_service = (api_service *)
        SendMessage(plugin.hMainWindow,WM_WA_IPC,0,IPC_GET_API_SERVICE);
    if (g_service == (api_service *)1)
      g_service = 0;
    }

    static int fcc[] = {
      WaSvc::NONE,
      WaSvc::UNIQUE,
      WaSvc::OBJECT,
      WaSvc::CONTEXTCMD,
      WaSvc::DEVICE,
      WaSvc::FILEREADER,
      WaSvc::FILESELECTOR,
      WaSvc::STORAGEVOLENUM,
      WaSvc::IMAGEGENERATOR,
      WaSvc::IMAGELOADER,
      WaSvc::IMAGEWRITER,
      WaSvc::ITEMMANAGER,
      WaSvc::PLAYLISTREADER,
      WaSvc::PLAYLISTWRITER,
      WaSvc::MEDIACONVERTER,
      WaSvc::MEDIACORE,
      WaSvc::MEDIARECORDER,
      WaSvc::SCRIPTOBJECT,
      WaSvc::WINDOWCREATE,
      WaSvc::XMLPROVIDER,
      WaSvc::DB,
      WaSvc::SKINFILTER,
      WaSvc::METADATA,
      WaSvc::METATAG,
      WaSvc::EVALUATOR,
      WaSvc::MINIBROWSER,
      WaSvc::TOOLTIPSRENDERER,
      WaSvc::XUIOBJECT,
      WaSvc::STRINGCONVERTER,
      WaSvc::ACTION,
      WaSvc::COREADMIN,
      WaSvc::DROPTARGET,
      WaSvc::OBJECTDIR,
      WaSvc::TEXTFEED,
      WaSvc::ACCESSIBILITY,
      WaSvc::ACCESSIBILITYROLESERVER,
      WaSvc::EXPORTER,
      WaSvc::COLLECTION,
      WaSvc::REDIRECT,
      WaSvc::FONTRENDER,
      WaSvc::SRCCLASSFACTORY,
      WaSvc::SRCEDITOR,
      WaSvc::MP4AUDIODECODER,
      WaSvc::PLAYLISTREADER_WA5,
      WaSvc::PLAYLISTWRITER_WA5,
      WaSvc::PLAYLISTHANDLER,
      WaSvc::TAGPROVIDER,
      WaSvc::NSVFACTORY,
      -1
    };

    if (g_service) {
      int i, n, j;
      waServiceFactory * s;
      DBG("SVC api <%p>\n", g_service);

      for (j=0; fcc[j] != -1; ++j) {
        char cc[5];
        cc[0] = fcc[j]>>24; cc[1] = fcc[j]>>16;
        cc[2] = fcc[j]>> 8; cc[3] = fcc[j]; cc[4] = 0;

        n = g_service->service_getNumServices(fcc[j]);
        DBG("[%s] got %d service(s)\n", cc, n);
        for (s = g_service->service_enumService(fcc[j], i=0);
             s;
             s = g_service->service_enumService(fcc[j], ++i)) {
          DBG("[%s] #%02d SVC factory  '%s'\n",
              cc, i, s ? s->getServiceName() : "(nil)");
        }
      }

      s = g_service->service_getServiceByGuid(languageApiGUID);
      if (s) {
        DBG("service factory lang %p '%s'\n",
            s, s?s->getServiceName():"(nil)");
      } else {
        DBG("don't have service factory lang\n");
      }

      s = g_service->service_getServiceByGuid(memMgrApiServiceGuid);
      if (s) {
        DBG("service factory memman %p '%s'\n",
            s, s?s->getServiceName():"(nil)");
      } else {
        DBG("don't have service memman lang\n");
      }
      //if (sf) WASABI_API_LNG = reinterpret_cast<api_language*>(sf->getInterface());
    }
#endif
    DBG("init completed\n");
  }
}

static
/*****************************************************************************
 * PLUGIN SHUTDOWN
 ****************************************************************************/
void quit(void)
{
  DBG("\n");
#if 0
  mw_unhook();
#endif
  // no need to be doing this as closing
  // the config dialog will trigger it &
  // avoids generating a file unless its
  // actually needed (e.g. load + close)
  /*lock();
  sc68_cntl(0,SC68_CONFIG_SAVE);
  unlock();*/
#ifdef USE_LOCK
  CloseHandle(g_lock);
  g_lock = 0;
#endif
  wasc68_cache_kill();
  msg68_cat_free(wasc68_cat);
  wasc68_cat = msg68_NEVER;
  sc68_shutdown();
}

static int xinfo(const char *data, char *dest, size_t destlen,
                 sc68_t * sc68, sc68_disk_t disk, int track)
{
  sc68_music_info_t tmpmi, * const mi = &tmpmi;
  const char * value = 0;
  if (sc68_music_info(sc68, mi, track>0 ? track : SC68_DEF_TRACK, disk)) {
  }
  else if (!strcasecmp(data,"album")) { /* Album name */
    value = mi->album;
  }
  else if (!strcasecmp(data,"title")) { /* Song title */
    value = !track ? mi->album : mi->title;

    /* if (!track) { */
    /*   if (mi->tracks > 1) { */
    /*     snprintf(dest, destlen, "%s [%02d]",mi->album, mi->tracks); */
    /*     value = dest; */
    /*   } else { */
    /*     value = mi->album; */
    /*   } */
    /* } else if (!strcmp68(mi->album,mi->title)) { */
    /*   snprintf(dest, destlen, "%s #%02", mi->album, mi->trk.track); */
    /*   value = dest; */
    /* } else { */
    /*   value = mi->title; */
    /* } */
  }
  else if (!strcasecmp(data,"artist")) { /* Song artist */
    value = get_tag(&mi->trk,"aka");
    if (!value)
      value = mi->artist;
  }
  else if (!strcasecmp(data,"track")) {
    if (track == mi->trk.track) {
      snprintf(dest, destlen, "%d", track);
      value = dest;
    }
  }
  /* else if (!strcasecmp(data,"disc")) { */
  /*   snprintf(dest, destlen, "%02d", mi->tracks); */
  /*   value = dest; */
  /* } */
  else if (!strcasecmp(data,"albumartist")) {
    value = get_tag(&mi->dsk,"aka");
    if (!value) value = get_tag(&mi->dsk,"artist");
    if (!value) value = mi->artist;
  }
  else if (!strcasecmp(data,"composer")) {
    value = get_tag(&mi->trk,"original");
    if (!value) value = get_tag(&mi->trk,"composer");
    if (!value) value = get_tag(&mi->dsk,"original");
    if (!value) value = get_tag(&mi->dsk,"composer");
  } else if (!strcasecmp(data,"genre")) {
    value = mi->genre;
  }
  else if (!strcasecmp(data,"length")) {
    /* length in ms */
    if (track == mi->trk.track) {
      snprintf(dest, destlen, "%u", mi->trk.time_ms);
    } else {
      snprintf(dest, destlen, "%u", mi->dsk.time_ms);
    }
    value = dest;
  }
  else if (!strcasecmp(data,"year")) {
    value = get_tag(&mi->dsk,"year");
    if (!value)
      value = get_tag(&mi->trk,"year");
  }
  else if (!strcasecmp(data,"publisher")) {
    value = get_tag(&mi->trk,"converter");
    if (!value) value = get_tag(&mi->dsk,"converter");
    value = get_tag(&mi->trk,"converter");
  }
  else if (!strcasecmp(data, "comment")) {
    value = get_tag(&mi->trk,"comment");
    if(!value)
    value = get_tag(&mi->dsk,"comment");
  }
  else if (!strcasecmp(data, "samplerate")) {
    value = I2AStr(sc68_cntl(g_sc68, SC68_GET_SPR), dest, destlen);
  }
  else if (!strcasecmp(data, "bitrate")) {
    const int br = (sc68_cntl(g_sc68, SC68_GET_SPR) * 2 * 16);
    if (br > 0) {
      value = I2AStr((br / 1000), dest, destlen);
    }
  }
  else if (!strcasecmp(data, "formatinformation")) {
    // TODO localise
    StringCchPrintf(dest, destlen, "Length: %u seconds\nSamplerate: %d Hz\n"
                    "Loop count: %d\n# of tracks: %d", ((track == mi->trk.track) ?
                    mi->trk.time_ms : mi->dsk.time_ms) / 1000, sc68_cntl(g_sc68,
                    SC68_GET_SPR), sc68_cntl(g_sc68, SC68_GET_LOOPS),
                    sc68_cntl(g_sc68, SC68_GET_TRACKS));
    value = dest;
    //value = I2AStr(sc68_cntl(g_sc68, SC68_GET_SPR), dest, destlen);
  }  
  /*else if (!strcasecmp(data,"")) {
    DBG("unhandled TAG '%s'\n", data);
  }*/

  if (!value)
    return 0;

  if (value != dest)
    strncpy(dest, value, destlen);
  dest[destlen-1] = 0;
  return 1;
}


/**
 * Provides the extended meta tag support of winamp.
 *
 * @param  uri   URI or file to get info from (0 or empty for playing)
 * @param  data  Tag name
 * @param  dest  Buffer for tag value
 * @param  max   Size of dest buffer
 *
 * @retval 1 tag handled
 * @retval 0 unsupported tag
 */
EXPORT
int winampGetExtendedFileInfo(const char *uri, const char *data,
                              char *dest, size_t max)
{
  if (SameStrA(data, "type") ||
      SameStrA(data, "lossless") ||
      SameStrA(data, "streammetadata"))
  {
    dest[0] = L'0';
    dest[1] = L'\0';
    return 1;
  }
  else if (SameStrA(data, "streamgenre") ||
           SameStrA(data, "streamtype") ||
           SameStrA(data, "streamurl") ||
           SameStrA(data, "streamname") ||
           SameStrA(data, "reset"))
  {
    return 0;
  }
  else if (SameStrA(data, "family"))
  {
    if (uri && *uri && SameStrA(uri, ".gz"))
    {
      strncpy(dest, "sc68 (Compressed) Audio File", max);
    }
    else
    {
      strncpy(dest, "sc68 Audio File", max);
    }
    return 1;
  }

  if (!uri || !uri[0])
  {
    return 0;
  }

  create_sc68();

  sc68_disk_t disk;
  int res = 0;

  if (data && *data && dest && max > 2) {
    /*if (!uri || !*uri) {
      if (lock()) {
        res = xinfo(data, dest, max, g_sc68, 0, g_track);
        unlock();
      }
    } else*/ {
      char * filename = 0;
      int settrack = extract_track_from_uri(uri, &filename);
      if (disk = wasc68_cache_get(filename/*/uri/**/), disk) {
        res = xinfo(data, dest, max, 0, disk, settrack);
        wasc68_cache_release(disk, 0);
      }
      if (filename)
        free(filename);
    }
  }
  return res;
}

struct transcon {
    sc68_t* sc68;                        /* sc68 instance               */
    int done;                             /* 0:not done, 1:done -1:error */
    int allin1;                           /* 1:all tracks at once        */
    size_t pcm;                           /* pcm counter                 */
};

EXPORT
int GetSubSongInfo(const wchar_t* filename) {
  int tracks = 0;
  struct transcon* trc = (struct transcon* )malloc(sizeof(struct transcon));
  if (trc) {
    trc->pcm = 0;
    trc->allin1 = 0;
    trc->sc68 = sc68_create(0);
    if (trc->sc68) {
      char uri[MAX_PATH] = { 0 };
      ConvertUnicodeFn(uri, ARRAYSIZE(uri), filename, CP_ACP);
      if (!sc68_load_uri(trc->sc68, uri)) {
        if (tracks = sc68_cntl(trc->sc68, SC68_GET_TRACKS), tracks <= 0) {
          tracks = 0;
        }
      }
      sc68_destroy(trc->sc68);
    }
    free(trc);
  }
  return tracks;
}

#if 0
/**
 * Provides fake support for writing tag to prevent winamp unified
 * file info to complain.
 */
EXPORT
int winampSetExtendedFileInfo(const char *fn, const char *data, char *val)
{
  DBG("\"%s\" [%s] = \"%s\"\n", fn, data, val);
  return 1;
}

/**
 * Provides writing tags to prevent winamp unified file info to
 * complain.
 */
EXPORT
int winampWriteExtendedFileInfo()
{
  DBG("fake success\n");
  return 1;
}
#endif

int set_asid(int asid)
{
  int res = -1;
  if (lock()) {
    res = sc68_cntl(g_sc68,SC68_SET_ASID,asid);
    if (g_sc68)
      res = sc68_cntl(0,SC68_SET_ASID,asid);
    unlock();
  }
  DBG("set asid mode -- %d -> %d\n", asid, res);
  return res;
}

#if 0
//Not used anymore config is saved by the dial68 handler
int save_config(void)
{
  int res;
  res = sc68_cntl(0,SC68_CONFIG_SAVE);
  if (!res) {
    if (lock()) {
      if (g_sc68)
        res = sc68_cntl(g_sc68,SC68_CONFIG_LOAD);
      unlock();
    }
  }
  DBG("save config -> %d\n", res);
  return res;
}
#endif