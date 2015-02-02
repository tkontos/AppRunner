//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Separator.h"

CSeparator::CSeparator(HWND hParent, CPoint pt, int cx, const TCHAR *p)
{
   static int height;
   if (height == 0)
   {
      CDCHandle dc = ::GetDC(NULL);

      HFONT hOldFont = dc.SelectFont(Globals::Instance().sep.font);

      CSize textSize;
      dc.GetTextExtent(L"ABC", 3, &textSize);
      dc.SelectFont(hOldFont);
      height = textSize.cy;

      ::ReleaseDC(NULL, dc);
   }

	CRect rcSep(pt.x, pt.y, pt.x+cx, pt.y + (p && *p ? height : 5));
	Create(hParent, rcSep, nullptr, SS_GRAYFRAME | WS_CHILD | WS_VISIBLE);
	SetWindowText(p);
	ShowWindow(SW_SHOW);
}

int CSeparator::GetHeight() const
{
   CRect rc;
   GetClientRect(rc);
   return rc.Height();
}

void CSeparator::OnPaint(CDCHandle)
{
	CPaintDC dc(*this);

	// Get the text
	CString text;
	GetWindowText(text);

	// Get the rectangle
	CRect clientRect;
	GetClientRect(&clientRect);

   dc.FillSolidRect(clientRect, Globals::Instance().bkColor);
   DrawLine(dc, clientRect);

   if (!text.IsEmpty())
      DrawTextImpl(dc, text, clientRect);
}


BOOL CSeparator::OnEraseBackground(CDCHandle dc)
{
   return TRUE;
}

void CSeparator::DrawLine(CPaintDC &dc, CRect &clientRect)
{
   HPEN hp = dc.SelectPen(Globals::Instance().sep.pen);
   dc.MoveTo(clientRect.left, (clientRect.top + clientRect.bottom) / 2-1);
   dc.LineTo(clientRect.right, (clientRect.top + clientRect.bottom) / 2-1);
   dc.SelectPen(hp);
}

void CSeparator::DrawTextImpl(CPaintDC &dc, CString text, CRect clientRect)
{
   auto &g = Globals::Instance();

   HFONT hOldFont = dc.SelectFont(g.sep.font);

   dc.SetTextColor(g.sep.textColor);
   dc.SetBkColor(g.bkColor);

   const int margin = 0;
   CSize textSize;
   dc.GetTextExtent(text, -1, &textSize);
   CRect textRect(clientRect);
   textRect.left = margin;
   textRect.right = textRect.left + textSize.cx + 2 * 3;
   dc.FillSolidRect(textRect, g.bkColor);
   textRect.InflateRect(-3, 0);
   dc.DrawText(text, -1, textRect, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

   dc.SelectFont(hOldFont);
}
