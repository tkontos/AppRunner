// MainViewDlg.cpp : implementation of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "View.h"
#include "Separator.h"

const int margin  = 15;
const int sep     = 17;
const int space   = 7;
const int spacing = 3;

LRESULT CView::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
   CRect rc;
   GetClientRect(rc);

   const int cx = rc.Width()-2*margin, cy = 70;
   int y = 5;
   int cmd = CMD_START;

   for (auto &b : m_cmdPage.buttons)
   {
      if (b.type == Command::Type::Sep)
      {
         auto s = std::make_unique<CSeparator>(*this, CPoint(margin, y), cx - 2 * margin, b.name.c_str());
         y += s->GetHeight();
         m_separators.push_back(std::move(s));
      }
      else
      {
         auto btn = std::make_unique<CButton>();

         btn->Create(*this, CRect(margin, y, margin + cx, y + cy), b.name.c_str(), BS_COMMANDLINK | WS_CHILD | WS_VISIBLE | WS_TABSTOP, 0, cmd);
         btn->SetNote(b.note.size() != 0 ? b.note.c_str() : b.command.c_str());

         if (auto icon = b.GetIcon())
            btn->SetIcon(icon);
         m_buttons.push_back(std::move(btn));

         y += cy + spacing;

      }

      ++cmd;
   }

   return TRUE;
}

LRESULT CView::OnCommand(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
   if (wID >= CMD_START && wID < CMD_END)
   {
      size_t ndx = wID - CMD_START;

      auto &b = m_cmdPage.buttons;
      if (ndx < b.size())
         b[ndx].RunCommand();
   }
   return 0;
}

void CView::OnSize(UINT /*nType*/, CSize size)
{
   const int cxBtn = size.cx - 2 * margin;
   const int cxLine = size.cx - 6 * margin;

   HWND hWnd = ::GetTopWindow(*this);
   while (hWnd != 0)
   {
      TCHAR szClassName[256];
      ::GetClassName(hWnd, szClassName, 255);

      CWindow w = hWnd;
      CRect rc;
      w.GetWindowRect(&rc);
      ScreenToClient(&rc);

      if (_tcsicmp(szClassName, _T("Static")) == 0)
      {
         rc.right = rc.left + cxLine;
         w.MoveWindow(rc);
      }
      else 
      {
         rc.right = rc.left + cxBtn;
         w.MoveWindow(rc);
      }

      hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
   }

//    for (auto &b : m_buttons)
//    {
//       CRect rc;
//       b->GetWindowRect(&rc);
//       ScreenToClient(&rc);
// 
//       rc.right = rc.left + cxBtn;
//       b->MoveWindow(rc);
//    }
}
