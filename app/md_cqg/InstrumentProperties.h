// InstrumentProperties.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "znet/ZNet.h"

// CInstrumentPropertiesApp:
// See InstrumentProperties.cpp for the implementation of this class
class CInstrumentPropertiesApp : public CWinApp
{
public:

	virtual BOOL InitInstance();
   virtual BOOL ExitInstance();
};

extern CInstrumentPropertiesApp theApp;