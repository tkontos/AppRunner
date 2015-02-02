#pragma once

//#include "DsCommon.h"
////////////////////////////////////////////////////////////////////

namespace BaseDetails
{
   template<typename ...BaseOther>
   class BaseObjImpl;

   template<typename Base>
   class BaseObjImpl<typename Base> : public Base
   {
   protected:
      HRESULT DoQuery(REFIID riid, void **ppvObject)
      {
         if (ppvObject == nullptr)
            return E_POINTER;

         if (QueryImpl<Base>(riid, ppvObject))
            return S_OK;

         if (QueryImpl<IUnknown>(riid, ppvObject))
            return S_OK;

         *ppvObject = 0;
         return E_NOINTERFACE;
      }
   private:
      template<typename T>
      bool QueryImpl( REFIID riid, void **ppvObject)
      {
         if (riid != __uuidof(T))
            return false;

         T *pb = static_cast<T *>(this);
         pb->AddRef();
         *ppvObject = pb;

         return true;
      }
   };

   template<typename Base, typename ...BaseOther>
   class BaseObjImpl<Base, BaseOther...> : public Base, public BaseObjImpl<BaseOther...>
   {
   protected:
      HRESULT DoQuery(REFIID riid, void **ppvObject)
      {
         if (riid == __uuidof(Base))
         {
            Base *pb = static_cast<Base *>(this);
            pb->AddRef();
            *ppvObject = pb;
            return S_OK;
         }

         return BaseObjImpl<BaseOther...>::DoQuery(riid, ppvObject);
      }
   };
}

template<typename ...Base>
class StackBaseObjT
   : public BaseDetails::BaseObjImpl< Base... >
{
public:
   virtual ~StackBaseObjT()
   {
   }
   virtual ULONG  STDMETHODCALLTYPE AddRef() override
   {
      return 1;
   }
   virtual ULONG  STDMETHODCALLTYPE Release() override
   {
      return 1;
   }
   virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) override
   {
      if (ppvObject == 0)
         return E_POINTER;

      *ppvObject = 0;
      return DoQuery(riid, ppvObject);
   }
};


template<typename ...Base>
class BaseObjT
   : public StackBaseObjT< Base...>
{
public:
   virtual ULONG  STDMETHODCALLTYPE AddRef() override
   {
      return InterlockedIncrement(&m_dwRefCount);
   }
   virtual ULONG  STDMETHODCALLTYPE Release() override
   {
      LONG l = InterlockedDecrement(&m_dwRefCount);
      if (l == 0)
         delete this;
      return l;
   }

protected:
   LONG     GetRefCount() const
   {
      return m_dwRefCount;
   }

private:
   LONG        m_dwRefCount = 0;
};
