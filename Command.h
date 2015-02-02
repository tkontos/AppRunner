#pragma once

struct Command
{
   enum class Type { None, Sep, App, File};
   Type           type = Type::None;
   std::wstring   name;
   std::wstring   note;
   std::wstring   command;
   std::wstring   args;

   Command() { }
   Command(const wchar_t *name, const wchar_t *cmd);

   const bool IsValid() const;

   bool RunCommand() const;
   HICON GetIcon() const;
};
