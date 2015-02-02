// MainViewDlg.h : interface of the CMainViewDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "resource.h"
#include "CommandPage.h"
#include "Separator.h"

template<typename Base>
class CViewBase : public CDialogImpl < Base >
{
public:
   CViewBase()
   { }

   BEGIN_MSG_MAP(CViewBase)
      MSG_WM_CTLCOLORSTATIC(OnCtlColorStatic)
      MSG_WM_CTLCOLORBTN(OnCtlColorStatic)
      MSG_WM_CTLCOLORDLG(OnCtlColorStatic)
   END_MSG_MAP()

   HBRUSH  OnCtlColorStatic(CDCHandle dc, HWND hWnd)
   {
      const Globals &bd = Globals::Instance();
      dc.SetBkColor(bd.bkColor);
      return bd.bkBrush;
   }

protected:
   CRect GetDlgItemRect(int idc)
   {
      CRect rc;
      GetDlgItem(idc).GetWindowRect(&rc);
      ScreenToClient(&rc);
      return rc;
   }
};

class CView : public CViewBase<CView>
{
public:
   enum {CMD_START = 40000, CMD_END = CMD_START+100};

   CView(const CommandPage &p)
      : m_cmdPage(p) { }

   enum { IDD = IDD_VIEW };
      
   BEGIN_MSG_MAP( CView)
      MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
      COMMAND_RANGE_HANDLER( CMD_START, CMD_END, OnCommand)
      MSG_WM_SIZE(OnSize)
      //REFLECT_NOTIFICATIONS()
      CHAIN_MSG_MAP(CViewBase<CView>)
   END_MSG_MAP( )

   LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
   LRESULT OnCommand(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
   void OnSize(UINT nType, CSize size);

private:
   typedef std::unique_ptr<CButton>       ButtonPtr;
   typedef std::unique_ptr<CSeparator>    SeparatorPtr;

   Delegate                   m_delegate;
   CommandPage                m_cmdPage;
   std::vector<ButtonPtr>     m_buttons;
   std::vector<SeparatorPtr>  m_separators;
};
