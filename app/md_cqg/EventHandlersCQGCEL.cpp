// EventHandlersCQGCEl.cpp : implementation file
//

#include "stdafx.h"
#include "Common.h"
#include "InstrumentProperties.h"
#include "InstrumentPropertiesDlg.h"
#include "QFormulasDlg.h"
#include "znet/ZNet.h"
#include "znet/TCToken.h"
#include "znet/TCHelper.h"
#include "znet/TCSettings.h"
#include "leaf/LFMarketData.h"

STDMETHODIMP CInstrumentPropertiesDlg::OnDataError(
                                             LPDISPATCH obj, 
                                             BSTR errorDescription)
{
   CString errMsg(_T("Data error: "));
   errMsg += ATL::CW2T(errorDescription);

   Z::report_alert("%s", ATL::CT2A(errMsg));
//   AfxMessageBox(errMsg);

   return S_OK;
}

STDMETHODIMP CInstrumentPropertiesDlg::OnCELStarted()
{
   // Disable controls until data connection goes up
   EnableControls(FALSE);
   Z::report_info("CQGCEL started");

   return S_OK;
}

STDMETHODIMP CInstrumentPropertiesDlg::OnDataConnectionStatusChanged(
                                             eConnectionStatus newStatus)
{
   try
   {
	   CString statusMsg(_T("Data connection : "));
	   statusMsg += ConnectionStatusToString(newStatus);

	   m_lbDataConnection.SetWindowText(statusMsg);
	
	   Z::report_info("%s", ATL::CT2A(statusMsg));

	   EnableControls(TRUE); 

	   if(newStatus == csConnectionUp)
	   {
			TCDate dt = TCDateTime::now().get_date();
			TCJulianDate jd(dt);	
			
			jd -= 2;
			TCDate dt_back = jd.get_TCDate();
			jd += 4;
			TCDate dt_forth = jd.get_TCDate();

			std::vector<char> sep;
			sep.push_back(':');
			sep.push_back(' ');
			sep.push_back(',');
			std::string securities = TCSettings::get("DataFeed:securities", "QO, CLE, NGE");
			std::vector<std::string> und;
			tc_token_vec(securities, und, sep);
			

			for (size_t i = 0; i < und.size(); ++i)
			{
				try
				{
					HRESULT hr = m_spCQGCEL->NewInstrument(ATL::CA2W(LFSecurityMaster::instance()->get_instr(und[i], dt_back).c_str()));
					AssertCOMError(m_spCQGCEL, hr);
				}tc_catch_all;
			}

			if (dt_back != dt_forth)
			{
				for (size_t i = 0; i < und.size(); ++i)
				{
					try
					{
						HRESULT hr = m_spCQGCEL->NewInstrument(ATL::CA2W(LFSecurityMaster::instance()->get_instr(und[i], dt_forth).c_str()));
						AssertCOMError(m_spCQGCEL, hr);
					}tc_catch_all;
				}
			}
	   }
   } 
   catch (const std::runtime_error& e) 
   {
	   Z::report_alert("OnDataConnectionStatusChanged error: %s", e.what());
      return E_FAIL;
   }
   return S_OK;
}

STDMETHODIMP CInstrumentPropertiesDlg::OnInstrumentSubscribed(
                                             BSTR symbl, 
                                             ICQGInstrument* instrument)
{
   try
   {
      ATL::CComBSTR fullName;
      HRESULT hr = instrument->get_FullName(&fullName);
      AssertCOMError<ICQGInstrument>(instrument, hr);

      // If instrument is already subscribed do not add it to list
      if (m_lsInstruments.FindStringExact(0, ATL::CW2T(fullName)) != LB_ERR)
         return S_FALSE;

	  Z::report_info("Instrument subscribed : %s", ATL::CW2A(fullName));
      // Add name to list
      m_lsInstruments.AddString(ATL::CW2T(fullName));
      
      m_btRemoveAll.EnableWindow(TRUE);

	  ATL::CComPtr<ICQGQuotes> quotes;
	  hr = instrument->get_Quotes(&quotes);
	  AssertCOMError<ICQGInstrument>(instrument, hr);

	  PublishSettlement(instrument, quotes);
   }
   catch (const std::runtime_error& e) 
   {
	   Z::report_alert("OnInstrumentSubscribed error: %s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()));
      return E_FAIL;
   }

   return S_OK;
}

STDMETHODIMP CInstrumentPropertiesDlg::OnIncorrectSymbol(BSTR symbl)
{
   try
   {
      CString msg(_T("Incorrect symbol: "));
      msg.Append(ATL::CW2T(symbl));
	  Z::report_alert("%s", ATL::CT2A(msg));
//      AfxMessageBox(msg);
   }
   catch (const std::runtime_error& e) 
   {
	   Z::report_alert("OnIncorrectSymbol error: %s", e.what());
//      AfxMessageBox(ATL::CA2T(e.what()));
      return E_FAIL;
   }
   return S_OK;
}

STDMETHODIMP CInstrumentPropertiesDlg::OnInstrumentChanged(
                                             ICQGInstrument* instrument, 
                                             ICQGQuotes* quotes, 
                                             ICQGInstrumentProperties* props)
{
   try
   {
	   PublishTrades(instrument, quotes);
	   PublishSettlement(instrument, quotes);

      ATL::CComBSTR fullname;
      HRESULT hr = instrument->get_FullName(&fullname);
      AssertCOMError<ICQGInstrument>(instrument, hr);

      if (fullname != GetSelectedFullName())
      {
         // Not selected instrument was changed
         return S_OK;
      }

      //UpdateQuotes(instrument, quotes);
      UpdateProperties(instrument, props);
   }
   catch (const std::runtime_error& e) 
   {
	   Z::report_alert("OnInstrumentChanged error: %s", e.what());
//      AfxMessageBox(ATL::CA2T(e.what()));
      return E_FAIL;
   }
   return S_OK;
}

STDMETHODIMP CInstrumentPropertiesDlg::OnInstrumentDOMChanged(
                                             ICQGInstrument* instrument, 
                                             ICQGDOMQuotes* prevAsks, 
                                             ICQGDOMQuotes* prevBids)
{
   try
   {
		PublishDOM(instrument);
    
		ATL::CComBSTR fullname;
      HRESULT hr = instrument->get_FullName(&fullname);
      AssertCOMError<ICQGInstrument>(instrument, hr);
      if (fullname != GetSelectedFullName())
      {
         // Not selected instrument was changed
         return S_OK;
      }

      //UpdateDOM(instrument);
   }
   catch (const std::runtime_error& e) 
   {
	   Z::report_alert("OnInstrumentDOMChanged error: %s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()));
      return E_FAIL;
   }
   return S_OK;
}

STDMETHODIMP CQFormulasDlg::OnQFormulaDefinitionsResolved(
                              ICQGQFormulaDefinitions* cqg_qformula_definitions, 
                              ICQGError* cqg_error)
{
   try
   {
      // Raise error, when any error occurred during request
      VARIANT_BOOL isValid;
      HRESULT hr = m_spCQGCEL->IsValid(ATL::CComVariant(cqg_error), &isValid);
      AssertCOMError(m_spCQGCEL, hr);

      if (isValid == VARIANT_TRUE)
      {
         ATL::CComBSTR errDescription;
         hr = cqg_error->get_Description(&errDescription);
         AssertCOMError<ICQGError>(cqg_error, hr);

		 Z::report_alert("%s", ATL::CW2A(errDescription));
//		 AfxMessageBox(ATL::CW2T(errDescription), MB_ICONEXCLAMATION);

         return S_OK;
      }

      m_spQFormulaDefinitions = cqg_qformula_definitions;

      // Fill up new data
      ClearAllData();

      long count;
      hr = cqg_qformula_definitions->get_Count(&count);
      AssertCOMError<ICQGQFormulaDefinitions>(cqg_qformula_definitions, hr);

      if (count > 0)
      {
         for (long i = 0; i < count; ++i)
         {
            ATL::CComPtr<ICQGQFormulaDefinition> spQFormulaDefinition;
            hr = cqg_qformula_definitions->get_Item(i, &spQFormulaDefinition);
            AssertCOMError<ICQGQFormulaDefinitions>(cqg_qformula_definitions, hr);

            ATL::CComBSTR name;
            hr = spQFormulaDefinition->get_Name(&name);
            AssertCOMError(spQFormulaDefinition, hr);

            const int row = m_cmbDefinition.AddString(ATL::CW2T(name));
            m_cmbDefinition.SetItemData(row, i);
         }

         // Select first definition
         m_cmbDefinition.SetCurSel(0);
         OnCbnSelchangeCmbDefinition();
      }
   }
   catch (const std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   return S_OK;
}