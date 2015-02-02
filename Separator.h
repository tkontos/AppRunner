#pragma once

class CSeparator : public CWindowImpl<CSeparator>
{

public:
   DECLARE_WND_CLASS(L"Wtl_Separator")

   CSeparator(HWND hParent, CPoint pt, int cx, const TCHAR *p);
   int GetHeight() const;

private:
   BEGIN_MSG_MAP_EX(CSeparator)
      MSG_WM_ERASEBKGND(OnEraseBackground)
      MSG_WM_PAINT(OnPaint)
   END_MSG_MAP()

   BOOL    OnEraseBackground(CDCHandle dc);
   void    OnPaint(CDCHandle dc);

   void DrawTextImpl(CPaintDC &dc, CString text, CRect clientRect);
   void DrawLine(CPaintDC &dc, CRect &clientRect);
};


