// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//  are changed infrequently
//

#pragma once

// Change these values to use different versions
#define WINVER          0x0600
#define _WIN32_WINNT    0x0601
#define _WIN32_IE       0x0601
#define _RICHEDIT_VER   0x0200

#define _WTL_NO_WTYPES
#define _WTL_NO_CSTRING
#define _WTYPES_NS
#define	_ATL_USE_DDX_FLOAT

#include <atlbase.h>
#include <atlwin.h>
#include <atltypes.h>
#include <atlstr.h>
#include <atlapp.h>

extern CAppModule _Module;

#include <atlmisc.h>
#include <atlframe.h>
#include <atltheme.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include <atlimage.h>
#include <atlddx.h>
#include <atlcrack.h>
#include <ATLComTime.h>

#include <shlwapi.h>
#include <xmllite.h>

#include <functional>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <algorithm>

// boost includes
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <Boost/format.hpp>

// Local includes
#include "EventDelegate.h"
#include "AnimationHelper.h"
#include "Globals.h"
#include "UniqueVector.h"

#include "ModuleVer.h"

BOOL ShellError(HINSTANCE hInst, const TCHAR *psz);

namespace std
{
   template<typename T>
   using Iterator = typename T::iterator;

   template<typename Cont, typename Pred>
   inline Iterator<Cont> find_if(Cont &c, Pred pred)
   {
      return find_if(begin(c), end(c), pred);
   }

   template<typename Cont, typename Type>
   inline Iterator<Cont> find(Cont &c, const Type &d)
   {
      return find(begin(c), end(c), d);
   }
}
