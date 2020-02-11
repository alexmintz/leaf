// OrderPlacing.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "OrderPlacing.h"
#include "OrderPlacingDlg.h"

class CTEMPORALModule : public ATL::CComModule
{
};

namespace
{
   // This instance is auxiliary. It will initialize all COM required
   // modules in its constructor.
   CTEMPORALModule atlTemp;
};

// The one and only COrderPlacingApp object
COrderPlacingApp theApp;

// COrderPlacingApp initialization
BOOL COrderPlacingApp::InitInstance()
{
   InitCommonControls();
   CoInitialize(NULL);
   CWinApp::InitInstance();

//ZNET
	try {

		Z::init(std::string(CT2A(GetCommandLine())));

	}
	catch(Z::Exception& e) 
	{
		Z::report_alert("%s", e.get_error_msg());
//		AfxMessageBox(ATL::CA2T(e.get_error_msg()), MB_ICONSTOP);
		return FALSE;
	}
//ZNET

   COrderPlacingDlg dlg;
   m_pMainWnd = &dlg;
   dlg.DoModal();

   return FALSE;
}

BOOL COrderPlacingApp::ExitInstance()
{
   CoUninitialize();

	Z::report_debug("---finilizing");
	Z::fini();

   return CWinApp::ExitInstance();
}
