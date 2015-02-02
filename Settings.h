#pragma once

#include "rsettings.h"

class CSettings : public CRegSettings
{
public:
   static CSettings &Instance()
   {
      static CSettings s;
      return s;
   }

   DWORD       m_lastOpenedPage;
   BOOL        m_exitOnCommand;
   CString     m_winPlacement;

private:
   CSettings()
   {
      Init(HKEY_CURRENT_USER, _T("Software\\4N Systems\\AppRunner"));
      Load();
   }

   BEGIN_REG_MAP(CSettings)
      REG_ITEM_EX("LastOpenedPage" , m_lastOpenedPage, 0)
      REG_ITEM_EX("ExitOnCommand"  , m_exitOnCommand, 0)
      REG_ITEM_EX("WindowPlacement", m_winPlacement, _T(""))
   END_REG_MAP()
};

