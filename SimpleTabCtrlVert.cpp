#include "stdafx.h"

#define SIMPLETABCTRL_IMPL
#include "SimpleTabCtrlVert.h"

#define ANIMATE_FOREGROUND    1

const int leftMargin  = 5;
const int topMargin   = 10;
const int rightMargin = 3;
const int btnMargin   = 0;
const int btnHeight   = 40;

CSimpleTabCtrlVert::CSimpleTabCtrlVert(HWND hParentWnd, int idc, std::function<bool(unsigned ndx)> func)
   : m_hParentWnd(hParentWnd), m_func(func)
{
   SubclassWindow(CWindow(hParentWnd).GetDlgItem(idc));
//   CreateViewFont();
   ShowWindow(SW_SHOW);
}

void CSimpleTabCtrlVert::AddButton(const char *psz, CView *pwv)
{
   CRect rc;
   GetClientRect(rc);

   rc.left   += leftMargin;
   rc.top    += topMargin + m_bvl.size() * (btnHeight + btnMargin);
   rc.bottom  = rc.top + btnHeight;

   USES_CONVERSION_EX;

   BtnData bd;

   bd.rc       = rc;
   bd.title    = std::wstring(A2CW_EX_DEF(psz)) + L" ";
   bd.pWinView_ = pwv;
   bd.mode     = m_bvl.size() == 0 ? Mode::SELECTED : Mode::NORMAL;

   m_bvl.push_back(bd);
}

void CSimpleTabCtrlVert::EnableView(bool b)
{
   if (m_enabled != b)
   {
      m_enabled = b;
      if (auto am = Animation::Manager::Instance())
      {
         am->SetTimerEventHandler(this);
         m_animColor = am->CreateAndAnimateVariable(b ? 0.0 : 1.0, b ? 1.0 : 0.0, 0.5);
      }
      else
      {
         Invalidate();
      }
   }
}

void CSimpleTabCtrlVert::SelectItem(size_t ndx)
{
   for (size_t i = 0; i < m_bvl.size(); ++i)
   {
      if (i == ndx)
         m_bvl[i].mode = Mode::SELECTED;
      else if (m_bvl[i].mode == Mode::SELECTED)
         m_bvl[i].mode = Mode::NORMAL;
   }

   Invalidate();
}

// void CButtonViewGroupAzure::CreateViewFont()
// {
//    CFontHandle font = CWindow(m_hParentWnd).GetFont();
// 
//    if (1)
//    {
// 
//    LOGFONT  lf =
//       { -16, 0, 0, 0, 400, 255, 0, 0,
//       0, 3, 2, 1, 34, _T("Arial") };
// 
//       m_fontStd.CreateFontIndirect(&lf);
// 
//       lf.lfWeight = FW_BOLD;
//       lf.lfItalic = 0;
//       lf.lfWeight -= 2;
//       m_fontSelected.CreateFontIndirect(&lf);
//    }
//    else
//    {
//    LOGFONT lf;
//    font.GetLogFont(&lf);
// 
//    lf.lfItalic = 1;
//    lf.lfHeight -= 3;
//       m_fontStd.CreateFontIndirect(&lf);
// 
// 
//       lf.lfWeight = FW_BOLD;
//       lf.lfItalic = 0;
//       lf.lfWeight -= 2;
//       m_fontSelected.CreateFontIndirect(&lf);
//    }
// 
// //    LOGFONT  lf =
// //    { -19, 0, 0, 0, 700, 0, 0, 0, 0, 3, 2, 1, 34, L"Arial" };
// // 
// //    m_font.CreateFontIndirect(&lf);
// }

void CSimpleTabCtrlVert::DoPaint(CDCHandle dc, RECT& rect)
{
   if (IsBufferedPaintSupported())
   {
      DoPaintImpl(dc, rect);
   }
   else
   {
      CMemoryDC memDC(dc, rect);
      DoPaintImpl(memDC, rect);
   }
}

void CSimpleTabCtrlVert::PrepareToDraw(CDCHandle &dc, CRect rc, Mode mode)
{
   auto c = GetColor(mode);

   dc.SetTextColor (c.first);
   dc.SetBkColor   (c.second);
   dc.FillSolidRect(rc, c.second);
}

void CSimpleTabCtrlVert::DoPaintImpl(HDC hDC, CRect rc)
{
   CDCHandle dc = hDC;

   PrepareToDraw(dc, rc, Mode::NORMAL);

   auto &tc = Globals::Instance().tab;
   HFONT hFont = dc.SelectFont(tc.fontNormal);
   for(auto &b : m_bvl)
   {
      PrepareToDraw(dc, b.rc, b.mode);

      CRect rct(b.rc);
      rct.right -= rightMargin;
      rct.left += 2*rightMargin;
      
      if (b.mode == Mode::SELECTED)
      {
         dc.SelectFont(tc.fontSelected);
         dc.DrawText(b.title.c_str(), b.title.size(), rct, /*DT_RIGHT | */DT_VCENTER | DT_SINGLELINE);
         dc.SelectFont(tc.fontNormal);
      }
      else
      {
         dc.DrawText(b.title.c_str(), b.title.size(), rct, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

      }
   }
   dc.SelectFont(hFont);
}

void CSimpleTabCtrlVert::OnMouseMove(UINT nFlags, CPoint point)
{
   size_t curHover = std::find_if(m_bvl.begin(), m_bvl.end(), 
      [] ( const BtnData &d) { return d.mode == Mode::HOVER; }) - m_bvl.begin();

   size_t newHover = std::find_if(m_bvl.begin(), m_bvl.end(),
      [point](const BtnData &d) { return d.rc.PtInRect(point); }) - m_bvl.begin();

   if (curHover == newHover)
      return;

   if (curHover < m_bvl.size())
      m_bvl[curHover].mode = Mode::NORMAL;

   bool bInHover = false;
   if (newHover < m_bvl.size())
   {
      if (m_bvl[newHover].mode != Mode::SELECTED)
      {
         m_bvl[newHover].mode = Mode::HOVER;
         bInHover = true;
      }
   }

   if (GetCapture() == m_hWnd)
   {
      if (!bInHover)
         ReleaseCapture();
   }
   else
   {
      if (bInHover)
         SetCapture();
   }

   Invalidate();
}

void CSimpleTabCtrlVert::OnLButtonDown(UINT nFlags, CPoint point)
{
   if (!m_enabled)
   {
      return;
   }

   size_t curSel = std::find_if(m_bvl.begin(), m_bvl.end(),
      [](const BtnData &d) { return d.mode == Mode::SELECTED; }) - m_bvl.begin();

   size_t newSel = std::find_if(m_bvl.begin(), m_bvl.end(),
      [point](const BtnData &d) { return d.rc.PtInRect(point); }) - m_bvl.begin();

   if (newSel >= m_bvl.size() || curSel == newSel)
      return;

   if (!m_func(newSel))
      return;

   for( size_t i=0; i != m_bvl.size(); ++i)
   {
      m_bvl[i].mode = (i == newSel) ? Mode::SELECTED : Mode::NORMAL;
   }
   Invalidate();
}

auto CSimpleTabCtrlVert::GetColor( Mode mode) -> ColorPair
{
   double x = m_enabled ? 1.0 : 0.0;
   if (m_animColor)
      m_animColor->GetValue(&x);

   auto &g = Globals::Instance();

   const COLORREF rgbBgNormal   = g.tab.bkColor; // RGB(229, 243, 251);
   const COLORREF rgbBgHover    = g.tab.bkColorHover;
   const COLORREF rgbBgSelected = g.bkColor;
   //const COLORREF rgbBgDisabled = RGB(229, 229, 229);

   ColorPair c;

#if ANIMATE_FOREGROUND
   switch (mode)
   {
   default:
   case Mode::NORMAL:
      c.first = Interpolate(g.tab.fgNormal, g.tab.fgDisabled, x);
      c.second = rgbBgNormal;
      break;

   case Mode::HOVER:
      c.first = Interpolate(g.tab.fgNormal, g.tab.fgDisabled, x);
      c.second = m_enabled ? rgbBgHover : rgbBgNormal;
      break;

   case Mode::SELECTED:
      c.first = g.tab.fgSelected;
      c.second = rgbBgSelected;
      break;
   }
#else
   c.first = g.tab.fgNormal;
   switch (mode)
   {
   default:
   case Mode::NORMAL:
      c.second = Interpolate(rgbBgNormal, rgbBgDisabled, x);
      break;

   case Mode::HOVER:
      c.second = m_enabled ? rgbBgHover : rgbBgDisabled;
      break;

   case Mode::SELECTED:
      c.second = rgbBgSelected;
      break;
   }
#endif

   return c;
}
