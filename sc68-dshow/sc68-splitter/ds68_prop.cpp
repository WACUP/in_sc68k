/*
 * @file    ds68_prop.cpp
 * @brief   sc68 for directshow - properties page
 * @author  http://sourceforge.net/users/benjihan
 *
 * Copyright (c) 1998-2014 Benjamin Gerard
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
#include "config.h"
#endif

#include "ds68_headers.h"
#include "ds68_dbg.h"
#include <assert.h>

HRESULT Sc68Splitter::GetASID(int * asid)
{
  HRESULT hr = S_OK;
  int v = SC68_ASID_OFF;
  if (!asid) {
    hr = E_POINTER;
  } else {
    v = sc68_cntl(m_sc68, SC68_GET_ASID);
    if (v == -1) {
      v = SC68_ASID_OFF;
      hr = E_FAIL;
    }
    *asid = v;
  }
  DBG("%s()=%d => %s\n",__FUNCTION__,v,!hr?"OK":"FAIL");
  return hr;
}

HRESULT Sc68Splitter::SetASID(int asid)
{
  int v = sc68_cntl(m_sc68, SC68_SET_ASID,asid);
  HRESULT hr = (v == -1) ? E_INVALIDARG : S_OK;
  DBG("%s(%d)=%d => %s\n",__FUNCTION__,asid,v,!hr?"OK":"FAIL");
  return hr;
}

// ISpecifyPropertyPages::GetPages() method
// Retrieves a list of property pages that can be displayed in this object's property sheet.
HRESULT Sc68Splitter::GetPages(CAUUID *pPages)
{
  HRESULT hr;
  if (!pPages)
    hr = E_POINTER;
  else {
    hr = E_OUTOFMEMORY;
    pPages->cElems = 1;
    pPages->pElems = (GUID *)CoTaskMemAlloc(sizeof(GUID) * pPages->cElems);
    if (pPages->pElems != nullptr) {
      pPages->pElems[0] = __uuidof(Sc68Prop);
      hr = S_OK;
    }
  }
  DBG("%s() => %s\n",__FUNCTION__,!hr?"OK":"FAIL");
  return hr;
}

#define keyis(N) !strcmp(key,N)

int Sc68Prop::Cntl(const char * key, int op, sc68_dialval_t *val)
{
  assert(key && val);
  if (!key || !val)
    return -1;

  switch (op) {
  case SC68_DIAL_CALL:
    if (keyis(SC68_DIAL_KILL)) {
      m_hparent = m_hwnd = 0;
      m_dialRetval = val->i;
    }
    else if (keyis(SC68_DIAL_HELLO))
      val->s = "config";
    else if (keyis(SC68_DIAL_WAIT))
      val->i = !!m_bModal;
    else if (keyis("instance"))
      val->s = (const char *) g_hInst;
    else if (keyis("parent"))
      val->s = (const char *) m_hparent;
    else if (keyis("handle"))
      m_hwnd = (HWND) val->s;
    else if (keyis("frameonly") || keyis("borderless"))
      val->i = 1;
    else if (keyis("measuring"))
      val->i = !!m_bMeasuring;
    else if (keyis("wm_windowposchanged")) {
      LPWINDOWPOS pos = (LPWINDOWPOS) val->s;
      if (pos && (pos->flags&SWP_NOACTIVATE)) {
        if (pos->cx > 0) m_size.cx = pos->cx;
        if (pos->cy > 0) m_size.cy = pos->cy;
      }
    }
    else if (keyis("wm_windowposchanging")) {
      LPWINDOWPOS pos = (LPWINDOWPOS) val->s;
      if (pos && (pos->flags&SWP_SHOWWINDOW)) {
        if (m_rect.left < m_rect.right) {
          RECT parentrect;
          pos->x = m_rect.left;
          pos->y = m_rect.top;
          if (m_hparent && GetWindowRect(m_hparent,&parentrect)) {
            pos->x += parentrect.left;
            pos->y += parentrect.top;
          }
          pos->flags &= ~SWP_NOMOVE;
          m_rect.right = m_rect.left;
        }
      }
    }
    else if (keyis("asid"))
      /*set_asid(val->i)*/;
    else break;
    return 0;
  }
  return 1;
}

static int cntl(void * _cookie, const char * key, int op, sc68_dialval_t *val)
{
  return ((Sc68Prop *)_cookie)->Cntl(key,op,val);
}

Sc68Prop::Sc68Prop(IUnknown *pUnk)
    : CUnknown(NAME("Sc68Prop"), pUnk)
    , m_pIf(nullptr)
    , m_asid(SC68_ASID_OFF)
    , m_bModal(false)
    , m_bMeasuring(false)
    , m_dialRetval(-2)
    , m_hparent(0)
    , m_hwnd(0)
{
  m_size.cx = m_size.cy = -1;
  m_rect.left = m_rect.right = m_rect.top= m_rect.bottom = -1;

  DBG("%s()\n",__FUNCTION__);
}

// Creates the dialog box window for the property page.
HRESULT Sc68Prop::Activate(HWND hWndParent,LPCRECT pRect,BOOL bModal)
{
  HRESULT hr = E_NOTIMPL;

  //// Language
  //LCID localeID;
  //hr = GetLocaleID(&localeID);

  m_bModal = !!bModal;
  m_bMeasuring = false;
  m_hparent = hWndParent;
  m_rect = *pRect;
  m_size.cx = m_rect.right-m_rect.left;
  m_size.cy = m_rect.bottom-m_rect.top;
  sc68_cntl(0, SC68_DIAL, this, cntl);

  DBG("%s -> [%s]\n", __FUNCTION__, !hr ?"OK":"FAIL");
  return hr;
}
HRESULT Sc68Prop::Apply()
{
  HRESULT hr = E_NOTIMPL;
  DBG("%s -> [%s]\n", __FUNCTION__, !hr ?"OK":"FAIL");
  return hr;
}

HRESULT Sc68Prop::Deactivate()
{
  HRESULT hr = E_NOTIMPL;
  DBG("%s -> [%s]\n", __FUNCTION__, !hr ?"OK":"FAIL");

  // Post WM_CLOSE ?
  if (m_hwnd)
    PostMessageA(m_hwnd,WM_CLOSE,0,0);
  if (m_hwnd)
    if (m_bModal)
      PostMessageA(m_hwnd,WM_QUIT,0,0);
    else
      DestroyWindow(m_hwnd);

  m_hparent = 0;
  m_hwnd    = 0;

  return hr;
}

// Retrieves information about the property page.
HRESULT Sc68Prop::GetPageInfo(PROPPAGEINFO *pPageInfo)
{
  HRESULT hr = E_NOTIMPL;

  if (!pPageInfo)
    hr = E_POINTER;
  else for (;;) {
    // IPropertyPageSite::GetLocaleID() should be call to determine language
    WCHAR title[] = OLESTR("SC68 Config");
    WCHAR doc[]   = OLESTR("Configure SC68 filter");
    // Tab title
    pPageInfo->pszTitle = (LPOLESTR)CoTaskMemAlloc(sizeof(title));
    if (!pPageInfo->pszTitle) {
      hr = E_OUTOFMEMORY; break;
    }
    memcpy(pPageInfo->pszTitle,title,sizeof(title));

    // Set defaults

    m_bModal = true;
    m_bMeasuring =  true;
    m_size.cx = m_size.cy = -1;
    ZeroMemory(&m_rect,sizeof(m_rect));
    if ( ! sc68_cntl(0, SC68_DIAL, this, cntl) && m_size.cx > 0 && m_size.cy > 0 ) {
      pPageInfo->size = m_size;
    } else {
      // Set something reasonable ion case measurement failed */
      pPageInfo->size.cx = 340;
      pPageInfo->size.cy = 150;
    }



    hr = S_OK; break;
  }

  DBG("%s -> [%s]\n", __FUNCTION__, !hr ?"OK":"FAIL");
  return hr;
}
HRESULT Sc68Prop::IsPageDirty()
{
  HRESULT hr = E_NOTIMPL;
  DBG("%s -> [%s]\n", __FUNCTION__, !hr ?"OK":"FAIL");
  return hr;
}
HRESULT Sc68Prop::Move(LPCRECT pRect)
{
  HRESULT hr = E_NOTIMPL;
  DBG("%s -> [%s]\n", __FUNCTION__, !hr ?"OK":"FAIL");
  return hr;
}

// Provides the property page with an array of pointers to objects associated with this property page.
// @param cObjects [in] The number of pointers in the array pointed to by ppUnk.
//        If this parameter is 0, the property page must release any pointers previously passed to this method.

HRESULT Sc68Prop::SetObjects(ULONG cObjects,IUnknown **ppUnk)
{
  HRESULT hr = S_OK;

  if (cObjects == 0) {
    // Disconnect ...
  } else if (cObjects == 1) {
    // Connect ...
  } else {
    hr = E_UNEXPECTED;
  }
  DBG("%s -> %sconnect [%s]\n", __FUNCTION__, !cObjects?"dis":"", !hr ?"OK":"FAIL");
  return hr;
}

HRESULT Sc68Prop::SetPageSite(IPropertyPageSite *pPageSite)
{
  HRESULT hr = E_NOTIMPL;

  if (pPageSite) {
    if (m_pPageSite)
      hr = E_UNEXPECTED;

    m_pPageSite = pPageSite;
    m_pPageSite->AddRef();

  } else {

    if (m_pPageSite == NULL) {
      return E_UNEXPECTED;
    }

    m_pPageSite->Release();
    m_pPageSite = NULL;
  }
  hr = NOERROR;
exit:
  DBG("%s -> [%s]\n", __FUNCTION__, !hr ?"OK":"FAIL");
  return hr;
}

HRESULT Sc68Prop::Show(UINT nCmdShow)
{
    if (!m_hwnd)
        return E_UNEXPECTED;
    if ((nCmdShow != SW_SHOW) && (nCmdShow != SW_SHOWNORMAL) && (nCmdShow != SW_HIDE))
        return E_INVALIDARG;
    ShowWindow(m_hwnd,nCmdShow);
    InvalidateRect(m_hwnd,NULL,TRUE);
    return NOERROR;
}

HRESULT Sc68Prop::Help(LPCOLESTR pszHelpDir)
{
  HRESULT hr = E_NOTIMPL;
  DBG("%s -> [%s]\n", __FUNCTION__, !hr ?"OK":"FAIL");
  return hr;
}

HRESULT Sc68Prop::TranslateAccelerator(MSG *pMsg)
{
  HRESULT hr = E_NOTIMPL;
  DBG("%s -> [%s]\n", __FUNCTION__, !hr ?"OK":"FAIL");
  return hr;
}


CUnknown *WINAPI Sc68Prop::CreateInstance(LPUNKNOWN pUnk, HRESULT *phr)
{
  assert(phr);
  *phr = S_OK;
  CUnknown *punk = new Sc68Prop(pUnk);
  if(punk == nullptr)
    *phr = E_OUTOFMEMORY;
  return punk;
}

STDMETHODIMP Sc68Prop::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
  CheckPointer(ppv, E_POINTER);
  *ppv = nullptr;
  HRESULT hr = 0 ? 0
    : (riid == IID_IPropertyPage)
    ? GetInterface((IPropertyPage*)this, ppv)
    : __super::NonDelegatingQueryInterface(riid, ppv);
  return hr;
}
