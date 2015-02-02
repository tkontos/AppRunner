#pragma once

#include "WindowAnimate.h"
#include "SimpleTabCtrl.h"
#include "XmlFile.h"
#include "View.h"

class CViewContainer
   : public CWindowImpl<CViewContainer>
   , public CIdleHandler
{
   typedef CWindowImpl<CViewContainer> BaseClass;

public:
   CViewContainer(CXmlFile &xml)
      : m_xml(xml) { }

   ~CViewContainer();

   void ShowWindow(unsigned ndx);
   BOOL CreateContainer(CSimpleTabCtrl &bvg, HWND hWndParent, UINT nID);


protected:
   void OnSize(UINT nType, CSize size);

   BEGIN_MSG_MAP(CViewContainer)
      MSG_WM_SIZE(OnSize)
   END_MSG_MAP()

   virtual BOOL OnIdle() override;

private:
   typedef std::shared_ptr<CView>   ViewPtr;

   CXmlFile                     &m_xml;
   unsigned                      m_curWndNdx = 0;
   unsigned                      m_prevWndNdx = 0;
   std::vector<ViewPtr>          m_vdl;
   CWindowAnimate                m_wa;

   void  AddView(CSimpleTabCtrl &bvg, const wchar_t *name, const CommandPage &p)
   {
      CRect rc;
      GetClientRect(&rc);

      auto v = std::make_shared<CView>(p);
      v->Create(*this, rc);
      v->MoveWindow(&rc);

      m_vdl.push_back(v);

      USES_CONVERSION_EX;
      bvg.AddButton(W2CA_EX_DEF(name), nullptr);
   }

};
