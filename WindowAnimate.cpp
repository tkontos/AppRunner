#include "stdafx.h"
#include "WindowAnimate.h"

////////////////////////////////////////////////////////////////////////////////////////////////

CWindowAnimate::CWindowAnimate()
{
}

void CWindowAnimate::Animate(HWND hWndFrom, HWND hWndTo, MODE mode)
{
   if (hWndFrom == hWndTo)
      return;

   m_wndSlide = std::make_pair(hWndFrom, hWndTo);
   if (auto anim = GetAnimMgr())
   {
      AnimateImpl(anim, mode);
   }
   else
   {
      m_wndSlide.first.ShowWindow(SW_HIDE);
      m_wndSlide.second.ShowWindow(SW_SHOW);
   }
}

void CWindowAnimate::AnimateImpl(Animation::Manager *anim, MODE mode)
{
   CWindow parent = m_wndSlide.first.GetParent();
   m_wndSlide.first.GetWindowRect(&m_rc);
   parent.ScreenToClient(&m_rc);

   m_wndSlide.second.MoveWindow(-2*m_rc.right, 0, 0, m_rc.bottom);
   m_wndSlide.second.ShowWindow(SW_SHOW);

   anim->SetTimerEventHandler(this);
   Animation::Storyboard sb(anim);

   typedef std::pair<double, double> PAIR;
   PAIR x, y;
   switch (m_mode = mode)
   {
   case MODE::DN:
      y = std::make_pair(-double(m_rc.Height()), 0.0);
      break;

   case MODE::UP:
      y = std::make_pair(double(m_rc.Height()), 0.0);
      break;

   case MODE::RIGHT:
      x = std::make_pair(-double(m_rc.Width()), 0.0);
      break;

   case MODE::LEFT:
      x = std::make_pair(double(m_rc.Width()), 0.0);
      break;
   }

   m_animX = anim->CreateAnimationVariable(x.first);
   m_animY = anim->CreateAnimationVariable(y.first);

   sb.AnimateVariable(m_animX, x.second, Globals::Instance().animationDuration);
   sb.AnimateVariable(m_animY, y.second, Globals::Instance().animationDuration);
}


IFACEMETHODIMP CWindowAnimate::OnPostUpdate()
{
   INT32 x, y;
   m_animX->GetIntegerValue(&x);
   m_animY->GetIntegerValue(&y);

   CRect rcPrev(m_rc);
   if (m_mode == MODE::UP || m_mode == MODE::DN)
   {
      if (y < 0)
         rcPrev.OffsetRect(0, y + m_rc.Height());
      else
         rcPrev.OffsetRect(0, y - m_rc.Height());
   }

   if (m_mode == MODE::LEFT || m_mode == MODE::RIGHT)
   {
      if (x < 0)
         rcPrev.OffsetRect(x + m_rc.Width(), 0);
      else
         rcPrev.OffsetRect(x - m_rc.Width(), 0);
   }

   CRect rcCur(m_rc);
   rcCur.OffsetRect(x, y);

   CRect rc = m_rc & rcPrev;
   if (rc.IsRectEmpty())
      ShowWindow(m_wndSlide.first, SW_HIDE);

   m_wndSlide.first.MoveWindow(rcPrev);
   m_wndSlide.second.MoveWindow(rcCur);
   m_wndSlide.second.Invalidate();

   return S_OK;
}
