#include "stdafx.h"
#include "MainDlg.h"
#include "Settings.h"
#include "WindowPlacement.h"

inline BOOL IsShiftKeyPressed()
{
   return ::GetAsyncKeyState(VK_SHIFT) < 0;
}

CMainDlg::CMainDlg(CXmlFile &xml) : m_viewContainer(xml)
{
   LOGFONT	lf =
   { -10, 0, 0, 0, 400, 255, 0, 0,
   0, 3, 2, 1, 34, _T("Arial") };

   m_font.CreateFontIndirect(&lf);
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   DestroyWindow();
   ::PostQuitMessage(wID);
   return 0;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   auto &si = CSettings::Instance();

   si.m_winPlacement = CWindowPlacement(*this).Save();
   si.m_exitOnCommand = SendDlgItemMessage(IDC_AUTO_CLOSE, BM_GETCHECK);
   si.Save();

   // unregister message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->RemoveMessageFilter(this);
   pLoop->RemoveIdleHandler(this);

   return 0;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   // center the dialog on the screen
   CenterWindow();
   DlgResize_Init(false);

   // set icons
   HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
   SetIcon(hIcon, TRUE);
   HICON hIconSmall = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
   SetIcon(hIconSmall, FALSE);

   // register object for message filtering and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->AddMessageFilter(this);
   pLoop->AddIdleHandler(this);

   m_btnViewGroup = CSimpleTabCtrl::CreateIntance(*this, IDC_TABMENU, [this](unsigned ndx)->bool
   {
      m_viewContainer.ShowWindow(ndx);
      return true;
   });
   m_viewContainer.CreateContainer(*m_btnViewGroup, *this, IDC_CONTENTS);

   m_delegate.Connect(dsEvents.notif.commandFired, [this]() {
      if (this->SendDlgItemMessage(IDC_AUTO_CLOSE, BM_GETCHECK))
         this->PostMessage(WM_COMMAND, IDCANCEL);
   });
   SendDlgItemMessage(IDC_AUTO_CLOSE, BM_SETCHECK, CSettings::Instance().m_exitOnCommand);

   UIAddChildWindowContainer(m_hWnd);

   CWindowPlacement wp(*this);
   (void)wp.Restore(CSettings::Instance().m_winPlacement);

   return TRUE;
}

BOOL CMainDlg::OnIdle()
{
   UIUpdateChildWindows();
   return FALSE;
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
   return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnEraseBackground(CDCHandle dc)
{
   auto &g = Globals::Instance();

   dc.FillSolidRect(GetSectRect(Sect::Tab), g.tab.bkColor);

   CRect rcc = GetSectRect(Sect::Contents);
   dc.FillSolidRect(rcc, g.bkColor);

   DrawVersion(dc, rcc);

   return 1;
}

CRect CMainDlg::GetSectRect(Sect s) const
{
   CRect rc;
   GetClientRect(rc);

   CRect r;
   CWindow w = GetDlgItem(IDC_TABMENU);
   w.GetWindowRect(r);
   ScreenToClient(r);

   switch (s)
   {
   case Sect::Tab:
      rc.right = r.right;
      break;
   case Sect::Contents:
      rc.left = r.right;
      break;
   default:
      break;
   }

   return rc;
}

void CMainDlg::DrawVersion(CDCHandle &dc, CRect rcc)
{
   CModuleVersion ver;
   if (!ver.GetFileVersionInfo(NULL))
      return;

   TCHAR sz[100];
   if (1)
   {
      CString sp = ver.GetValue(L"ProductName");
      CString sc = ver.GetValue(L"LegalCopyright");

      _stprintf_s(sz, _T("Version %d.%02d. %s"),
         HIWORD(ver.dwFileVersionMS), LOWORD(ver.dwFileVersionMS), sc);
   }
   else
   {
      _stprintf_s(sz, _T("AppRunner Version %d.%02d.%04d.%04d"),
         HIWORD(ver.dwFileVersionMS), LOWORD(ver.dwFileVersionMS),
         HIWORD(ver.dwFileVersionLS), LOWORD(ver.dwFileVersionLS));
   }

   HFONT hf = dc.SelectFont(m_font);
   auto c = dc.SetBkColor(Globals::Instance().bkColor);
   dc.DrawText(sz, -1, rcc, DT_RIGHT | DT_BOTTOM | DT_SINGLELINE);
   dc.SetBkColor(c);
   dc.SelectFont(hf);
}

HBRUSH CMainDlg::OnCtlColorButton(CDCHandle dc, HWND hWnd)
{
   const Globals &bd = Globals::Instance();
   if (::GetDlgCtrlID(hWnd) == IDCANCEL)
   {
      dc.SetBkColor(bd.tab.bkColor);
      return bd.tab.bkBrush;
   }
   else
   {
      dc.SetTextColor(bd.tab.fgNormal);
      dc.SetBkColor(bd.tab.bkColor);
      return bd.tab.bkBrush;
   }
}
