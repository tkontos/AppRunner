// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"
#include "SimpleTabCtrl.h"
#include "ViewContainer.h"

class CMainDlg
   : public CViewBase<CMainDlg>
   , public CUpdateUI<CMainDlg>
   , public CDialogResize<CMainDlg>
   , public CMessageFilter
   , public CIdleHandler
//   , public CBufferedPaintWindowImpl<CMainDlg>

{
public:
   CMainDlg(CXmlFile &xml);

   enum { IDD = IDD_MAINDLG };

   BEGIN_UPDATE_UI_MAP(CMainDlg)
   END_UPDATE_UI_MAP()

   BEGIN_MSG_MAP(CMainDlg)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
      COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
      MSG_WM_ERASEBKGND(OnEraseBackground)
      MSG_WM_CTLCOLORBTN(OnCtlColorButton)
      MSG_WM_CTLCOLORSTATIC(OnCtlColorButton)
      CHAIN_MSG_MAP(CDialogResize<CMainDlg>)
   END_MSG_MAP()

   BEGIN_DLGRESIZE_MAP(CMainDlg)
      DLGRESIZE_CONTROL(IDC_TABMENU, DLSZ_SIZE_Y)
      DLGRESIZE_CONTROL(IDC_CONTENTS, DLSZ_SIZE_X | DLSZ_SIZE_Y)
      DLGRESIZE_CONTROL(IDCANCEL, DLSZ_MOVE_Y)
      DLGRESIZE_CONTROL(IDC_AUTO_CLOSE, DLSZ_MOVE_Y)
   END_DLGRESIZE_MAP()

   LRESULT OnInitDialog     (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnDestroy        (UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnCancel         (WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   BOOL    OnEraseBackground(CDCHandle dc);
   HBRUSH  OnCtlColorButton (CDCHandle dc, HWND /*hWnd*/);

protected:
   virtual BOOL PreTranslateMessage(MSG* pMsg) override;
   virtual BOOL OnIdle() override;

private:
   enum class Sect { Tab, Contents};

   CRect GetSectRect(Sect s) const;
   void DrawVersion(CDCHandle &dc, CRect rcc);

   std::unique_ptr<CSimpleTabCtrl>  m_btnViewGroup;
   CViewContainer                   m_viewContainer;
   Delegate                         m_delegate;
   CFont                            m_font;
};
