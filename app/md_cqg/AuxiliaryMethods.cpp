#include "stdafx.h"
#include "znet/ZNet.h"
#include "leaf/LFMarketState.h"
#include <cassert>
#include <vector>

#include "Common.h"
#include "InstrumentProperties.h"
#include "InstrumentPropertiesDlg.h"
#include "QFormulasDlg.h"

void CInstrumentPropertiesDlg::InitializeControls()
{
   OnDataConnectionStatusChanged(csConnectionDown);

//   ClearQuotes();
   InitializeProperties();
//   InitializeDOM();
   InitializeSessions();
}

void CInstrumentPropertiesDlg::InitializeProperties()
{
   m_lblInstrDescription.SetWindowText(m_N_A);
   
   m_lvProperties.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
   m_lvProperties.InsertColumn(0, _T("Name"), LVCFMT_LEFT , 115);
   m_lvProperties.InsertColumn(1, _T("Value"), LVCFMT_LEFT, 94);

   // Add empty rows
   for (int i = 0; i < m_stPropsCount; ++i)
   {
      m_lvProperties.InsertItem(0, _T(""));
      m_lvProperties.SetItemText(0, 1, m_N_A);
   }

   // Fill property names to list, it is assumed that all items
   // in eInstrumentProperty enum are sequential. 
   m_lvProperties.SetItemText(ipFullName, 0, _T("FullName"));
   m_lvProperties.SetItemText(ipDescription, 0, _T("Description"));
   m_lvProperties.SetItemText(ipInstrumentID, 0, _T("InstrumentID"));
   m_lvProperties.SetItemText(ipCurrency, 0, _T("Currency"));
   m_lvProperties.SetItemText(ipExpirationDate, 0, _T("ExpirationDate"));
   m_lvProperties.SetItemText(ipInitialMargin, 0, _T("InitialMargin"));
   m_lvProperties.SetItemText(ipMaintenanceMargin, 0, _T("MaintenanceMargin"));
   m_lvProperties.SetItemText(ipSessionMask, 0, _T("SessionMask"));
   m_lvProperties.SetItemText(ipCommodity, 0, _T("Commodity"));
   m_lvProperties.SetItemText(ipMonthChar, 0, _T("MonthChar"));
   m_lvProperties.SetItemText(ipCountry, 0, _T("Country"));
   m_lvProperties.SetItemText(ipMonth, 0, _T("Month"));
   m_lvProperties.SetItemText(ipStrike, 0, _T("Strike"));
   m_lvProperties.SetItemText(ipInstrumentType, 0, _T("InstrumentType"));
   m_lvProperties.SetItemText(ipYear, 0, _T("Year"));
   m_lvProperties.SetItemText(ipYearString, 0, _T("YearString"));
   m_lvProperties.SetItemText(ipTickSize, 0, _T("TickSize"));
   m_lvProperties.SetItemText(ipTickValue, 0, _T("TickValue"));
   m_lvProperties.SetItemText(ipSessionNumber, 0, _T("SessionNumber"));
   m_lvProperties.SetItemText(ipNetChange, 0, _T("NetChange"));
   m_lvProperties.SetItemText(ipDTE, 0, _T("DTE"));
//   m_lvProperties.SetItemText(ipYOI, 0, _T("YOI"));
   m_lvProperties.SetItemText(ipIsDownTick, 0, _T("IsDownTick"));
   m_lvProperties.SetItemText(ipIsUpTick, 0, _T("IsUpTick"));
   m_lvProperties.SetItemText(ipIsFastMarket, 0, _T("IsFastMarket"));
 //  m_lvProperties.SetItemText(ipTodayTotalVolume, 0, _T("TodayTotalVolume"));
   m_lvProperties.SetItemText(ipPreferredSessionName, 0, _T("PreferredSessionName"));
 //  m_lvProperties.SetItemText(ipYesterdayTotalVolume, 0, _T("YesterdayTotalVolume"));
   m_lvProperties.SetItemText(ipTodayCTotalVolume, 0, _T("TodayCTotalVolume"));
   m_lvProperties.SetItemText(ipYesterdayCTotalVolume, 0, _T("YesterdayCTotalVolume"));
   m_lvProperties.SetItemText(ipYCOI, 0, _T("YCOI"));
   m_lvProperties.SetItemText(ipExchangeID, 0, _T("ExchangeID"));
   m_lvProperties.SetItemText(ipExchangeAbbreviation, 0, _T("ExchangeAbbreviation"));
   m_lvProperties.SetItemText(ipScale, 0, _T("Scale"));
   m_lvProperties.SetItemText(ipStrikeScale, 0, _T("StrikeScale"));
   m_lvProperties.SetItemText(ipDecimalStrike, 0, _T("DecimalStrike"));
   m_lvProperties.SetItemText(ipDelayTime, 0, _T("DelayTime"));
   m_lvProperties.SetItemText(ipLeadingContractName, 0, _T("LeadingContractName"));
}
/*
void CInstrumentPropertiesDlg::InitializeDOM()
{
   m_lvDOM.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
   m_lvDOM.InsertColumn(0, _T("VolAsk"), LVCFMT_LEFT, 68);
   m_lvDOM.InsertColumn(1, _T("Price"), LVCFMT_LEFT, 70);
   m_lvDOM.InsertColumn(2, _T("VolBid"), LVCFMT_LEFT, 68);

   // Add empty rows
   for (int i = 0; i < 2 * m_stDOMCount + 2; ++i)
   {
      m_lvDOM.InsertItem(0, _T(""));
   }
}
*/
void CInstrumentPropertiesDlg::InitializeSessions()
{
   m_lvSessions.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
   m_lvSessions.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 80);
   m_lvSessions.InsertColumn(1, _T("Number"), LVCFMT_LEFT, 60);
   m_lvSessions.InsertColumn(2, _T("Open"), LVCFMT_LEFT, 40);
   m_lvSessions.InsertColumn(3, _T("ValidOpen"), LVCFMT_LEFT, 60);
   m_lvSessions.InsertColumn(4, _T("Close"), LVCFMT_LEFT, 40);
   m_lvSessions.InsertColumn(5, _T("ValidClose"), LVCFMT_LEFT, 60);
   m_lvSessions.InsertColumn(6, _T("Active Days"), LVCFMT_LEFT, 70);
   m_lvSessions.InsertColumn(7, _T("Primary"), LVCFMT_LEFT, 55);
}

void CInstrumentPropertiesDlg::InitializeCQGCEL()
{
   // create an instance of CQG API
   HRESULT hr = m_spCQGCEL.CoCreateInstance(__uuidof(CQGCEL), NULL, CLSCTX_INPROC_SERVER);
   if (FAILED(hr))
   {
      throw std::runtime_error("Unable to create CQGCEL object. "
                               "Please register it again and restart application.");
   }

   // Change configuration in order not to check Ready status, 
   // and let CQCCEL know that it is used from ATL client.
   ATL::CComPtr<ICQGAPIConfig> spConf;
   hr = m_spCQGCEL->get_APIConfiguration(&spConf);
   AssertCOMError<ICQGCEL>(m_spCQGCEL, hr);

   hr = spConf->put_ReadyStatusCheck(rscOff);
   AssertCOMError<ICQGAPIConfig>(spConf, hr);

   hr = spConf->put_TimeZoneCode(tzEastern);
   AssertCOMError<ICQGAPIConfig>(spConf, hr);

   hr = spConf->put_DefaultInstrumentSubscriptionLevel(dsQuotesAndDOM);
   AssertCOMError<ICQGAPIConfig>(spConf, hr);

   // Now advise the connection, to get events
   hr = ICQGCELDispEventImpl::DispEventAdvise(m_spCQGCEL);
   ATLASSERT(SUCCEEDED(hr));

   hr = m_spCQGCEL->Startup();
   AssertCOMError<ICQGCEL>(m_spCQGCEL, hr);
}

void CInstrumentPropertiesDlg::ClearAllData()
{
 //  ClearQuotes();
//   ClearDOM();
   ClearSessions();
   ClearProperties();
}

void CQFormulasDlg::ClearAllData()
{

   m_cmbDefinition.ResetContent();
   m_lblQFormulaNumber.SetWindowText(CInstrumentPropertiesDlg::m_N_A);
   m_edQFormulaExpression.SetWindowText(CInstrumentPropertiesDlg::m_N_A);
   m_qFormula = _T("");
   
}

const CString& CQFormulasDlg::GetQFormulaName() const
{
   return m_qFormula;
}
/*
void CInstrumentPropertiesDlg::ClearQuotes()
{
   // Clear quotes
   m_lbAskPrice.SetWindowText(m_N_A);
   m_lbAskVolume.SetWindowText(m_N_A);
   m_lbBidPrice.SetWindowText(m_N_A);
   m_lbBidVolume.SetWindowText(m_N_A);
   m_lbTradePrice.SetWindowText(m_N_A);
   m_lbTradeVolume.SetWindowText(m_N_A);
}
*/
void CInstrumentPropertiesDlg::ClearProperties()
{
   m_lblInstrDescription.SetWindowText(m_N_A);

   for (int i = 0; i < m_stPropsCount; ++i)
   {
      m_lvProperties.SetItemText(i, 1, m_N_A);
   }
}
/*
void CInstrumentPropertiesDlg::ClearDOM()
{
   for (int i = 0; i < m_stDOMCount; ++i)
   {
      m_lvDOM.SetItemText(m_stDOMCount - i, 0, _T(""));
      m_lvDOM.SetItemText(m_stDOMCount - i, 1, _T(""));
      m_lvDOM.SetItemText(m_stDOMCount + i + 1, 1, _T(""));
      m_lvDOM.SetItemText(m_stDOMCount + i + 1, 2, _T(""));
   }
}
*/
void CInstrumentPropertiesDlg::ClearSessions()
{
   m_lvSessions.DeleteAllItems();
}

void CInstrumentPropertiesDlg::ShowQuotes(const ATL::CComPtr<ICQGInstrument>& spInstrument)
{
   ATL::CComPtr<ICQGQuotes> spQuotes;
   HRESULT hr = spInstrument->get_Quotes(&spQuotes);
   AssertCOMError(spInstrument, hr);

   // Update Settlement
   ATL::CComPtr<ICQGQuote> spQuote;
   hr = spQuotes->get_Item(qtSettlement, &spQuote);
   if (SUCCEEDED(hr))
   {
	   double price;
	   hr = spQuote->get_Price(&price);
	   AssertCOMError(spQuote, hr);

	   Z::report_info("Show Quotes: today Settlement Price %f", price);
   }
   hr = spQuotes->get_Item(qtYesterdaySettlement, &spQuote);
   if (SUCCEEDED(hr))
   {
	   double price;
	   hr = spQuote->get_Price(&price);
	   AssertCOMError(spQuote, hr);

	   Z::report_info("Show Quotes: yesterday Settlement Price %f", price);
   }
   PublishSettlement(spInstrument, spQuotes);
}
/*
void CInstrumentPropertiesDlg::UpdateQuotes(
                                    const ATL::CComPtr<ICQGInstrument>& spInstrument,
                                    const ATL::CComPtr<ICQGQuotes>& spQuotes)
{	
   CString displayPrice, displayVolume;

   // Update Ask
   ATL::CComPtr<ICQGQuote> spQuote;

   HRESULT hr = spQuotes->get_Item(qtAsk, &spQuote);
   if (SUCCEEDED(hr))
   {
      GetQuoteDisplayValues(spInstrument, spQuote, displayPrice, displayVolume); 
      m_lbAskPrice.SetWindowText(displayPrice);
      m_lbAskVolume.SetWindowText(displayVolume);
      spQuote.Release();
   }

   // Update Trade
   hr = spQuotes->get_Item(qtTrade, &spQuote);
   if (SUCCEEDED(hr))
   {
      GetQuoteDisplayValues(spInstrument, spQuote, displayPrice, displayVolume); 
      m_lbTradePrice.SetWindowText(displayPrice);
      m_lbTradeVolume.SetWindowText(displayVolume);

      spQuote.Release();
   }

   // Update Bid
   hr = spQuotes->get_Item(qtBid, &spQuote);
   if (SUCCEEDED(hr))
   {
      GetQuoteDisplayValues(spInstrument, spQuote, displayPrice, displayVolume); 
      m_lbBidPrice.SetWindowText(displayPrice);
      m_lbBidVolume.SetWindowText(displayVolume);
   }
}
*/
void CInstrumentPropertiesDlg::PublishSettlement(
	const ATL::CComPtr<ICQGInstrument>& spInstrument,
	const ATL::CComPtr<ICQGQuotes>& spQuotes)
{
	ATL::CComPtr<ICQGQuote> spQuote;
	HRESULT hr = spQuotes->get_Item(qtSettlement, &spQuote);
	if (!SUCCEEDED(hr) || spQuote == NULL)
		return;

	auto_ptr<LFVWAPData> lfvwap(new LFVWAPData);
	ATL::CComBSTR fullname;
	hr = spInstrument->get_FullName(&fullname);
	AssertCOMError<ICQGInstrument>(spInstrument, hr);

	std::string key = ATL::CW2A(fullname);
	lfvwap->_instr_key = key;

	hr = spInstrument->get_InstrumentID(&lfvwap->_cqg_InstrumentID);
	AssertCOMError<ICQGInstrument>(spInstrument, hr);

	ATL::CComBSTR undername;
	hr = spInstrument->get_UnderlyingInstrumentName(&undername);
	AssertCOMError<ICQGInstrument>(spInstrument, hr);
	std::string und = ATL::CW2A(undername);

	VARIANT_BOOL isValid;

	hr = spQuote->get_IsValid(&isValid);
	AssertCOMError(spQuote, hr);

	if (isValid == VARIANT_TRUE)
	{
		hr = spQuote->get_Price(&lfvwap->_vwap);
		AssertCOMError(spQuote, hr);
	}

	hr = spQuotes->get_Item(qtYesterdaySettlement, &spQuote);
	if (!SUCCEEDED(hr) || spQuote == NULL)
		return;

	hr = spQuote->get_IsValid(&isValid);
	AssertCOMError(spQuote, hr);
	double y_settlement = 0;
	if (isValid == VARIANT_TRUE)
	{
		hr = spQuote->get_Price(&y_settlement);
		AssertCOMError(spQuote, hr);
	}
	Z::report_info("VWAP %s, yesterday's %f", lfvwap->as_string().c_str(), y_settlement);

	TCDateTime dt = TCDateTime::now();
	
	if (y_settlement != lfvwap->_vwap || dt.get_time() > 18) // todays's settle
		lfvwap->_t = TCTimestamp::stamp();
	else
	{	
		TCJulianDate jd(dt.get_date());
		while (!tc_b_date->is_business(jd, und)) --jd;
		dt = TCDateTime(jd.get_TCDate(), TCTime(18, 0, 0));
		lfvwap->_t = dt.get_ace_time();
	}

	LFMarketState::instance()->send(lfvwap.release());
	spQuote.Release();
}
void CInstrumentPropertiesDlg::PublishTrades(
                                    const ATL::CComPtr<ICQGInstrument>& spInstrument,
                                    const ATL::CComPtr<ICQGQuotes>& spQuotes)
{	
   ATL::CComPtr<ICQGQuote> spQuote;
   HRESULT hr = spQuotes->get_Item(qtTrade, &spQuote);
   if (!SUCCEEDED(hr) || spQuote == NULL)
	   return;

	auto_ptr<LFTradeData> lftrade(new LFTradeData);
	ATL::CComBSTR fullname;
	hr = spInstrument->get_FullName(&fullname);
	AssertCOMError<ICQGInstrument>(spInstrument, hr);
	std::string key = ATL::CW2A(fullname);
	lftrade->_instr_key = key;

	hr = spInstrument->get_InstrumentID(&lftrade->_cqg_InstrumentID);
	AssertCOMError<ICQGInstrument>(spInstrument, hr);

	VARIANT_BOOL isValid;

	hr = spQuote->get_IsValid(&isValid);
	AssertCOMError(spQuote, hr);

	if (isValid == VARIANT_TRUE)
	{
		double price;
		hr = spQuote->get_Price(&price);
		AssertCOMError(spQuote, hr);

		long volume;
		hr = spQuote->get_Volume(&volume);
		AssertCOMError(spQuote, hr);
		lftrade->_trade.reset(price, volume);
	}
	spQuote.Release();
	lftrade->_t = TCTimestamp::stamp();
//	Z::publish_event(lftrade.release(), key);
	LFMarketState::instance()->send(lftrade.release());

}

void CInstrumentPropertiesDlg::GetQuoteDisplayValues(
                                    const ATL::CComPtr<ICQGInstrument>& spInstrument,
                                    const ATL::CComPtr<ICQGQuote>& spQuote,
                                    CString& displayPrice,
                                    CString& displayVolume) const
{	
   displayPrice = m_N_A;
   displayVolume = m_N_A;

   if (spQuote != NULL)
   {
      VARIANT_BOOL isValid;
      HRESULT hr = spQuote->get_IsValid(&isValid);
      AssertCOMError(spQuote, hr);
      if (isValid)
      {
         double price;
         hr = spQuote->get_Price(&price);
         AssertCOMError(spQuote, hr);

         long volume;
         hr = spQuote->get_Volume(&volume);
         AssertCOMError(spQuote, hr);

         // Get the display price from raw price
         ATL::CComBSTR dispPrice;
         hr = spInstrument->ToDisplayPrice(price, &dispPrice);
         AssertCOMError(spInstrument, hr);

         // Convert volume to string
         CString dispVolume;
         dispVolume.Format(_T("%d"), volume);

         displayPrice = ATL::CW2T(dispPrice);
         displayVolume = dispVolume;
      }
   }
}

void CInstrumentPropertiesDlg::ShowAllProperties(const ATL::CComPtr<ICQGInstrument>& spInstrument)
{
   ATL::CComBSTR instrDescr;
   HRESULT hr = spInstrument->get_Description(&instrDescr);
   AssertCOMError(spInstrument, hr);
   m_lblInstrDescription.SetWindowText(CW2T(instrDescr));

   // Get properties
   ATL::CComPtr<ICQGInstrumentProperties> spProperties;
   hr = spInstrument->get_Properties(&spProperties);
   AssertCOMError(spInstrument, hr);

   // Iterate over properties
   UpdateProperties(spInstrument, spProperties);
}

void CInstrumentPropertiesDlg::UpdateProperties(
                                    const ATL::CComPtr<ICQGInstrument>& spInstrument,
                                    const ATL::CComPtr<ICQGInstrumentProperties>& spProperties)
{
   long count;
   HRESULT hr = spProperties->get_Count(&count);
   AssertCOMError(spProperties, hr);

   // get_Item method gets eInstrumentProperty as index parameter, but is assumed 
   // that all items in eInstrumentProperty enum are sequential,
   // from 0 to m_stPropsCount. That's why for is used to iterate over all items. 
   // Here we iterate over all items and if such item exists in properties collection
   // we update it.
   ATL::CComPtr<ICQGInstrumentProperty> spProperty;
   for (int i = 0; i < m_stPropsCount; ++i)
   {
      hr = spProperties->get_Item((eInstrumentProperty)i, &spProperty);

      if (hr == S_OK)
      {
         UpdateProperty(spInstrument, spProperty);
         spProperty.Release();
      }
   }
}

ATL::CComPtr<ICQGInstrumentProperty> CInstrumentPropertiesDlg::GetProperty(
                                    const ATL::CComPtr<ICQGInstrumentProperties>& spProperties,
                                    eInstrumentProperty propType) const
{
   ATL::CComPtr<ICQGInstrumentProperty> spProperty;

   HRESULT hr = spProperties->get_Item(ipInstrumentType, &spProperty);
   AssertCOMError(spProperty, hr);
   return spProperty;
}

void CInstrumentPropertiesDlg::UpdateProperty(
                                    const ATL::CComPtr<ICQGInstrument>& spInstrument,
                                    const ATL::CComPtr<ICQGInstrumentProperty>& spProperty)
{
   eInstrumentProperty propType;
   HRESULT hr = spProperty->get_Type(&propType);
   AssertCOMError(spProperty, hr);

   ATL::CComVariant propValue;
   hr = spProperty->get_Value(&propValue);
   AssertCOMError(spProperty, hr);

   VARIANT_BOOL isValid;
   hr = m_spCQGCEL->IsValid(propValue, &isValid);
   AssertCOMError(m_spCQGCEL, hr);

   // If not valid property show N/A
   if (isValid == VARIANT_FALSE)
   {
      m_lvProperties.SetItemText(propType, 1, m_N_A);
      return;
   }

   if (propType == ipInstrumentType)
   {
      CString strValue = InstrumentTypeToString((eInstrumentType)propValue.intVal);
      m_lvProperties.SetItemText(ipInstrumentType, 1, strValue);
   }
   else if (propType == ipExpirationDate)
   {
      CString strValue = TsTimeToString(propValue.date, false);
      m_lvProperties.SetItemText(ipExpirationDate, 1, strValue); 
   }
   else if (propType == ipTickSize)
   {
      // Get the display price from raw price
      ATL::CComBSTR dispValue;
      hr = spInstrument->ToDisplayPrice(propValue.dblVal, &dispValue);
      AssertCOMError(spInstrument, hr);

      m_lvProperties.SetItemText(ipTickSize, 1, ATL::CW2T(dispValue)); 
   }
   else // All other properties
   {
      CString strValue = VariantToString(propValue);
      m_lvProperties.SetItemText(propType, 1, strValue);
   }
}
/*
void CInstrumentPropertiesDlg::UpdateDOM(const ATL::CComPtr<ICQGInstrument>& spInstrument)
{
   // Clear all DOM data
   ClearDOM();

   // Update ask values on the form
   ATL::CComPtr<ICQGDOMQuotes> spAsks;
   HRESULT hr = spInstrument->get_DOMAsks(&spAsks);
   AssertCOMError(spInstrument, hr);

   long count;
   hr = spAsks->get_Count(&count);
   AssertCOMError(spAsks, hr);

   ATL::CComPtr<ICQGQuote> spQuote;
   VARIANT_BOOL isValid;
   CString displayPrice;
   CString displayVolume;
   for (int i = 0; i < count && i < m_stDOMCount; ++i)
   {
      hr = spAsks->get_Item(i, &spQuote);
      AssertCOMError(spAsks, hr);

      hr = spQuote->get_IsValid(&isValid);
      AssertCOMError(spAsks, hr);

      if (isValid == VARIANT_TRUE)
      {
         GetQuoteDisplayValues(spInstrument, spQuote, displayPrice, displayVolume); 

         m_lvDOM.SetItemText(m_stDOMCount - i, 0, displayVolume);
         m_lvDOM.SetItemText(m_stDOMCount - i, 1, displayPrice);
      }
      spQuote.Release();
   }

   // Update bids values on the form
   ATL::CComPtr<ICQGDOMQuotes> spBids;
   hr = spInstrument->get_DOMBids(&spBids);
   AssertCOMError(spInstrument, hr);

   hr = spBids->get_Count(&count);
   AssertCOMError(spAsks, hr);

   for (int i = 0; i < count && i < m_stDOMCount; ++i)
   {
      hr = spBids->get_Item(i, &spQuote);
      AssertCOMError(spBids, hr);

      hr = spQuote->get_IsValid(&isValid);
      AssertCOMError(spQuote, hr);

      if (isValid == VARIANT_TRUE)
      {
         GetQuoteDisplayValues(spInstrument, spQuote, displayPrice, displayVolume); 

         m_lvDOM.SetItemText(m_stDOMCount + i + 1, 1, displayPrice);
         m_lvDOM.SetItemText(m_stDOMCount + i + 1, 2, displayVolume);
      }
      spQuote.Release();
   }
}
*/
void CInstrumentPropertiesDlg::PublishDOM(const ATL::CComPtr<ICQGInstrument>& spInstrument)
{
	auto_ptr<LFDepthData> lfdepth(new LFDepthData);
	ATL::CComBSTR fullname;
	HRESULT hr = spInstrument->get_FullName(&fullname);
	AssertCOMError<ICQGInstrument>(spInstrument, hr);
	std::string key = ATL::CW2A(fullname);
	lfdepth->_instr_key = key;

	hr = spInstrument->get_InstrumentID(&lfdepth->_cqg_InstrumentID);
	AssertCOMError<ICQGInstrument>(spInstrument, hr);
	hr = spInstrument->get_TickSize(&lfdepth->_tick_size);
	AssertCOMError<ICQGInstrument>(spInstrument, hr);
	hr = spInstrument->get_TickValue(&lfdepth->_tick_value);
	AssertCOMError<ICQGInstrument>(spInstrument, hr);

	// Update ask values on the form
   ATL::CComPtr<ICQGDOMQuotes> spAsks;
   hr = spInstrument->get_DOMAsks(&spAsks);
   AssertCOMError(spInstrument, hr);

   long count;
   hr = spAsks->get_Count(&count);
   AssertCOMError(spAsks, hr);

   ATL::CComPtr<ICQGQuote> spQuote;
   VARIANT_BOOL isValid;
   for (int i = 0; i < 1 && i < count; /*count && i < LFDepthData::DEPTH_SIZE;*/ ++i)
   {
      hr = spAsks->get_Item(i, &spQuote);
      AssertCOMError(spAsks, hr);

      hr = spQuote->get_IsValid(&isValid);
      AssertCOMError(spAsks, hr);

      if (isValid == VARIANT_TRUE)
      {
		 double price;
		 hr = spQuote->get_Price(&price);
		 AssertCOMError(spQuote, hr);

		 long volume;
		 hr = spQuote->get_Volume(&volume);
		 AssertCOMError(spQuote, hr);
		 lfdepth->_ask[i].reset(price, volume);
      }

      spQuote.Release();
   }

   // Update bids values on the form
   ATL::CComPtr<ICQGDOMQuotes> spBids;
   hr = spInstrument->get_DOMBids(&spBids);
   AssertCOMError(spInstrument, hr);

   hr = spBids->get_Count(&count);
   AssertCOMError(spBids, hr);

   for (int i = 0; i < 1 && i < count/*count && i < LFDepthData::DEPTH_SIZE*/; ++i)
   {
      hr = spBids->get_Item(i, &spQuote);
      AssertCOMError(spBids, hr);

      hr = spQuote->get_IsValid(&isValid);
      AssertCOMError(spQuote, hr);

      if (isValid == VARIANT_TRUE)
      {
		 double price;
		 hr = spQuote->get_Price(&price);
		 AssertCOMError(spQuote, hr);

		 long volume;
		 hr = spQuote->get_Volume(&volume);
		 AssertCOMError(spQuote, hr);
		 lfdepth->_bid[i].reset(price, volume);
      }
      spQuote.Release();
   }

   lfdepth->_t = TCTimestamp::stamp();
//   Z::publish_event(lfdepth.release(), key);
   LFMarketState::instance()->send(lfdepth.release());
}

void CInstrumentPropertiesDlg::ShowSessions(const ATL::CComPtr<ICQGInstrument>& spInstrument)
{  
   // Clear all sessions data
   ClearSessions();

   ATL::CComPtr<ICQGSessions> spSessions;
   HRESULT hr = spInstrument->get_Sessions(&spSessions);
   AssertCOMError(spInstrument, hr);

   long count;
   hr = spSessions->get_Count(&count);
   AssertCOMError(spSessions, hr);

   // Iterate over sessions collection and add them to the list
   ATL::CComPtr<IEnumVARIANT> spEnumVariant;
   hr = spSessions->get__NewEnum(&spEnumVariant);
   AssertCOMError(spSessions, hr);

   std::vector<ATL::CComVariant> pItems(count);

   ULONG celtFetched;
   hr = spEnumVariant->Next(count, &pItems[0], &celtFetched);
   AssertCOMError(spEnumVariant, hr);


   ATL::CComBSTR name;
   DATE startTime, endTime, validOpen, validClose;
   short number;
   CString strNumber;
   VARIANT_BOOL isPrimary;
   eSessionWeekDays sessWeekDays;
   for (std::vector<ATL::CComVariant>::iterator it = pItems.begin(); it != pItems.end(); ++it)
   {
      ATLASSERT(it->vt == VT_DISPATCH);
      // Retreive session
      ATL::CComPtr<ICQGSession> spSession;
      hr = it->pdispVal->QueryInterface(__uuidof(ICQGSession), (void** )&spSession);
      AssertCOMError<IDispatch>(it->pdispVal, hr);

      // Get all required session data
      hr = spSession->get_Name(&name);
      AssertCOMError(spSession, hr);

      hr = spSession->get_Number(&number);
      AssertCOMError(spSession, hr);
      strNumber.Format(_T("%d"),number);

      hr = spSession->get_StartTime(&startTime);
      AssertCOMError(spSession, hr);

      hr = spSession->get_ValidStartTime(&validOpen);
      AssertCOMError(spSession, hr);

      hr = spSession->get_EndTime(&endTime);
      AssertCOMError(spSession, hr);

      hr = spSession->get_ValidEndTime(&validClose);
      AssertCOMError(spSession, hr);      

      hr = spSession->get_PrimaryFlag(&isPrimary);
      AssertCOMError(spSession, hr);

      hr = spSession->get_WorkingWeekDays(&sessWeekDays);
      AssertCOMError(spSession, hr);
    
      // Add data to list
      const long row = m_lvSessions.InsertItem(m_lvSessions.GetItemCount(), ATL::CW2T(name));
      m_lvSessions.SetItemText(row, 1, strNumber);
      m_lvSessions.SetItemText(row, 2, TsTimeToString(startTime, true, false));
      m_lvSessions.SetItemText(row, 3, TsTimeToString(validOpen, true, false));
      m_lvSessions.SetItemText(row, 4, TsTimeToString(endTime, true, false));
      m_lvSessions.SetItemText(row, 5, TsTimeToString(validClose, true, false));
      m_lvSessions.SetItemText(row, 6, WorkingDaysToString(sessWeekDays));
      m_lvSessions.SetItemText(row, 7, (isPrimary == VARIANT_FALSE) ? _T("No") : _T("Yes"));
      spSession.Release();
   }
}

ATL::CComBSTR CInstrumentPropertiesDlg::GetSelectedFullName() const
{

   try
   {
      // Get selected row index
      int curIdx = m_lsInstruments.GetCurSel();
      if (curIdx == -1)
      {
         return NULL;
      }

      // Get text from the selected row
      CString curText;
      m_lsInstruments.GetText(curIdx, curText);

      ATL::CComBSTR fullname = ATL::CT2W(curText);
      return fullname;
   }
   catch (...)
   {
      return NULL;
   }
}

ATL::CComPtr<ICQGInstrument> CInstrumentPropertiesDlg::GetInstrument(const ATL::CComBSTR& fullname) const
{
   try
   {
      // Get instruments collection
      ATL::CComPtr<ICQGInstruments> spInstruments;
      HRESULT hr = m_spCQGCEL->get_Instruments(&spInstruments);
      AssertCOMError(m_spCQGCEL, hr);

      // Get instrument from collection by it's full name
      ATL::CComVariant idx(fullname);
      ATL::CComPtr<ICQGInstrument> spInstrument;
      hr = spInstruments->get_Item(idx, &spInstrument);
      AssertCOMError(m_spCQGCEL, hr);

      return spInstrument;
   }
   catch (...)
   {
   }

   return NULL;
}

void CInstrumentPropertiesDlg::EnableControls(BOOL enable/* = TRUE*/)
{
   VARIANT_BOOL isStarted = VARIANT_FALSE;
   HRESULT hr;

   if (m_spCQGCEL)
   {
      hr = m_spCQGCEL->get_IsStarted(&isStarted);
      AssertCOMError(m_spCQGCEL, hr);
   }

   if (isStarted == VARIANT_FALSE)
   {
      m_edSymbolName.EnableWindow(FALSE);
      m_btSubscribe.EnableWindow(FALSE);
      m_btnQFormula.EnableWindow(FALSE);
      m_btRemove.EnableWindow(FALSE);
      m_btRemoveAll.EnableWindow(FALSE);
      return;
   }

   // Check connection status 
   ATL::CComPtr<ICQGEnvironment> spEnv;
   hr = m_spCQGCEL->get_Environment(&spEnv);
   AssertCOMError(m_spCQGCEL, hr);

   eConnectionStatus connStatus;
   hr = spEnv->get_DataConnectionStatus(&connStatus);
   AssertCOMError(spEnv, hr);

   BOOL isEnableAndConnectionUp = enable && (connStatus == csConnectionUp);
   
   m_edSymbolName.EnableWindow(isEnableAndConnectionUp);
   m_btSubscribe.EnableWindow(isEnableAndConnectionUp);
   m_btnQFormula.EnableWindow(isEnableAndConnectionUp);

   int isNotEmpty = (m_lsInstruments.GetCount() > 0);
   m_btRemove.EnableWindow(isNotEmpty && isEnableAndConnectionUp);
   m_btRemoveAll.EnableWindow(isNotEmpty && isEnableAndConnectionUp);
}

CString CInstrumentPropertiesDlg::InstrumentTypeToString(eInstrumentType instrType)
{
   CString strInstrType;

   switch (instrType)
   {
   case itFuture:
      strInstrType = _T("Future");
      break;
   case itOptionCall:
      strInstrType = _T("OptionCall");
      break;
   case itOptionPut:
      strInstrType = _T("OptionPut");
      break;
   case itOther:
      strInstrType = _T("Other");
      break;
   case itStock:
      strInstrType = _T("Stock");
      break;
   case itTreasure:
      strInstrType = _T("Treasure");
      break;
   case itSyntheticStrategy:
      strInstrType = _T("SyntheticStrategy");
      break;
   default:
      strInstrType = _T("Undefined");
      break;
   }

   return strInstrType;
}

CString CInstrumentPropertiesDlg::WorkingDaysToString(eSessionWeekDays daysMask)
{
   CString workDays;

   workDays = (daysMask & swdSunday) ? _T("S") : _T("-");
   workDays += (daysMask & swdMonday) ? _T("M") : _T("-");
   workDays += (daysMask & swdTuesday) ? _T("T") : _T("-");
   workDays += (daysMask & swdWednesday) ? _T("W") : _T("-");
   workDays += (daysMask & swdThursday) ? _T("T") : _T("-");
   workDays += (daysMask & swdFriday) ? _T("F") : _T("-");
   workDays += (daysMask & swdSaturday) ? _T("S") : _T("-");

   return workDays;
}

CString CInstrumentPropertiesDlg::ConnectionStatusToString(eConnectionStatus status)
{
   CString  statusString;

   switch (status)
   {
   case csConnectionDelayed:
      statusString = "delayed";
      break;
   case csConnectionDown:
      statusString = "down";
      break;
   case csConnectionUp:
      statusString = "up";
      break;
   case csConnectionTrouble:
      statusString = "trouble";
      break;
   case csConnectionNotLoggedOn:
      statusString = "not logged on";
      break;
   default:
      assert(!"Unknown connection status");
   }

   std::string tce_host = TCSettings::get("COMMAND:tce_host", TCSettings::get("TCE:host"));
   std::string tce_port = TCSettings::get("COMMAND:tce_port", TCSettings::get("TCE:port", "9299"));

   CString  tceString; tceString.Format(_T(" to %S:%S"), tce_host.c_str(), tce_port.c_str());
   statusString += tceString;
   return statusString;
}