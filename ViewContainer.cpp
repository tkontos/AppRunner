#include "stdafx.h"
#include "resource.h"
#include "ViewContainer.h"
#include "SimpleTabCtrl.h"
#include "Settings.h"

////////////////////////////////////////////////////////////////////////////////////////////////

CViewContainer::~CViewContainer()
{
}

BOOL CViewContainer::CreateContainer(CSimpleTabCtrl &bvg, HWND hWndParent, UINT nID)
{
   CWindow parent = hWndParent;
   CWindow wnd = parent.GetDlgItem(nID);
   if (wnd == 0)
      return FALSE;

   CRect rc;
   wnd.GetWindowRect(&rc);
   wnd.DestroyWindow();
   parent.ScreenToClient(&rc);

   if (!BaseClass::Create(hWndParent, &rc, _T("Container"), WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, WS_EX_CONTROLPARENT, nID))
      return FALSE;

   // register object for and idle updates
   CMessageLoop* pLoop = _Module.GetMessageLoop();
   ATLASSERT(pLoop != NULL);
   pLoop->AddIdleHandler(this);

   auto pages = m_xml.GetPages();
   for (auto &p : pages)
   {
      AddView(bvg, p.name.c_str(), p);
   }

   auto ndx = CSettings::Instance().m_lastOpenedPage;
   if (ndx >= m_vdl.size())
      ndx = 0;

   m_curWndNdx = ndx;
   m_vdl[ndx]->ShowWindow(SW_SHOW);

   bvg.SelectItem(ndx);

   return TRUE;
}

void CViewContainer::ShowWindow(unsigned nextWnd)
{
   if (m_curWndNdx == nextWnd)
      return;

   CSettings::Instance().m_lastOpenedPage = nextWnd;

   m_prevWndNdx = m_curWndNdx;
   m_curWndNdx = nextWnd;

   m_wa.Animate(*m_vdl[m_prevWndNdx], *m_vdl[m_curWndNdx], m_prevWndNdx < m_curWndNdx ? CWindowAnimate::MODE::UP : CWindowAnimate::MODE::DN);
}

void CViewContainer::OnSize(UINT nType, CSize size)
{
   HWND hWnd = GetTopWindow();
   while (hWnd != 0)
   {
      CWindow w = hWnd;
      w.MoveWindow(0, 0, size.cx, size.cy);
      w.Detach();

      hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
   }
}

BOOL CViewContainer::OnIdle()
{
   return FALSE;
}
