// stdafx.cpp : source file that includes just the standard includes
//	AppRunner.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#pragma comment(lib, "XmlLite.lib")

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

BOOL ShellError(HINSTANCE hInst, const TCHAR *psz)
{
   static struct
   {
      long err;
      const TCHAR *pszMsg;
   } errList[] =
   {
      ERROR_FILE_NOT_FOUND, _T("The specified file (%s) was not found."),
      ERROR_PATH_NOT_FOUND, _T("The specified path (%s) was not found."),
      ERROR_DDE_FAIL, _T("The Dynamic Data Exchange (DDE) transaction failed."),
      ERROR_NO_ASSOCIATION, _T("There is no application associated with the given file name extension."),
      ERROR_ACCESS_DENIED, _T("Access to the specified file (%s) is denied."),
      ERROR_DLL_NOT_FOUND, _T("One of the library files necessary to run the application can't be found."),
      ERROR_CANCELLED, _T("The function prompted the user for additional information, but the user canceled the request."),
      ERROR_NOT_ENOUGH_MEMORY, _T("There is not enough memory to perform the specified action."),
      ERROR_SHARING_VIOLATION, _T("A sharing violation occurred."),
   };

   long err = long(hInst);
   if (err > 32)
      return TRUE;

   for (int i = 0; i < _countof(errList); ++i)
   {
      if (errList[i].err == err)
      {
         TCHAR sz[_MAX_PATH + 100];
         _stprintf_s(sz, errList[i].pszMsg, psz);
         MessageBox(NULL, sz, _T("JumpListError"), MB_OK | MB_ICONSTOP);
         break;
      }
   }

   return FALSE;
}
