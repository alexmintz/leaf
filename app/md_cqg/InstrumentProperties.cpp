// InstrumentProperties.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "InstrumentProperties.h"
#include "InstrumentPropertiesDlg.h"
#include "leaf/LFMarketState.h"


class CTEMPORALModule : public ATL::CComModule
{
};
// This instance is auxiliary. It will initialize all COM required
// modules in its constructor.
static CTEMPORALModule atlTemp;


// The one and only CInstrumentPropertiesApp object
CInstrumentPropertiesApp theApp;


// CInstrumentPropertiesApp initialization


BOOL CInstrumentPropertiesApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

   // Initializes the COM library
   CoInitialize(NULL);

	CWinApp::InitInstance();

	AfxEnableControlContainer();
//ZNET
	try {
		Z::init(std::string(CT2A(GetCommandLine())));

		LFMarketState::instance()->open();
	}
	catch(Z::Exception& e) 
	{
		Z::report_alert("%s", e.get_error_msg());
//		AfxMessageBox(CA2T(e.get_error_msg()), MB_ICONSTOP);
		return FALSE;
	}
//ZNET
   {
      CInstrumentPropertiesDlg dlg;
	   m_pMainWnd = &dlg;
      dlg.DoModal();
   }

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CInstrumentPropertiesApp::ExitInstance()
{
   // Close the COM library and unload all DLLs
   CoUninitialize();
	LFMarketState::instance()->close();
	Z::fini();

   return CWinApp::ExitInstance();
}