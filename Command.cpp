#include "stdafx.h"
#include "resource.h"
#include "Command.h"

Command::Command(const wchar_t *name, const wchar_t *pszArgs)
   : name(name), note(pszArgs)
{
   std::wstring s = pszArgs;
   boost::algorithm::trim(s);
   size_t n = s.find(L".exe ");
   if (n != std::string::npos || boost::iends_with(s, L".exe"))
   {
      type = Type::App;
      if (n != std::string::npos)
      {
         n += 4;
         command = s.substr(0, n);
         args = s.substr(n);
      }
      else
      {
         command = s;
      }
   }
   else
   {
      type = Type::File;
      args = pszArgs;
   }
}

const bool Command::IsValid() const
{
   std::wstring file;

   switch (type)
   {
   case Command::Type::None:
      break;
   case Command::Type::Sep:
      break;
   case Command::Type::App:
      file = command;
      break;
   case Command::Type::File:
      file = args;
      break;

   default:
      break;
   }

   if (file.size() == 0)
      return true;

   boost::algorithm::trim_if(file, [](wchar_t x) { return isspace(x) || x == '"'; });
   DWORD a = GetFileAttributes(file.c_str());
   return a != INVALID_FILE_ATTRIBUTES;
}

bool Command::RunCommand() const
{
   BOOL bOK = FALSE;
   switch (type)
   {
   case Command::Type::None:
      break;
   case Command::Type::Sep:
      break;
   case Command::Type::App:
      bOK = ShellError(ShellExecute(NULL, NULL, command.c_str(), args.c_str(), NULL, SW_SHOWNORMAL), command.c_str());
      break;
   case Command::Type::File:
      bOK = ShellError(ShellExecute(NULL, _T("open"), args.c_str(), NULL, NULL, SW_SHOWNORMAL), args.c_str());
      break;
   default:
      break;
   }

   if (bOK)
      dsEvents.notif.commandFired();

   return true;
}

HICON Command::GetIcon() const
{
   if (!IsValid())
   {
      return LoadIcon(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDI_REDX));
   }

   if (type == Type::File)
   {
      SHFILEINFO sfi = { 0 };
      if (SHGetFileInfo(args.c_str(), -1, &sfi, sizeof(sfi), SHGFI_ICON) != 0)
         return sfi.hIcon;
   }
   else if (type == Type::App)
   {
      HICON icon;
      ExtractIconEx(command.c_str(), 0, &icon, nullptr, 1);
      return icon;
   }

   return 0;
}
