// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER             // Allow use of features specific to Windows 95 and Windows NT 4 or later.
#define WINVER 0x0501      // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINNT    // Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0501   // Change this to the appropriate value to target Windows 98 and Windows 2000 or later.
#endif

#ifndef _WIN32_WINDOWS        // Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0501 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE          // Allow use of features specific to IE 4.0 or later.
#define _WIN32_IE 0x0500   // Change this to the appropriate value to target IE 5.0 or later.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes

#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#define NOMINMAX

#include <atlbase.h>
#include <atlcom.h>		// IDispEventImpl
#include <iomanip>

#pragma message ("Please make sure that the path of CQGCEL-4_0.dll on your system corresponds to the one given in stdafx.h file.")
#ifdef LEAF_CQG_IN_X86
#import "C:\Program Files (x86)\CQG\CQGNET\Bin\CQGCEL-4_0.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search
#else
#import "C:\Program Files\CQG\CQGNET\Bin\CQGCEL-4_0.dll" raw_interfaces_only, raw_native_types, no_namespace, named_guids, auto_search
#endif
