////////////////////////////////////////////////////////////////
// CWindowPlacement Copyright 1996 Microsoft Systems Journal.
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.

#include "stdafx.h"
#include "WindowPlacement.h"

#include <tchar.h>

const TCHAR FormatWP[] = _T("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d");
const int NFIELDS = 10;


//////////////////

BOOL CWindowPlacement::Restore( const TCHAR *psz)
{
   if (!LoadFromString(psz))
      return FALSE;


   if (!IsPlacementValid())
      return FALSE;

   // Only restore if window intersects the screen.
//    CRect rcTemp, rcScreen(0,0,GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
//    if (!::IntersectRect(&rcTemp, &rcNormalPosition, &rcScreen))
//       return FALSE;

   return ::SetWindowPlacement(m_hWnd, &m_wp);
}

const TCHAR *CWindowPlacement::Save()
{
   m_szBuf[0] = 0;
   if (::GetWindowPlacement(m_hWnd, &m_wp))
      SaveToString();
   return m_szBuf;
}

bool CWindowPlacement::LoadFromString( const TCHAR *psz )
{
   return _stscanf_s(psz, FormatWP,
      &m_wp.showCmd,
      &m_wp.flags,
      &m_wp.ptMinPosition.x,
      &m_wp.ptMinPosition.y,
      &m_wp.ptMaxPosition.x,
      &m_wp.ptMaxPosition.y,
      &m_wp.rcNormalPosition.left,
      &m_wp.rcNormalPosition.right,
      &m_wp.rcNormalPosition.top,
      &m_wp.rcNormalPosition.bottom) == NFIELDS;
}

bool CWindowPlacement::SaveToString()
{
   _stprintf_s( m_szBuf, FormatWP,
      m_wp.showCmd,
      m_wp.flags,
      m_wp.ptMinPosition.x,
      m_wp.ptMinPosition.y,
      m_wp.ptMaxPosition.x,
      m_wp.ptMaxPosition.y,
      m_wp.rcNormalPosition.left,
      m_wp.rcNormalPosition.right,
      m_wp.rcNormalPosition.top,
      m_wp.rcNormalPosition.bottom);

   return true;
}


BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
   std::vector<CRect> *mrcl = reinterpret_cast<std::vector<CRect> *>(dwData);
   mrcl->push_back(*lprcMonitor);
   return TRUE;
}

bool CWindowPlacement::IsPlacementValid()
{
   std::vector<CRect> mrcl;
   if (!EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&mrcl))
      return false;
   
   for (auto &r : mrcl)
   {
      CRect rTemp;
      if (::IntersectRect(&rTemp, &m_wp.rcNormalPosition, &r))
         return true;

   }

   return false;
}