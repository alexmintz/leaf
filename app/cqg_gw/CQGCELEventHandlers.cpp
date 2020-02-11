// CQGCELEventHandlers.cpp : implementation file
//

#include "stdafx.h"
#include <stdexcept>
#include <cassert>
#include "Common.h"
#include "OrderPlacing.h"
#include "OrderPlacingDlg.h"
#include "znet/ZNet.h"
#include "znet/TCToken.h"
#include "znet/TCHelper.h"
#include "znet/TCSettings.h"
#include "leaf/LFData.h"
STDMETHODIMP COrderPlacingDlg::OnDataError(
                                 LPDISPATCH obj,
                                 BSTR errorDescription)
{
   CString errMsg("Data error: ");
   errMsg += ATL::CW2T(errorDescription);
   Z::report_alert("%s", CT2A(errMsg));
//   AfxMessageBox(errMsg);

   return S_OK;
}

STDMETHODIMP COrderPlacingDlg::OnGWConnectionStatusChanged(
                                 eConnectionStatus newStatus)
{
   try
   {
		CString statusMsg(_T("GW connection : "));
		statusMsg += ConnectionStatusToString(newStatus);
		Z::report_info("%s", ATL::CT2A(statusMsg));

	   m_lblGWConnection.SetWindowText(_T("GW connection : ") + ConnectionStatusToString(newStatus));

      if (newStatus == csConnectionUp)
      {
         m_spCQGCEL->put_AccountSubscriptionLevel(aslAccountUpdatesAndOrders);
         m_GWConnectionUp = true;
      }
      else
      {
         m_GWConnectionUp = false;
      }
      
      EnableControls();
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   return S_OK;
}

STDMETHODIMP COrderPlacingDlg::OnDataConnectionStatusChanged(
                                 eConnectionStatus newStatus)
{
   try
   {
      CString status;

		CString statusMsg(_T("Data connection : "));
		statusMsg += ConnectionStatusToString(newStatus);
		Z::report_info("%s", ATL::CT2A(statusMsg));
      
      switch (newStatus)
      {
         case csConnectionDelayed:
            status = _T("delayed");
            break;
         case csConnectionDown:
            status = _T("down");
            break;
         case csConnectionUp:
		{
            status = _T("up");

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
			std::string securities = TCSettings::get("Gateway:securities", "QO, CLE, NGE");
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
			break;
         default:
            assert(!"Unexpected data connection status");
      }
      m_lblDataConnection.SetWindowText(_T("Data connection ") + status);

      m_DataConnectionUp = (newStatus == csConnectionUp);
   
      EnableControls();
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   return S_OK;
}

STDMETHODIMP COrderPlacingDlg::OnAccountChanged(
                                 eAccountChangeType change, 
                                 ICQGAccount* account, 
                                 ICQGPosition* position)
{
   try
   {
      if (change == actAccountsReloaded)
      {
         ReloadAccounts();
      }
   }
   catch (std::exception& e) 
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   return S_OK;
}

STDMETHODIMP COrderPlacingDlg::OnInstrumentSubscribed(
                                 BSTR symbol, 
                                 ICQGInstrument* instrument)
{
   try
   {
      ATL::CComBSTR fullName;
      HRESULT hr = instrument->get_FullName(&fullName);
      AssertCOMError<ICQGInstrument>(instrument, hr);

      // If instrument is already subscribed do not add it to the list
      if (m_lsInstruments.FindStringExact(0, ATL::CW2T(fullName)) == LB_ERR)
      {
         m_lsInstruments.AddString(ATL::CW2T(fullName));
      }
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   return S_OK;
}

STDMETHODIMP COrderPlacingDlg::OnInstrumentChanged(
                                 ICQGInstrument* instrument, 
                                 ICQGQuotes* quotes, 
                                 ICQGInstrumentProperties* props)
{
   try
   {
      // Get selected instrument
      ATL::CComPtr<ICQGInstrument> spSelInstrument = GetSelectedInstrument();

      if (spSelInstrument == NULL)
      {
         // No instrument is selected
         return S_OK;
      }

      // Get changed instrument's full name
      ATL::CComBSTR fullName;
      HRESULT hr = instrument->get_FullName(&fullName);
      AssertCOMError<ICQGInstrument>(instrument, hr);

      // Get selected instrument's full name
      ATL::CComBSTR selFullName;
      hr = spSelInstrument->get_FullName(&selFullName);
      AssertCOMError(spSelInstrument, hr);
   
      if (fullName == selFullName)
      {
         // Selected instrument was changed, show it's quotes
         ShowQuotes(instrument);
      }
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   return S_OK;
}

STDMETHODIMP COrderPlacingDlg::OnIncorrectSymbol(
                                 BSTR symbl)
{
   try
   {
      CString msg("Incorrect symbol: ");
      msg.Append(ATL::CW2T(symbl));
	  Z::report_alert("%s", CT2A(msg));
//	  AfxMessageBox(msg, MB_ICONASTERISK);
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   return S_OK;
}

STDMETHODIMP COrderPlacingDlg::OnOrderChanged(
                                 eChangeType change, 
                                 ICQGOrder* order, 
                                 ICQGOrderProperties* oldProperties, 
                                 ICQGFill* fill, 
                                 ICQGError* cqgerr)
{
   try
   {

      // Get order's account
      ATL::CComPtr<ICQGAccount> spAccount;
      HRESULT hr = order->get_Account(&spAccount);
      AssertCOMError<ICQGOrder>(order, hr);

      // Get order's account ID 
      long orderAccID;
      hr = spAccount->get_GWAccountID(&orderAccID);
      AssertCOMError(spAccount, hr);

	  // send to ZNET
	  SendOrder(order, change, fill, cqgerr, orderAccID);

      // Get selected account ID
      long selAccID = static_cast<long>(m_cmbAccounts.GetItemData(m_cmbAccounts.GetCurSel()));

      if (orderAccID != selAccID)
      {
         // Order's account is not selected
         return S_OK;
      }

      // Get order status
      eOrderStatus orderStatus;
      hr = order->get_GWStatus(&orderStatus);
      AssertCOMError<ICQGOrder>(order, hr);
	//------------------------------------
      // Get filter type
      int status = FilterTypeToOrderStatus(static_cast<eFilterType>(m_tabOrders.GetCurSel()));

      if (status == m_INVALID_INTEGER || status == orderStatus) 
      {
         // Add or update order in the list control
         ShowOrder(order, change, fill, cqgerr, orderAccID);
      }
      else if (GetOrderRow(order) > -1)
      {
         // If order exist in the list control, but status changed, remove it from the list
         ShowOrder(order, ctRemoved);
      }
   }
   catch (std::exception& e)
   {
	   Z::report_alert("%s", e.what());
//	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   return S_OK;
}
