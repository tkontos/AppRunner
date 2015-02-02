#include "stdafx.h"
#include "SimpleTabCtrl.h"

#define SIMPLETABCTRL_IMPL
#include "SimpleTabCtrlVert.h"

std::unique_ptr<CSimpleTabCtrl> CSimpleTabCtrl::CreateIntance(HWND hParentWnd, int idc, std::function<bool(unsigned ndx)> func)
{
   //return std::make_unique<CButtonViewGroupStd>(hParentWnd, idc, func);
   return std::make_unique<CSimpleTabCtrlVert>(hParentWnd, idc, func);
}
