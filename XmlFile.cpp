#include "stdafx.h"
#include "XmlFile.h"

inline void COM_VERIFY(HRESULT hr)
{
   if (FAILED(hr))
      throw std::runtime_error("Errors occurred");
}

CXmlFile::CXmlFile(const TCHAR *fileName)
{
   wchar_t sz[512];
   DWORD dw = _countof(sz);
   GetComputerNameW(sz, &dw);
   m_computerName = sz;

   USES_CONVERSION_EX;
   LoadXmlFile(T2CW_EX_DEF(fileName));
}

CXmlFile::~CXmlFile()
{
}

void CXmlFile::LoadXmlFile(const wchar_t *fileName)
{
   USES_CONVERSION_EX;

   CComPtr<IStream> stream;
   if (FAILED(SHCreateStreamOnFile(fileName, STGM_READ | STGM_SHARE_DENY_NONE, &stream)))
      throw std::runtime_error( (boost::format("File '%1' not found") % W2CA_EX_DEF(fileName)).str());

   CComPtr<IXmlReader> reader;
   COM_VERIFY(CreateXmlReader(__uuidof(IXmlReader), reinterpret_cast<void**>(&reader), 0));
   COM_VERIFY(reader->SetInput(stream));

   XmlNodeType nodeType = XmlNodeType_None;
   while (S_OK == reader->Read(&nodeType))
   {
      if (nodeType == XmlNodeType_Element)
      {
         const wchar_t *pszName = nullptr;
         COM_VERIFY(reader->GetLocalName(&pszName, NULL));

         const wchar_t config[] = L"Config";
         if (Equal(pszName, config))
         {
            LoadConfig(reader, config);
            continue;
         }

         const wchar_t defines[] = L"Defines";
         if (Equal(pszName, defines))
         {
            LoadDefines(reader, defines);
            continue;
         }

         const wchar_t commandPage[] = L"CommandPage";
         if (Equal(pszName, commandPage))
         {
            LoadCommandPage(reader, commandPage);
            continue;
         }
      }
   }
}

void CXmlFile::LoadConfig(IXmlReader *reader, const TCHAR *endElement)
{
   LoadSection(reader, endElement, [this](const wchar_t *pszName, const AttribList &attrList) {
      if (attrList.size() != 2)
         throw std::runtime_error("Invalid Config Item");

      m_config.push_back(std::make_pair(attrList[0].value, attrList[1].value));
   });
}


void CXmlFile::LoadDefines(IXmlReader *reader, const TCHAR *endElement)
{
   LoadSection(reader, endElement, [this](const wchar_t *pszName, const AttribList &attrList) {
      if (attrList.size() != 2)
         throw std::runtime_error("Invalid Define Item");

      auto value = attrList[1].value;
      ProcessMacros(value);

      const std::wstring p(L"%");
      auto key = p + attrList[0].value + p;

      m_defines.push_back(std::make_pair(key, value));
   });
}

void CXmlFile::LoadCommandPage(IXmlReader *reader, const TCHAR *endElement)
{
   auto attrList = GetAttrList(reader);

   CommandPage page;
   for (auto &a : attrList)
   {
      if (boost::algorithm::iequals(a.name, L"name"))
         page.name = a.value;
   }

   if (page.name.size() == 0)
      throw std::runtime_error("Missing command page name");

   auto fn = [&page, this](const wchar_t *pszName, const AttribList &attrList) {
      auto btn = GetCommandData(pszName, attrList);
      page.buttons.push_back(btn);
   };

   if (!LoadSection(reader, endElement, fn))
      return;

   auto &btns = page.buttons;
   // Optionally, remove 'bad' commands
   if (Config<bool>(std::wstring(L"RemoveMissing"), true))
   {
      auto it = std::remove_if(btns.begin(), btns.end(), [](const CommandButton &btn) {
         return !btn.IsValid();
      });
      btns.erase(it, btns.end());
   }


   // Remove duplicate separators
   auto it = std::unique(btns.begin(), btns.end(), [](const CommandButton &a, const CommandButton &b) {
      return a.type == b.type && a.type == Command::Type::Sep;
   });
   btns.erase(it, btns.end());

   m_pages.push_back(page);
}

Command CXmlFile::GetCommandData(const wchar_t *name, const std::vector<Attrib> &a)
{
   std::wstring n, c;
   for (auto &a : a)
   {
      if (Equal(a.name, L"name"))
         n = a.value;
      else if (Equal(a.name, L"cmd") || Equal(a.name, L"args"))
         c = a.value;
   }

   if (Equal(name, L"Separator") || Equal(name, L"Space"))
   {
      Command cmd;
      cmd.type = Command::Type::Sep;
      cmd.name = n;
      return cmd;
   }

   if (n.size() == 0 || c.size() == 0)
      throw std::runtime_error("Invalid Button data");

   ProcessMacros(c);
   return Command(n.c_str(), c.c_str());
}

void CXmlFile::ProcessMacros(std::wstring &s) const
{
   for (auto &m : m_defines)
   {
      boost::replace_all(s, m.first, m.second);
   }

   TCHAR sz[2 * _MAX_PATH];
   ExpandEnvironmentStrings(s.c_str(), sz, _countof(sz));
   s = sz;

   boost::replace_all(s, L"/", L"\\");
   boost::replace_all(s, L"'", L"\"");
}

auto CXmlFile::GetAttrList(IXmlReader *reader) -> AttribList
{
   AttribList attrList;
   for (HRESULT hr = reader->MoveToFirstAttribute(); hr == S_OK; hr = reader->MoveToNextAttribute())
   {
      const wchar_t *pszName;
      COM_VERIFY(reader->GetLocalName(&pszName, NULL));
 
      const wchar_t *pszValue;
      COM_VERIFY(reader->GetValue(&pszValue, NULL));

      Attrib attr = { pszName, pszValue };
      attrList.push_back(attr);
   }
   return attrList;
}
