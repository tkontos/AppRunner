#pragma once

#include "AnimationHelper.h"

class CWindowAnimate : private CTimerEventHandlerImpl
{
public:
   CWindowAnimate();

   enum class MODE
   {
      UP, DN, LEFT, RIGHT
   };
   void  Animate(HWND hWndFrom, HWND hWndTo, MODE mode);


private:
   CRect                         m_rc;
   std::pair<CWindow, CWindow>   m_wndSlide;
   MODE                          m_mode;
   CComPtr<IUIAnimationVariable> m_animX, m_animY;

   void AnimateImpl(Animation::Manager *anim, MODE mode);
   Animation::Manager *GetAnimMgr()
   {
      return Animation::Manager::Instance();
   }

   // IUIAnimationTimerEventHandler
   IFACEMETHODIMP OnPostUpdate();
};
