#pragma once

#include "CommandPage.h"

namespace UnVecDetails   // Unique vector support
{
   using Key  = const wchar_t *;
   using Pair = std::pair<std::wstring, std::wstring>;

   template<typename Key, typename Data>
   struct KeyGen;

   template<> struct KeyGen<Key, Pair>
   {
      Key operator()(const Pair &d) const   {  return d.first.c_str(); }
   };

   template<> struct KeyGen<Key, CommandPage>
   {
      Key operator()(const CommandPage &d) const   { return d.name.c_str(); }
   };

   template<typename>
   struct iEqual
   {
      bool operator()(Key a, Key b) const  {
         return _tcsicmp(a, b) == 0;
      }
   };

   template<typename Data>
   using UniqueVector = unique_vector_t<Key, Data, KeyGen, iEqual>;
}

using UnVecDetails::UniqueVector;

class CXmlFile
{
public:
   CXmlFile(const TCHAR *fileName);

   ~CXmlFile();

   const std::vector<CommandPage> &GetPages() const
   {
      return m_pages.GetData();
   }

   // Implementation
private:
   struct Attrib
   {
      std::wstring name;
      std::wstring value;
   };

   typedef std::pair<std::wstring, std::wstring>   Pair;

   std::wstring                  m_computerName;
   UniqueVector<Pair>            m_config;
   UniqueVector<Pair>            m_defines;
   UniqueVector<CommandPage>     m_pages;

   static bool Equal(const wchar_t *p, const wchar_t *q)
   {
      return _tcsicmp(p, q) == 0;
   }
   static bool Equal(const std::wstring &p, const wchar_t *q)
   {
      return Equal(p.c_str(), q);
   }
   static bool Equal(const std::string &p, const wchar_t *q)
   {
      USES_CONVERSION_EX;
      return Equal(A2CW_EX_DEF(p.c_str()), q);
   }

   typedef std::vector<Attrib>   AttribList;

   void          LoadXmlFile    (const wchar_t *fileName);
   void          LoadConfig     (IXmlReader *reader, const TCHAR *endElement);
   void          LoadDefines    (IXmlReader *reader, const TCHAR *endElement);
   void          LoadCommandPage(IXmlReader *reader, const TCHAR *endElement);
   Command       GetCommandData (const wchar_t *name, const std::vector<Attrib> &a);
   void          ProcessMacros  (std::wstring &s) const;
   AttribList    GetAttrList    (IXmlReader *reader);

   template<typename Func>
   bool  LoadSection(IXmlReader *reader, const TCHAR *endElement, Func func)
   {
      auto attrList = GetAttrList(reader);
      auto it = std::find_if(attrList.begin(), attrList.end(), [](const Attrib &a) {
         return boost::algorithm::iequals(a.name, L"Computer");
      });

      bool bUseIt = it == attrList.end() || boost::algorithm::iequals(it->value, m_computerName);

      XmlNodeType nodeType = XmlNodeType_None;
      while (S_OK == reader->Read(&nodeType))
      {
         if (bUseIt && nodeType == XmlNodeType_Element)
         {
            const wchar_t *pszName = nullptr;
            COM_VERIFY(reader->GetLocalName(&pszName, NULL));

            auto attrList = GetAttrList(reader);
            func(pszName, attrList);
         }
         else if (nodeType == XmlNodeType_EndElement)
         {
            ATLASSERT(endElement != 0);
            const wchar_t *pszName = nullptr;
            COM_VERIFY(reader->GetLocalName(&pszName, NULL));
            if (Equal(pszName, endElement))
               break;
         }
      }

      return bUseIt;
   }

   template<typename T>
   T Config(const std::wstring &name, T defValue) const
   {
      T d = defValue;
      if (auto p = m_config[name.c_str()])
         d = boost::lexical_cast<T>(p->second);
      return d;
   }
};
