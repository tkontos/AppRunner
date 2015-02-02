#pragma once

#include <vector>
#include <algorithm>

namespace EventDetails
{
   namespace Internal
   {
      template<int ...>
      struct sequence
      {};

      template<int N, int ...S>
      struct generate : generate< N - 1, N - 1, S...>
      {};

      template<int ...S>
      struct generate < 0, S... >
      {
         typedef sequence<S...> type;
      };

      template< typename Callable, typename Tuple, size_t ...S >
      static void unpackAndApply(Tuple&& tuple, Callable& fn, sequence<S...>)
      {
         fn(std::get<S>(tuple) ...);
      }
   }

   template<typename Tuple, typename Callable>
   void apply_tuple(Tuple&& tuple, Callable&& fn)
   {
      using namespace Internal;
      const int size = std::tuple_size<typename std::remove_reference<Tuple>::type>::value;
      unpackAndApply(std::forward<Tuple>(tuple), std::forward<Callable>(fn), typename generate<size>::type());
   }
}

template<class... Args>
class Event;

class BaseDelegate
{
public:
   virtual ~BaseDelegate()   { }
};

template<class... Args>
class AbstractDelegate : public BaseDelegate
{
protected:
   virtual ~AbstractDelegate();

   friend class Event<Args...>;

   void Add(Event<Args...> *s)    { m_eventList.push_back(s); }
   void Remove(Event<Args...> *s) { m_eventList.erase(std::remove(m_eventList.begin(), m_eventList.end(), s), m_eventList.end()); }

   virtual void Call(Args... args) = 0;

   std::vector<Event<Args...>*> m_eventList;
};

template<class T, class... Args>
class ConcreteDelegate : public AbstractDelegate < Args... >
{
public:
   ConcreteDelegate(T *t, void(T::*f)(Args...), Event<Args...> &s);
   ConcreteDelegate(std::function<void(Args...)>, Event<Args...> &s);

private:
   ConcreteDelegate(const ConcreteDelegate&) = delete;
   void operator=(const ConcreteDelegate&) = delete;

   friend class Event < Args... > ;

   virtual void Call(Args... args) override
   {
      ATLENSURE(m_threadId == GetCurrentThreadId());
      if (m_obj)
         (m_obj->*m_memFn)(args...);
      else
         m_func(args...);
   }

   DWORD    m_threadId = 0;
   T       *m_obj = 0;
   void    (T::*m_memFn)(Args...);
   std::function<void(Args...)>  m_func;
};

template<class... Args>
class Event
{
public:
   Event()
   {
   }
   ~Event()
   {
      for (auto i : m_delegateList)
         i->Remove(this);
   }

   void Connect(AbstractDelegate<Args...> &s)
   {
      m_delegateList.push_back(&s);
      s.Add(this);
   }

   void Disconnect(AbstractDelegate<Args...> &s)
   {
      m_delegateList.erase(std::remove(m_delegateList.begin(), m_delegateList.end(), &s), m_delegateList.end());
   }

   void operator()(Args... args)
   {
      for (auto i : m_delegateList)
         i->Call(args...);
   }

private:
   Event(const Event&);
   void operator=(const Event&);

   typedef std::pair<DWORD, AbstractDelegate<Args...>> ThreadDelegate;

   std::vector<AbstractDelegate<Args...>*> m_delegateList;
};

template<class... Args>
AbstractDelegate<Args...>::~AbstractDelegate()
{
   for (auto i : m_eventList)
      i->Disconnect(*this);
}

template<class T, class... Args>
ConcreteDelegate<T, Args...>::ConcreteDelegate(T *t, void(T::*f)(Args...), Event<Args...> &s)
   : m_obj(t), m_memFn(f), m_threadId(GetCurrentThreadId())
{
   s.Connect(*this);
}

template<class T, class... Args>
ConcreteDelegate<T, Args...>::ConcreteDelegate(std::function<void(Args...)> func, Event<Args...> &s)
   : m_obj(0), m_func(func), m_threadId(GetCurrentThreadId())
{
   s.Connect(*this);
}

class Delegate
{
public:
   Delegate(){ }
   ~Delegate()
   {
      for (auto i : m_v) delete i;
   }

   template<class T, class... Args>
   void Connect(Event<Args...> &s, T *t, void(T::*f)(Args...))
   {
      m_v.push_back(new ConcreteDelegate<T, Args...>(t, f, s));
   }

   template<class Func, class... Args>
   void Connect(Event<Args...> &s, Func func)
   {
      m_v.push_back(new ConcreteDelegate<Delegate, Args...>(func, s));
   }

private:
   Delegate(const Delegate&);
   void operator=(const Delegate&);

   std::vector<BaseDelegate*> m_v;
};

/*******************************
 Example Usage

class AppEvents
{
    Event<bool> activated;
    Event<int> keyDown;
};

class Application
{
public:
    LRESULT wndProc(UINT msg, WPARAM wParam, LPARAM lParam);

private:
    AppEvents events;
};

LRESULT Application::wndProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
        case WM_ACTIVATE: events.activated(static_cast<bool>(wParam)); return 0;
        case WM_KEYDOWN : if(!(lParam & 0x40000000)) events.keyDown(wParam); return 0;

        case WM_LBUTTONDOWN: events.mouseDown(Vec2(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)), VK_LBUTTON); return 0;
    }

    return DefWindowProc(hw, msg, wParam, lParam);
}


class Player : public GameItem
{
public:
   Player(AppEvents &events, const Vec3 &pos);

private:
   void appActivated(bool state){ }
   void keyDown(int key){  }

   Delegate delegate;
};

Player::Player(AppEvents &events, const Vec3 &pos) : pos(pos)
{
   delegate.Connect(this, &Player::appActivated, events.activated);
   delegate.Connect(this, &Player::keyDown, events.keyDown);
}

Player *Application::createPlayer(const Vec3 &pos)
{
   return new Player(events, pos);
}

*/
