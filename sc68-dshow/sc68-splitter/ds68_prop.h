/**
 * @ingroup   sc68_directshow
 * @file      ds68_prop.h
 * @brief     Interface ISc68Prop (property page)
 * @author    Benjamin Gerard
 * @date      2014/06
 */

#pragma once

/**
 * @addtogroup sc68_directshow
 * @{
 */

#include "ds68_headers.h"

[uuid(IID_SC68PROP_STR)]
interface ISc68Prop: public IUnknown
{
  STDMETHOD(GetASID)(int * asid) = 0;
  STDMETHOD(SetASID)(int asid) = 0;
};

[uuid(CLID_SC68PROP_STR)]
class Sc68Prop
  : public CUnknown, public IPropertyPage
{
  friend int cntl(void * _cookie, const char * key, int op, sc68_dialval_t *val);

private:
  ISc68Prop  *m_pIf;    // Pointer to the filter's custom interface.

  bool m_bMeasuring; ///< True when creating the dialog for measuring.
  bool m_bModal;     ///< Create the dialog modal or modless.
  int m_asid;
  int m_dialRetval;
  HWND m_hparent;
  HWND m_hwnd;

  RECT m_rect; ///< activate rectangle
  SIZE m_size; ///< Measured size

protected:
  LPPROPERTYPAGESITE m_pPageSite; // Details for our property site

  int Cntl(const char * key, int op, sc68_dialval_t *val);

public:
  static CUnknown *WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT *phr);
  Sc68Prop(IUnknown *pUnk);

  // IUnknown
  DECLARE_IUNKNOWN
  STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

  // IPropertyPage

  /// Creates the dialog box window.
  STDMETHOD(Activate)(HWND hWndParent,LPCRECT pRect,BOOL bModal);
  /// Applies the current property page values to the object associated with the property page
  STDMETHOD(Apply)();
  /// Destroys the dialog window.
  STDMETHOD(Deactivate)();
  /// Retrieves information about the property page.
  STDMETHOD(GetPageInfo)(PROPPAGEINFO *pPageInfo);
  /// Indicates whether the property page has changed since it was activated or since the most recent call to IPropertyPage::Apply.
  STDMETHOD(IsPageDirty)();
  /// Positions and resizes the dialog box.
  STDMETHOD(Move)(LPCRECT pRect);
  /// Provides IUnknown pointers for the objects associated with the property page.
  STDMETHOD(SetObjects)(ULONG cObjects,IUnknown **ppUnk);
  /// Initializes the property page.
  STDMETHOD(SetPageSite)(IPropertyPageSite *pPageSite);
  /// Shows or hides the dialog box.
  STDMETHOD(Show)(UINT nCmdShow);
  STDMETHOD(Help)(LPCOLESTR pszHelpDir);
  /// Instructs the property page to process a keystroke.
  STDMETHOD(TranslateAccelerator)(MSG *pMsg);
};

/**
 * @}
 */
