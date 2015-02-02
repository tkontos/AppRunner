#include "stdafx.h"
#include "Globals.h"

DsEvents dsEvents;

Globals::Globals(void)
{
   Initialize();
   Initialize(tab);
   Initialize(sep);
}

void Globals::Initialize()
{
   animationDuration = 0.4;

   bkColor = RGB(251, 251, 252); // RGB(111, 255, 255);
   bkBrush.CreateSolidBrush(bkColor);
}

void Globals::Initialize(TAB &tab)
{
   tab.fgNormal     = RGB(255, 255, 255);
   tab.fgSelected   = RGB(  0,   0,   0);
   tab.fgDisabled   = RGB(121, 148, 176); // RGB(209, 212, 222);
   tab.bkColor      = RGB(101, 128, 146); // RGB(229, 243, 251);
   tab.bkColorHover = RGB(121, 148, 176); // RGB(203, 232, 246);

   tab.bkBrush.CreateSolidBrush(tab.bkColor);

   {
      LOGFONT	lf =
      { -16, 0, 0, 0, 400, 255, 0, 0,
      0, 3, 2, 1, 34, _T("Arial") };

      tab.fontNormal.CreateFontIndirect(&lf);

      lf.lfWeight = FW_BOLD;
      lf.lfItalic = 0;
      lf.lfWeight -= 2;
      tab.fontSelected.CreateFontIndirect(&lf);
   }
}

void Globals::Initialize(Separator &s)
{
   LOGFONT	lf =
   { -13, 0, 0, 0, 700, 255, 0, 0,
   0, 3, 2, 1, 34, _T("Calibri") };

   s.textColor = RGB(181, 181, 182);
   s.font.CreateFontIndirect(&lf);
   s.pen.CreatePen(PS_SOLID, 1, RGB(211, 211, 212));

}
