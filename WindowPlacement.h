////////////////////////////////////////////////////////////////
// CWindowPlacement Copyright 1996 Microsoft Systems Journal.
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.

////////////////
// CWindowPlacement reads and writes WINDOWPLACEMENT 
// from/to application profile and CArchive.
//

#include <windows.h>

struct CWindowPlacement
{
   CWindowPlacement(HWND hWnd)
      : m_hWnd( hWnd)
   {
      m_wp.length = sizeof(WINDOWPLACEMENT);
   }

   const TCHAR *Save   ( );
   BOOL         Restore( const TCHAR *psz);

private:
   WINDOWPLACEMENT   m_wp;
   HWND              m_hWnd;
   TCHAR             m_szBuf[256];

   bool     LoadFromString( const TCHAR *psz);
   bool     SaveToString  ( );
   bool     IsPlacementValid();
};
