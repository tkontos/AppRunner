#pragma once

class Globals
{
public:
   static const Globals &Instance()
   {
      static Globals gd;
      return gd;
   }

   struct TAB
   {
      COLORREF fgNormal;
      COLORREF fgSelected;
      COLORREF fgDisabled;
      COLORREF bkColor;
      COLORREF bkColorHover;
      CFont    fontNormal;
      CFont    fontSelected;
      CBrush   bkBrush;
   };

   struct Separator
   {
      COLORREF textColor;
      CFont    font;
      CPen     pen;
   };

   double      animationDuration;
   COLORREF    bkColor;
   CBrush      bkBrush;

   TAB         tab;
   Separator   sep;

private:
   Globals(void);

   void Initialize();
   void Initialize(TAB &tab);
   void Initialize(Separator &s);
};

struct DsEvents
{
   struct Notifications
   {
      Event<>          idle_;
      Event<unsigned>  changeView;
      Event<unsigned>  viewChanged;
      Event<>          terminating;
      Event<>          commandFired;
   };

   Notifications  notif;
};

extern DsEvents dsEvents;
