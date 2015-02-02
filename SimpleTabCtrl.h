#pragma once

class CView;

class CSimpleTabCtrl
{
public:
   virtual ~CSimpleTabCtrl() { }

   static std::unique_ptr<CSimpleTabCtrl> CreateIntance(HWND hParentWnd, int idc, std::function<bool(unsigned ndx)> func);

   virtual void  AddButton(const char *psz, CView *pwv) = 0;
   virtual void  EnableView(bool b) = 0;
   virtual void  SelectItem(size_t ndx) = 0;
};
