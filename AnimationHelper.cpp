#include "stdafx.h"
#include "AnimationHelper.h"

namespace Animation
{

   inline void SAFE_CALL(HRESULT hr, const char *error = "")
   {
      if (FAILED(hr))
         throw std::runtime_error(error);
   }

   Manager::Manager()
   {
      try
      {
         m_animObjs = std::make_unique<AnimObj>();

         SAFE_CALL(m_animObjs->animationManager.CoCreateInstance(CLSID_UIAnimationManager));
         SAFE_CALL(m_animObjs->animationTimer.CoCreateInstance(CLSID_UIAnimationTimer));
         SAFE_CALL(m_animObjs->transitionLibrary.CoCreateInstance(CLSID_UIAnimationTransitionLibrary));

         // Connect the animation manager to the timer.
         // UI_ANIMATION_IDLE_BEHAVIOR_DISABLE tells the timer to shut itself
         // off when there is nothing to animate.

         CComQIPtr<IUIAnimationTimerUpdateHandler> pTimerUpdateHandler = m_animObjs->animationManager;
         SAFE_CALL(m_animObjs->animationTimer->SetTimerUpdateHandler(pTimerUpdateHandler, UI_ANIMATION_IDLE_BEHAVIOR_DISABLE));

         m_delegate.Connect(dsEvents.notif.terminating, [this]() { this->m_animObjs = 0; });
      }
      catch (std::exception &)
      {
         m_animObjs = 0;
      }
   }


   Manager::~Manager()
   {
   }

   CComPtr<IUIAnimationStoryboard> Manager::CreateStoryBoard()
   {
      CheckObjs();
      CComPtr<IUIAnimationStoryboard> s;
      SAFE_CALL(m_animObjs->animationManager->CreateStoryboard(&s), "Unable to create storyboard");
      return s;
   }

   CComPtr<IUIAnimationVariable> Manager::CreateAnimationVariable(double initialValue)
   {
      CheckObjs();
      CComPtr<IUIAnimationVariable> v;
      HRESULT hr = m_animObjs->animationManager->CreateAnimationVariable(initialValue, &v);
      SAFE_CALL(hr, "Unable to create animation variable");
      return v;
   }

   void Manager::SetTimerEventHandler(IUIAnimationTimerEventHandler *p)
   {
      CheckObjs();
      SAFE_CALL(m_animObjs->animationTimer->SetTimerEventHandler(p), "Unable to set animation timer");
   }

   CComPtr<IUIAnimationVariable> Manager::CreateAndAnimateVariable(double start, double end, double duration)
   {
      CComPtr<IUIAnimationVariable> v = CreateAnimationVariable(start);
      Storyboard sb(this);
      sb.AnimateVariable(v, end, duration);
      return v;
   }

   Manager * Manager::Instance()
   {
      typedef std::pair<DWORD, Manager *> PAIR;
      static std::vector<PAIR> threadSpecificMgrs;
      if (threadSpecificMgrs.size() != 0)
      {
         if (threadSpecificMgrs[0].second == nullptr)
            return nullptr;
      }

      DWORD threadId = GetCurrentThreadId();
      auto f = std::find_if(threadSpecificMgrs.begin(), threadSpecificMgrs.end(), [threadId](const PAIR &p) { return threadId == p.first; });
      if (f != threadSpecificMgrs.end())
         return f->second;

      Manager *m = new Manager;
      if (m->m_animObjs == 0)
      {
         // No animation - probably windows XP
         delete m;
         m = 0;
      }
      threadSpecificMgrs.push_back(std::make_pair(threadId, m));
      return threadSpecificMgrs.back().second;
   }

   void Storyboard::AnimateVariable(IUIAnimationVariable *pv, double endValue, double duration, MODE mode)
   {
      const DOUBLE ACCELERATION_RATIO = 0.2;
      const DOUBLE DECELERATION_RATIO = 0.4;

      auto ao = *Manager::Instance()->m_animObjs;

      HRESULT hr = E_FAIL;
      CComPtr<IUIAnimationTransition> transition;
      if (mode == MODE::SmoothStop)
      {
         hr = ao.transitionLibrary->CreateSmoothStopTransition(duration, endValue, &transition);
      }
      else
      {
         hr = ao.transitionLibrary->CreateAccelerateDecelerateTransition(
            duration,
            endValue,
            ACCELERATION_RATIO,
            DECELERATION_RATIO,
            &transition
            );
      }

      hr = m_sb->AddTransition( pv, transition);
   }

   Storyboard::Storyboard(Manager *m)
   {
      if (FAILED(m->m_animObjs->animationManager->CreateStoryboard(&m_sb)))
         throw std::runtime_error("Unable to create storyboard");
   }

   Storyboard::~Storyboard()
   {
      auto ao = *Manager::Instance()->m_animObjs;

      UI_ANIMATION_SECONDS secondsNow;
      ao.animationTimer->GetTime(&secondsNow);
      m_sb->Schedule(secondsNow);
   }
}
