#pragma once

#ifndef SIMPLETABCTRL_IMPL
#error This file should not be included here
#endif

#include "SimpleTabCtrl.h"

class CSimpleTabCtrlVert
   : public CBufferedPaintWindowImpl<CSimpleTabCtrlVert>
   , public CSimpleTabCtrl
   , private CTimerEventHandlerImpl
{
public:
   CSimpleTabCtrlVert( HWND hParentWnd, int idc, std::function<bool(unsigned ndx)> func);

   virtual void  AddButton( const char *psz, CView *pwv) override;
   virtual void  EnableView(bool b) override;
   virtual void SelectItem(size_t ndx) override;

   BEGIN_MSG_MAP_EX(CSimpleTabCtrlVert)
      MSG_WM_LBUTTONDOWN(OnLButtonDown)
      MSG_WM_MOUSEMOVE  (OnMouseMove)
      CHAIN_MSG_MAP(CBufferedPaintWindowImpl<CSimpleTabCtrlVert>)
   END_MSG_MAP()

   void DoPaint(CDCHandle dc, RECT& rect);

   void    OnMouseMove  ( UINT nFlags, CPoint point);
   void    OnLButtonDown( UINT nFlags, CPoint point);

private:
   enum class Mode
   {
      NORMAL, HOVER, SELECTED,
   };
   struct BtnData
   {
      std::wstring  title;
      CRect         rc;
      Mode          mode = Mode::NORMAL;
      CView        *pWinView_ = 0;
   };

   HWND                               m_hParentWnd;
   bool                               m_enabled = true;
   std::function<bool(unsigned ndx)>  m_func;
   std::vector<BtnData>               m_bvl;
   CComPtr<IUIAnimationVariable>      m_animColor;


   typedef std::pair<COLORREF, COLORREF> ColorPair;
   void        PrepareToDraw(CDCHandle &dc, CRect rc, Mode mode);
   void DoPaintImpl(HDC dc, CRect rc);
   ColorPair   GetColor     (Mode mode);

   IFACEMETHODIMP OnPostUpdate()
   {
      ::InvalidateRect(m_hWnd, NULL, FALSE);
      return S_OK;
   }
   static COLORREF Interpolate(COLORREF a0, COLORREF a1, double p)
   {
      BYTE r = static_cast<BYTE>(GetRValue(a0) * p + GetRValue(a1) * (1 - p));
      BYTE g = static_cast<BYTE>(GetGValue(a0) * p + GetGValue(a1) * (1 - p));
      BYTE b = static_cast<BYTE>(GetBValue(a0) * p + GetBValue(a1) * (1 - p));

      return RGB(r, g, b);
   }


};
