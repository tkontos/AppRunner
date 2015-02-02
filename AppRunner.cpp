// AppRunner.cpp : main source file for AppRunner.exe
//

#include "stdafx.h"

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"

CAppModule _Module;

static int Run(CXmlFile &xml, int nCmdShow);
static std::unique_ptr<CXmlFile> GetXmlFile();

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
   HRESULT hRes = ::CoInitialize(NULL);
   ATLASSERT(SUCCEEDED(hRes));

   // this resolves ATL window thunking problem when Microsoft Layer for Unicode (MSLU) is used
   ::DefWindowProc(NULL, 0, 0, 0L);

   AtlInitCommonControls(ICC_BAR_CLASSES);	// add flags to support other controls

   hRes = _Module.Init(NULL, hInstance);
   ATLASSERT(SUCCEEDED(hRes));

   int nRet = -1;
   try
   {
      auto xml = GetXmlFile();
      nRet = Run(*xml, nCmdShow);
   }
   catch (std::exception &e)
   {
      MessageBoxA(NULL, e.what(), "Error", MB_OK | MB_ICONSTOP);
   }

   _Module.Term();
   ::CoUninitialize();

   return nRet;
}

static int Run(CXmlFile &xml, int nCmdShow)
{
   CMessageLoop theLoop;
   _Module.AddMessageLoop(&theLoop);

   CMainDlg dlgMain(xml);
   if (dlgMain.Create(NULL) == NULL)
   {
      ATLTRACE(_T("Main dialog creation failed!\n"));
      return 0;
   }

   dlgMain.ShowWindow(nCmdShow);

   int nRet = theLoop.Run();

   _Module.RemoveMessageLoop();
   return nRet;
}

static std::unique_ptr<CXmlFile> GetXmlFile()
{
   TCHAR path[MAX_PATH];
   if (__argc == 2)
   {
      _tcscpy_s(path, __targv[1]);
   }
   else
   {
      ::GetModuleFileName(NULL, path, MAX_PATH);
      PathRenameExtension(path, _T(".xml"));
   }


   return std::make_unique<CXmlFile>(path);
}
