/*
Module : VersionInfo.CPP
Purpose: Implementation for a MFC class encapsulation of Window's "Version Infos"
Created: PJN / 10-04-2000
History: PJN / 07-07-2006 1. Updated copyright details
                          2. Updated the code to clean compile on VC 2005
                          3. Addition of CVERSIONINFO_EXT_CLASS and CVERSIONINFO_EXT_API macros to allow
                          the class to be easily added to an extension DLL.
                          4. Optimized CVersionInfo constructor code
                          5. Reviewed all TRACE statements for correctness
                          6. Updated the documentation to use the same style as the web site.
         PJN / 14-09-2008 1. Updated copyright details.
                          2. Code now compiles cleanly using Code Analysis (/analyze)
                          3. Updated code to compile correctly using _ATL_CSTRING_EXPLICIT_CONSTRUCTORS define
                          4. Updated sample app to clean compile on VC 2008
                          5. The code has now been updated to support VC 2005 or later only. 
                          6. Removed VC 6 style AppWizard comments from the code.
                          7. Reworked code to use ATL::CHeapPtr for required memory allocations
         PJN / 04-01-2015 1. Updated the code to clean compile in VC 2010 - VC 2013.
                          2. Updated copyright details.
                          3. Replaced all TRACE calls with ATLTRACE.

Copyright (c) 2000 - 2015 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/

//////////////// Includes /////////////////////////////////////////////////////

#include "stdafx.h"
#include "VersionInfo.h"


//////////////// Macros / Locals //////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//Automatically pull in the win32 version Library
#pragma comment(lib, "version.lib")


//////////////// Implementation ///////////////////////////////////////////////

CVersionInfo::CVersionInfo() : m_pffi(NULL),
                               m_wLangID(0),
                               m_wCharset(1252), //Use the ANSI code page as a default
                               m_pTranslations(NULL),
                               m_nTranslations(0)
{
	TCHAR fname[_MAX_PATH + 1];
	if (!GetModuleFileName(NULL, fname, _MAX_PATH))
		return;
	Load(fname);
}

CVersionInfo::~CVersionInfo()
{
  Unload();
}

void CVersionInfo::Unload()
{
  m_pffi = NULL;
  if (m_VerData != NULL)
    m_VerData.Free();
  m_wLangID = 0;
  m_wCharset = 1252; //Use the ANSI code page as a default
  m_pTranslations = NULL;
  m_nTranslations = 0;
}

BOOL CVersionInfo::Load(LPCTSTR szFileName)
{
  //Free up any previous memory lying around
  Unload();

  BOOL bSuccess = FALSE;
  DWORD dwSize = GetFileVersionInfoSize(szFileName, NULL);
  if (dwSize)
  {
    //Allocate some memory to hold the version info data
    ATLASSERT(m_VerData == NULL);
    if (!m_VerData.Allocate(dwSize))
    {
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }
    if (GetFileVersionInfo(szFileName, 0, dwSize, m_VerData))
    {
      //Get the fixed size version info data
      UINT nLen = 0;
      if (VerQueryValue(m_VerData, _T("\\"), reinterpret_cast<LPVOID*>(&m_pffi), &nLen))
      {
        //Retrieve the Lang ID and Character set ID
        if (VerQueryValue(m_VerData, _T("\\VarFileInfo\\Translation"), reinterpret_cast<LPVOID*>(&m_pTranslations), &nLen) && (nLen >= sizeof(TRANSLATION))) 
        {
          m_nTranslations = nLen / sizeof(TRANSLATION);
          m_wLangID = m_pTranslations[0].m_wLangID;
          m_wCharset = m_pTranslations[0].m_wCodePage;
        }
        bSuccess = TRUE;
      }
      else
        ATLTRACE(_T("CVersionInfo::Load, Failed to query file size version info for file %s, LastError:%u\n"), szFileName, ::GetLastError());
    }
    else
      ATLTRACE(_T("CVersionInfo::Load, Failed to read in version info for file %s, LastError:%u\n"), szFileName, ::GetLastError());
  }
  else
    ATLTRACE(_T("CVersionInfo::Load, Failed to get version info for file %s, LastError:%u\n"), szFileName, ::GetLastError());

  return bSuccess;
}

VS_FIXEDFILEINFO* CVersionInfo::GetFixedFileInfo()
{
  //Validate our parameters
  ATLASSERT(m_pffi != NULL);

  return m_pffi;
}

DWORD CVersionInfo::GetFileFlagsMask()
{
  //Validate our parameters
  ATLASSUME(m_pffi != NULL);

  return m_pffi->dwFileFlagsMask;
}

DWORD CVersionInfo::GetFileFlags()
{
  //Validate our parameters
  ATLASSUME(m_pffi != NULL);

  return m_pffi->dwFileFlags;
}

DWORD CVersionInfo::GetOS()
{
  //Validate our parameters
  ATLASSUME(m_pffi != NULL);

  return m_pffi->dwFileOS;
}

DWORD CVersionInfo::GetFileType()
{
  //Validate our parameters
  ATLASSUME(m_pffi != NULL);

  return m_pffi->dwFileType;
}

DWORD CVersionInfo::GetFileSubType()
{
  //Validate our parameters
  ATLASSUME(m_pffi != NULL);

  return m_pffi->dwFileSubtype;
}

FILETIME CVersionInfo::GetCreationTime()
{
  //Validate our parameters
  ATLASSUME(m_pffi != NULL);

  FILETIME CreationTime;
  CreationTime.dwHighDateTime = m_pffi->dwFileDateMS; 
  CreationTime.dwLowDateTime = m_pffi->dwFileDateLS; 
  return CreationTime;
}

unsigned __int64 CVersionInfo::GetFileVersion()
{
  //Validate our parameters
  ATLASSUME(m_pffi != NULL);

  unsigned __int64 nFileVersion = 0;
  nFileVersion = m_pffi->dwFileVersionLS;
  nFileVersion += ((static_cast<unsigned __int64>(m_pffi->dwFileVersionMS)) << 32);
  return nFileVersion;
}

unsigned __int64 CVersionInfo::GetProductVersion()
{
  //Validate our parameters
  ATLASSUME(m_pffi != NULL);

  unsigned __int64 nProductVersion = 0;
  nProductVersion = m_pffi->dwProductVersionLS;
  nProductVersion += ((static_cast<unsigned __int64>(m_pffi->dwProductVersionMS)) << 32);
  return nProductVersion;
}

CString CVersionInfo::GetValue(LPCTSTR pszKey)
{
  //Validate our parameters
  ATLASSUME(m_VerData != NULL);

  //What will be the return value from this function
  CString sVal;

  //Form the string to query with
  CString sQueryValue;
  sQueryValue.Format(_T("\\StringFileInfo\\%04x%04x\\%s"), m_wLangID, m_wCharset, pszKey);

  //Note that the definition for VerQueryValue in the VC 2005 Winver.h header file is incorrectly
  //defined as taking a non-const 2nd parameter, so to avoid this issue, lets get a non const pointer
  //to the "sQueryValue" buffer
  LPTSTR pszQueryValue = sQueryValue.GetBuffer(sQueryValue.GetLength());

  //Do the query
  LPTSTR pVal = NULL;
  UINT nLen = 0;
  if (VerQueryValue(m_VerData, pszQueryValue, reinterpret_cast<LPVOID*>(&pVal), &nLen)) 
    sVal = pVal;
    
  //Release the non-const buffer now that we are finished with it    
  sQueryValue.ReleaseBuffer();

  return sVal;
}

CString CVersionInfo::GetCompanyName()
{
  return GetValue(_T("CompanyName"));
}

CString CVersionInfo::GetFileDescription()
{
  return GetValue(_T("FileDescription"));
}

CString CVersionInfo::GetFileVersionAsString()
{
  return GetValue(_T("FileVersion"));
}

CString CVersionInfo::GetInternalName()
{
  return GetValue(_T("InternalName"));
}

CString CVersionInfo::GetLegalCopyright()
{
  return GetValue(_T("LegalCopyright"));
}

CString CVersionInfo::GetOriginalFilename()
{
  return GetValue(_T("OriginalFilename"));
}

CString CVersionInfo::GetProductName()
{
  return GetValue(_T("Productname"));
}

CString CVersionInfo::GetProductVersionAsString()
{
  return GetValue(_T("ProductVersion"));
}
  
int CVersionInfo::GetNumberOfTranslations()
{
  return m_nTranslations;
}

CString CVersionInfo::GetComments()
{
  return GetValue(_T("Comments"));
}

CString CVersionInfo::GetLegalTrademarks()
{
  return GetValue(_T("LegalTrademarks"));
}

CString CVersionInfo::GetPrivateBuild()
{
  return GetValue(_T("PrivateBuild"));
}

CString CVersionInfo::GetSpecialBuild()
{
  return GetValue(_T("SpecialBuild"));
}

CVersionInfo::TRANSLATION* CVersionInfo::GetTranslation(int nIndex)
{
  ATLASSERT(nIndex >= 0 && nIndex < m_nTranslations);
  ATLASSUME(m_pTranslations != NULL);

  return &m_pTranslations[nIndex];
}

void CVersionInfo::SetTranslation(int nIndex)
{
  TRANSLATION* pTranslation = GetTranslation(nIndex);
  ATLASSUME(pTranslation != NULL);

  m_wLangID = pTranslation->m_wLangID;
  m_wCharset = pTranslation->m_wCodePage;
}
