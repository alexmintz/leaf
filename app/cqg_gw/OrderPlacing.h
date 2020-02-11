// OrderPlacing.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
   #error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"

// COrderPlacingApp:
// See OrderPlacing.cpp for the implementation of this class
//

class COrderPlacingApp : public CWinApp
{

public:
   virtual BOOL InitInstance();
   virtual BOOL ExitInstance();
};

extern COrderPlacingApp theApp;
