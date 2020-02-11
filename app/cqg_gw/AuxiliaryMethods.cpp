// AuxiliaryMethods.cpp : implementation file
//

#include "stdafx.h"
#include <stdexcept>
#include <cassert>
#include <vector>
#include "Common.h"
#include "OrderPlacing.h"
#include "OrderPlacingDlg.h"

void COrderPlacingDlg::InitializeCQGCEL()
{
   // create an instance of CQG API
   HRESULT hr = m_spCQGCEL.CoCreateInstance(__uuidof(CQGCEL), NULL, CLSCTX_INPROC_SERVER);
   if (FAILED(hr))
   {
      throw std::runtime_error("Unable to create CQGCEL object. "
                               "Please register it again and restart application.");
   }

   // Configure CQGCEL behavior
   ATL::CComPtr<ICQGAPIConfig> spConf;
   hr = m_spCQGCEL->get_APIConfiguration(&spConf);
   AssertCOMError(m_spCQGCEL, hr);

   hr = spConf->put_ReadyStatusCheck(rscOff);
   AssertCOMError(spConf, hr);

   hr = spConf->put_UsedFromATLClient(VARIANT_TRUE);
   AssertCOMError(spConf, hr);

   hr = spConf->put_CollectionsThrowException(VARIANT_FALSE);
   AssertCOMError(spConf, hr);

   hr = spConf->put_TimeZoneCode(tzCentral);
   AssertCOMError(spConf, hr);

   hr = spConf->put_UseOrderSide(VARIANT_TRUE);
   AssertCOMError(spConf, hr);

   // Now advise the connection, to get events
   hr = ICQGCELDispEventImpl::DispEventAdvise(m_spCQGCEL);
   ATLASSERT(SUCCEEDED(hr));

   hr = m_spCQGCEL->Startup();
   AssertCOMError(m_spCQGCEL, hr);
}

void COrderPlacingDlg::InitializeOrdersList()
{
   m_lvOrders.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
   m_lvOrders.ModifyStyle(0, LVS_SHOWSELALWAYS | LVS_SINGLESEL);

   m_lvOrders.InsertColumn(ocID, _T("Order #"), LVCFMT_LEFT, 55);
   m_lvOrders.InsertColumn(ocSide, _T("Buy/Sell"), LVCFMT_LEFT, 50);
   m_lvOrders.InsertColumn(ocSize, _T("Size"), LVCFMT_LEFT, 50);
   m_lvOrders.InsertColumn(ocState, _T("State"), LVCFMT_LEFT, 100);
   m_lvOrders.InsertColumn(ocInstrument, _T("Instrument"), LVCFMT_LEFT, 100);
   m_lvOrders.InsertColumn(ocType, _T("Type"), LVCFMT_LEFT, 58);
   m_lvOrders.InsertColumn(ocLimitPrice, _T("Limit Price"), LVCFMT_LEFT, 65);
   m_lvOrders.InsertColumn(ocStopPrice, _T("Stop Price"), LVCFMT_LEFT, 65);
   m_lvOrders.InsertColumn(ocDuration, _T("Duration"), LVCFMT_LEFT, 60);
   m_lvOrders.InsertColumn(ocAvgFillPrice, _T("Avg Fill Price"), LVCFMT_LEFT, 75);
   m_lvOrders.InsertColumn(ocPlaceTime, _T("Place Time"), LVCFMT_LEFT, 115);
   // Hidden column
   m_lvOrders.InsertColumn(ocGWOrderID, _T("GWOrderID"), LVCFMT_LEFT, 0);
}

void COrderPlacingDlg::InitializeTypesCombo()
{
   int row = m_cmbType.AddString(_T("Market"));
   m_cmbType.SetItemData(row, otMarket);

   row = m_cmbType.AddString(_T("Limit"));
   m_cmbType.SetItemData(row, otLimit);

   row = m_cmbType.AddString(_T("Stop"));
   m_cmbType.SetItemData(row, otStop);

   row = m_cmbType.AddString(_T("StopLimit"));
   m_cmbType.SetItemData(row, otStopLimit);

   // Select first element
   m_cmbType.SetCurSel(0);
}

void COrderPlacingDlg::InitializeDurationsCombo()
{
   int row = m_cmbDuration.AddString(_T("DAY"));
   m_cmbDuration.SetItemData(row, odDay);

   row = m_cmbDuration.AddString(_T("FAK"));
   m_cmbDuration.SetItemData(row, odFAK);

   row = m_cmbDuration.AddString(_T("FOK"));
   m_cmbDuration.SetItemData(row, odFOK);
   
   row = m_cmbDuration.AddString(_T("GTC"));
   m_cmbDuration.SetItemData(row, odGoodTillCanceled);
   
   row = m_cmbDuration.AddString(_T("GTD"));
   m_cmbDuration.SetItemData(row, odGoodTillDate);
   
   row = m_cmbDuration.AddString(_T("ATC"));
   m_cmbDuration.SetItemData(row, odATC);

   row = m_cmbDuration.AddString(_T("ATO"));
   m_cmbDuration.SetItemData(row, odATO);
   
   row = m_cmbDuration.AddString(_T("GTT"));
   m_cmbDuration.SetItemData(row, odGoodTillTime);

   // Select first element
   m_cmbDuration.SetCurSel(0);
}

void COrderPlacingDlg::InitializeOrdersTab()
{
   m_tabOrders.InsertItem(ftWorking, _T("Working"));
   m_tabOrders.InsertItem(ftFilled, _T("Filled"));
   m_tabOrders.InsertItem(ftCancelled, _T("Cancelled"));
   m_tabOrders.InsertItem(ftExceptions, _T("Exceptions"));
   m_tabOrders.InsertItem(ftParked, _T("Parked"));
   int tab = m_tabOrders.InsertItem(ftAll, _T("All"));

   // Select "All" tab
   m_tabOrders.SetCurSel(tab);
}

void COrderPlacingDlg::InitializeBooleans()
{
   m_DataConnectionUp = false;
   m_GWConnectionUp = false;
   m_LimitPriceEnabled = false;
   m_StopPriceEnabled = false;
   m_GTDEnabled = false;
}

void COrderPlacingDlg::InitializeControls()
{
   ClearAllInstrumentData();

   OnDataConnectionStatusChanged(csConnectionDown);
   OnGWConnectionStatusChanged(csConnectionDown);

   InitializeOrdersTab();
   InitializeOrdersList();
   InitializeTypesCombo();
   InitializeDurationsCombo();

   m_dtpGTD.SetFormat(m_SHORT_DATE);
}

void COrderPlacingDlg::EnableControls()
{
   m_cmbAccounts.EnableWindow(m_GWConnectionUp);

   m_edSymbolName.EnableWindow(m_DataConnectionUp);
   m_btnSubscribe.EnableWindow(m_DataConnectionUp);

   bool isInstrumentSelected = (m_spCQGCEL && GetSelectedInstrument());  

   m_lsInstruments.EnableWindow(m_DataConnectionUp);
   m_btnRemoveAll.EnableWindow(m_DataConnectionUp);
   m_btnRemove.EnableWindow(m_DataConnectionUp && isInstrumentSelected);

   bool isAccountSelected = (m_spCQGCEL && GetSelectedAccount());

   m_btnBuy.EnableWindow(m_GWConnectionUp && isAccountSelected && isInstrumentSelected);
   m_btnSell.EnableWindow(m_GWConnectionUp && isAccountSelected && isInstrumentSelected);

   m_edQuantity.EnableWindow(m_GWConnectionUp && isInstrumentSelected);
   m_spnQuantity.EnableWindow(m_GWConnectionUp && isInstrumentSelected);

   m_edLimitPrice.EnableWindow(m_GWConnectionUp && isInstrumentSelected && m_LimitPriceEnabled);
   m_spnLimitPrice.EnableWindow(m_GWConnectionUp && isInstrumentSelected && m_LimitPriceEnabled);
   
   m_edStopPrice.EnableWindow(m_GWConnectionUp && isInstrumentSelected && m_StopPriceEnabled);
   m_spnStopPrice.EnableWindow(m_GWConnectionUp && isInstrumentSelected && m_StopPriceEnabled);
   
   m_cmbType.EnableWindow(m_GWConnectionUp && isInstrumentSelected);

   m_cmbDuration.EnableWindow(m_GWConnectionUp && isInstrumentSelected);
   m_dtpGTD.EnableWindow(m_GWConnectionUp && isInstrumentSelected && m_GTDEnabled);

   m_chkParked.EnableWindow(m_GWConnectionUp && isInstrumentSelected);

   bool isOrderSelected = (m_spSelectedOrder != NULL);

   m_btnModify.EnableWindow(m_GWConnectionUp && isOrderSelected);
   m_btnActivate.EnableWindow(m_GWConnectionUp && isOrderSelected);
   m_btnActivateAll.EnableWindow(m_GWConnectionUp && isAccountSelected);
   m_btnCancel.EnableWindow(m_GWConnectionUp && isOrderSelected);
   m_btnCancelAll.EnableWindow(m_GWConnectionUp && isAccountSelected);
}

void COrderPlacingDlg::ClearAllInstrumentData()
{
   ClearDescription();
   ClearQuotes();

   m_edStopPrice.SetWindowText(m_N_A);
   m_edLimitPrice.SetWindowText(m_N_A);

   SetQuantity(m_edQuantity, m_MIN_QUANTITY);
}

void COrderPlacingDlg::ClearDescription()
{
   m_lblInstrDescription.SetWindowText(m_N_A);
}

void COrderPlacingDlg::ClearQuotes()
{
   // Set all quotes texts to N/A
   m_lblAskPrice.SetWindowText(m_N_A);
   m_lblAskVolume.SetWindowText(m_N_A);
   m_lblBidPrice.SetWindowText(m_N_A);
   m_lblBidVolume.SetWindowText(m_N_A);
   m_lblTradePrice.SetWindowText(m_N_A);
   m_lblTradeVolume.SetWindowText(m_N_A);
}

void COrderPlacingDlg::ClearOrders()
{
   // Clear orders
   m_lvOrders.DeleteAllItems();

   m_spSelectedOrder = NULL;
}

void COrderPlacingDlg::ReloadAccounts()
{
   // Clear all previous data
   m_cmbAccounts.ResetContent();
   ClearOrders();

   // Get accounts from the CQGCEL
   ATL::CComPtr<ICQGAccounts> spAccounts;
   HRESULT hr = m_spCQGCEL->get_Accounts(&spAccounts);
   AssertCOMError(m_spCQGCEL, hr);

   long count;
   hr = spAccounts->get_Count(&count);
   AssertCOMError(spAccounts, hr);

   if (count == 0)
   {
      // No accounts
      EnableControls();
      return;
   }

   // Iterate over accounts collection and add them to the list
   ATL::CComPtr<IEnumVARIANT> spEnumVariant;
   hr = spAccounts->get__NewEnum(&spEnumVariant);
   AssertCOMError(spAccounts, hr);

   std::vector<ATL::CComVariant> pItems(count);

   ULONG celtFetched;
   hr = spEnumVariant->Next(count, &pItems[0], &celtFetched);
   AssertCOMError(spEnumVariant, hr);

   for (std::vector<ATL::CComVariant>::iterator it = pItems.begin(); it != pItems.end(); ++it)
   {
      ATLASSERT(it->vt == VT_DISPATCH);

      ATL::CComPtr<ICQGAccount> spAccount;
      hr = it->pdispVal->QueryInterface(__uuidof(ICQGAccount), (void** )&spAccount);
      AssertCOMError<IDispatch>(it->pdispVal, hr);

      // Add current account to the list
      InsertAccount(spAccount);
   }

   // Select the first account in the list
   m_cmbAccounts.SetCurSel(0);

   EnableControls();
}

void COrderPlacingDlg::InsertAccount(
                                 const ATL::CComPtr<ICQGAccount>& spAccount)
{
   // Create string for specified account to add to the combo
   ATL::CComBSTR accName;
   HRESULT hr = spAccount->get_GWAccountName(&accName);
   AssertCOMError(spAccount, hr);

   // Get Fcm account ID
   long FcmID;
   hr = spAccount->get_FcmID(&FcmID);
   AssertCOMError(spAccount, hr);
   
   ATL::CComBSTR FcmAccID;
   hr = spAccount->get_FcmAccountID(&FcmAccID);
   AssertCOMError(spAccount, hr);

   CString accString = ATL::CW2T(accName);
   accString = accString + _T(" (") + ATL::CW2T(FcmAccID) + _T(")");

   // Get account ID to map with item
   long accID;
   hr = spAccount->get_GWAccountID(&accID);
   AssertCOMError(spAccount, hr);

   // Add new account name to the combo and map ID
   int row = m_cmbAccounts.AddString(accString);
   m_cmbAccounts.SetItemData(row, accID);
   Z::report_info("Account %s -- FcmId = %d,  FcmAccID = %s, GWAccountId = %d", 
	   (const char*)ATL::CW2A(accName), FcmID, (const char*)ATL::CW2A(FcmAccID), accID);
   /*
   std::ostringstream os;
   os << "AccountRouting:" << accID;
   long strat_num = Z::get_setting_integer(os.str(), -1);
   if (strat_num >= 0)
   {
	   _routing[accID] = strat_num;
	   Z::report_info("AccountRouting (%s) -- GWAccountId = %d -> strat_number = %d",
		   (const char*)ATL::CW2A(accName), accID, strat_num);
   }
   */
}

void COrderPlacingDlg::ShowDescription(
                                 const ATL::CComPtr<ICQGInstrument>& spInstrument)
{
   ATL::CComBSTR instrDescr;
   HRESULT hr = spInstrument->get_Description(&instrDescr);
   AssertCOMError(spInstrument, hr);

   m_lblInstrDescription.SetWindowText(CW2T(instrDescr));
}

double COrderPlacingDlg::GetValidPrice(
                                 const ATL::CComPtr<ICQGInstrument>& spInstrument)
{
   ATL::CComPtr<ICQGQuotes> spQuotes;
   HRESULT hr = spInstrument->get_Quotes(&spQuotes);
   AssertCOMError(spInstrument, hr);
 
   // Check quotes to get valid price
   ATL::CComPtr<ICQGQuote> spQuote;
   hr = spQuotes->get_Item(qtTrade, &spQuote);
   AssertCOMError(spQuotes, hr);
   
   VARIANT_BOOL isValid;
   hr = spQuote->get_IsValid(&isValid);
   AssertCOMError(spQuote, hr);
   
   if (!isValid)
   {
      hr = spQuotes->get_Item(qtAsk, &spQuote);
      AssertCOMError(spQuotes, hr);
   
      hr = spQuote->get_IsValid(&isValid);
      AssertCOMError(spQuote, hr);
      
      if (!isValid)
      {
         hr = spQuotes->get_Item(qtBid, &spQuote);
         AssertCOMError(spQuotes, hr);
      }
   }
   
   double price = m_INVALID_DOUBLE;;
   
   if (spQuote)
   {
      hr = spQuote->get_Price(&price);
      AssertCOMError(spQuote, hr);
   }
   
   return price;
}

void COrderPlacingDlg::ShowValidPrice(
                                 const ATL::CComPtr<ICQGInstrument>& spInstrument)
{   
   m_edLimitPrice.SetWindowText(m_N_A);
   m_edStopPrice.SetWindowText(m_N_A);
   
      // Get order type
   eOrderType orderType = static_cast<eOrderType>
                                    (m_cmbType.GetItemData(m_cmbType.GetCurSel()));

   m_LimitPriceEnabled = (orderType == otLimit || orderType == otStopLimit);
   m_StopPriceEnabled = (orderType == otStop || orderType == otStopLimit);
   
   if (m_LimitPriceEnabled || m_StopPriceEnabled)
   {
      VARIANT_BOOL isValid;
      double price = GetValidPrice(spInstrument);
      
      HRESULT hr = m_spCQGCEL->IsValid(ATL::CComVariant(price), &isValid);
      AssertCOMError(m_spCQGCEL, hr);
      
      if (isValid)
      {
         ATL::CComBSTR displayPrice;
         hr = spInstrument->ToDisplayPrice(price, &displayPrice);
         AssertCOMError(spInstrument, hr);
         
         if (m_LimitPriceEnabled)
         {
            m_edLimitPrice.SetWindowText(ATL::CW2T(displayPrice));
         }
            
         if (m_StopPriceEnabled)
         {
            m_edStopPrice.SetWindowText(ATL::CW2T(displayPrice));
         }
      }
   }
   
   EnableControls();
}

void COrderPlacingDlg::ShowQuotes(
                                 const ATL::CComPtr<ICQGInstrument>& spInstrument)
{
   ATL::CComPtr<ICQGQuotes> spQuotes;
   HRESULT hr = spInstrument->get_Quotes(&spQuotes);
   AssertCOMError(spInstrument, hr);

   ShowQuote(spQuotes, qtAsk, m_lblAskPrice, m_lblAskVolume);
   ShowQuote(spQuotes, qtTrade, m_lblTradePrice, m_lblTradeVolume);
   ShowQuote(spQuotes, qtBid, m_lblBidPrice, m_lblBidVolume);
}

void COrderPlacingDlg::ShowQuote(
                                 const ATL::CComPtr<ICQGQuotes>& spQuotes,
                                 eQuoteType quoteType,
                                 CStatic& priceLabel,
                                 CStatic& volumeLabel)
{
   // Update quote
   ATL::CComPtr<ICQGQuote> spQuote;
   HRESULT hr = spQuotes->get_Item(quoteType, &spQuote);
   if (SUCCEEDED(hr))
   {
      CString displayPrice;
      CString displayVolume;
      GetQuoteDisplayValues(spQuote, displayPrice, displayVolume); 
      priceLabel.SetWindowText(displayPrice);
      volumeLabel.SetWindowText(displayVolume);

      spQuote.Release();
   }
}

void COrderPlacingDlg::GetQuoteDisplayValues(
                                 const ATL::CComPtr<ICQGQuote>& spQuote,
                                 CString& displayPrice,
                                 CString& displayVolume)
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

         ATL::CComPtr<ICQGInstrument> spInstrument;
         hr = spQuote->get_Instrument(&spInstrument);
         AssertCOMError(spQuote, hr);

         // Get the display price from price
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

void COrderPlacingDlg::ShowFilteredOrders()
{
   // Clear old data
   ClearOrders();

   // Get account and it's all orders
   ATL::CComPtr<ICQGAccount> spAccount = GetSelectedAccount();
   
   if (spAccount == NULL)
   {
      // No account selected
      return;
   }

   ATL::CComPtr<ICQGOrders> spOrders;
   HRESULT hr = spAccount->get_Orders(&spOrders);
   AssertCOMError(spOrders, hr);

   // Get order status, to filter by
   int status = FilterTypeToOrderStatus(static_cast<eFilterType>(m_tabOrders.GetCurSel()));

   if (status == m_INVALID_INTEGER)
   {
      // No filter, show all orders
      ShowOrders(spOrders);
   }
   else
   {
      // Get filtered orders by selected status
      ATL::CComPtr<ICQGOrders> spOrdersBy;
      hr = spOrders->SelectByOrderStatus(static_cast<eOrderStatus>(status), &spOrdersBy);
      AssertCOMError(spOrders, hr);

      // Show filtered orders
      ShowOrders(spOrdersBy);
   }
}

void COrderPlacingDlg::UnselectAllOrders()
{
   for (int i = 0; i < m_lvOrders.GetItemCount(); ++i)
   {
      m_lvOrders.SetItemState(i, 0, LVIS_SELECTED);
   }
}

void COrderPlacingDlg::ShowOrders(
                                 const ATL::CComPtr<ICQGOrders>& spOrders)
{
   long count;
   HRESULT hr = spOrders->get_Count(&count);
   AssertCOMError(spOrders, hr);

   if (count == 0)
   {
      // No orders
      return;
   }

   // Iterate over orders collection and add them to the list
   ATL::CComPtr<IEnumVARIANT> spEnumVariant;
   hr = spOrders->get__NewEnum(&spEnumVariant);
   AssertCOMError(spOrders, hr);

   std::vector<ATL::CComVariant> pItems(count);

   ULONG celtFetched;
   hr = spEnumVariant->Next(count, &pItems[0], &celtFetched);
   AssertCOMError(spEnumVariant, hr);

   for (std::vector<ATL::CComVariant>::iterator it = pItems.begin(); it != pItems.end(); ++it)
   {
      assert(it->vt == VT_DISPATCH);

      ATL::CComPtr<ICQGOrder> spOrder;
      hr = it->pdispVal->QueryInterface(__uuidof(ICQGOrder), (void** )&spOrder);
      AssertCOMError<IDispatch>(it->pdispVal, hr);

      // Add current order to the list
      ShowOrder(spOrder, ctAdded);
   }
}

int COrderPlacingDlg::GetOrderRow(
                                 const ATL::CComPtr<ICQGOrder>& spOrder)
{
   ATL::CComBSTR orderID;
   HRESULT hr = spOrder->get_OriginalOrderID(&orderID);
   AssertCOMError(spOrder, hr);

   // Find row for specified order
   LVFINDINFO findInfo;
   findInfo.flags = LVFI_STRING;
   findInfo.psz = ATL::CW2T(orderID);

   return m_lvOrders.FindItem(&findInfo);
}

void COrderPlacingDlg::ShowOrder(
                                 const ATL::CComPtr<ICQGOrder>& spOrder,
                                 eChangeType change,
								 ICQGFill* fill, 
								 ICQGError* cqgerr,
								 long acount)

{
   // Get specified order's row
   int row = GetOrderRow(spOrder);

   // If change type is removed, remove order from list
   if (change == ctRemoved && row > -1)
   {
      m_lvOrders.DeleteItem(row);
      m_spSelectedOrder = NULL;

      return;
   }

   // Add new one if not found
   if (row == -1)
   {
      row = m_lvOrders.InsertItem(m_lvOrders.GetItemCount(), _T(""));
   }

   ATL::CComBSTR orderID;
   HRESULT hr = spOrder->get_OriginalOrderID(&orderID);
   AssertCOMError(spOrder, hr);

   eOrderSide side;
   hr = spOrder->get_Side(&side);
   AssertCOMError(spOrder, hr);

   long size;
   hr = spOrder->get_Quantity(&size);
   AssertCOMError(spOrder, hr);

   eOrderStatus status;
   hr = spOrder->get_GWStatus(&status);
   AssertCOMError(spOrder, hr);

   ATL::CComBSTR instrumentName;
   hr = spOrder->get_InstrumentName(&instrumentName);
   AssertCOMError(spOrder, hr);

   eOrderType type;
   hr = spOrder->get_Type(&type);
   AssertCOMError(spOrder, hr);

   double limitPrice;
   hr = spOrder->get_LimitPrice(&limitPrice);
   AssertCOMError(spOrder, hr);

   double stopPrice;
   hr = spOrder->get_StopPrice(&stopPrice);
   AssertCOMError(spOrder, hr);

   eOrderDuration duration;
   hr = spOrder->get_DurationType(&duration);
   AssertCOMError(spOrder, hr);

   DATE placeTime;
   hr = spOrder->get_PlaceTime(&placeTime);
   AssertCOMError(spOrder, hr);

   // Get order properties to get average fill price
   ATL::CComPtr<ICQGOrderProperties> spProperties;
   hr = spOrder->get_Properties(&spProperties);
   AssertCOMError(spOrder, hr);

   ATL::CComPtr<ICQGOrderProperty> spProperty;
   hr = spProperties->get_Item(opAverageFillPrice, &spProperty);
   AssertCOMError(spProperties, hr);

   ATL::CComVariant avgFillPriceVar;
   hr = spProperty->get_Value(&avgFillPriceVar);
   AssertCOMError(spProperty, hr);

   double avgFillPrice = avgFillPriceVar.dblVal;
     
   // Map order GW ID with corresponding item
   ATL::CComBSTR gwOrderID;
   hr = spOrder->get_GWOrderID(&gwOrderID);
   AssertCOMError(spOrder, hr);

   //----------------------------------------------------------------------
   m_lvOrders.SetItemText(row, ocID, ATL::CW2T(orderID));
   m_lvOrders.SetItemText(row, ocSide, OrderSideToString(side));
   m_lvOrders.SetItemText(row, ocSize, GetValueAsString(size));
   m_lvOrders.SetItemText(row, ocState, OrderStatusToString(status));
   m_lvOrders.SetItemText(row, ocInstrument, ATL::CW2T(instrumentName));
   m_lvOrders.SetItemText(row, ocType, OrderTypeToString(type));
   m_lvOrders.SetItemText(row, ocLimitPrice, GetDisplayPrice(spOrder, limitPrice));
   m_lvOrders.SetItemText(row, ocStopPrice, GetDisplayPrice(spOrder, stopPrice));
   m_lvOrders.SetItemText(row, ocDuration, OrderDurationToString(duration));
   m_lvOrders.SetItemText(row, ocAvgFillPrice, GetDisplayPrice(spOrder, avgFillPrice));
   m_lvOrders.SetItemText(row, ocPlaceTime, GetValueAsString(placeTime, VT_DATE));
   m_lvOrders.SetItemText(row, ocGWOrderID, ATL::CW2T(gwOrderID));
}
void COrderPlacingDlg::SendOrder(
                                 const ATL::CComPtr<ICQGOrder>& spOrder,
                                 eChangeType change,
								 ICQGFill* fill, 
								 ICQGError* cqgerr,
								 long acount)

{
   ATL::CComBSTR orderID;
   HRESULT hr = spOrder->get_OriginalOrderID(&orderID);
   AssertCOMError(spOrder, hr);

   eOrderSide side;
   hr = spOrder->get_Side(&side);
   AssertCOMError(spOrder, hr);

   long size;
   hr = spOrder->get_Quantity(&size);
   AssertCOMError(spOrder, hr);

   eOrderStatus status;
   hr = spOrder->get_GWStatus(&status);
   AssertCOMError(spOrder, hr);

   ATL::CComBSTR instrumentName;
   hr = spOrder->get_InstrumentName(&instrumentName);
   AssertCOMError(spOrder, hr);

   eOrderType type;
   hr = spOrder->get_Type(&type);
   AssertCOMError(spOrder, hr);

   double limitPrice;
   hr = spOrder->get_LimitPrice(&limitPrice);
   AssertCOMError(spOrder, hr);

   double stopPrice;
   hr = spOrder->get_StopPrice(&stopPrice);
   AssertCOMError(spOrder, hr);

   eOrderDuration duration;
   hr = spOrder->get_DurationType(&duration);
   AssertCOMError(spOrder, hr);

   DATE placeTime;
   hr = spOrder->get_PlaceTime(&placeTime);
   AssertCOMError(spOrder, hr);

   // Get order properties to get average fill price
   ATL::CComPtr<ICQGOrderProperties> spProperties;
   hr = spOrder->get_Properties(&spProperties);
   AssertCOMError(spOrder, hr);

   ATL::CComPtr<ICQGOrderProperty> spProperty;
   hr = spProperties->get_Item(opAverageFillPrice, &spProperty);
   AssertCOMError(spProperties, hr);

   ATL::CComVariant avgFillPriceVar;
   hr = spProperty->get_Value(&avgFillPriceVar);
   AssertCOMError(spProperty, hr);

   double avgFillPrice = avgFillPriceVar.dblVal;
     
   // Map order GW ID with corresponding item
   ATL::CComBSTR gwOrderID;
   hr = spOrder->get_GWOrderID(&gwOrderID);
   AssertCOMError(spOrder, hr);


	//-------------- fill LFUpdateOrder
	auto_ptr<LFOrderUpdate> r(new LFOrderUpdate);
	r->_t = TCTimestamp::stamp();
	
	// Get Tags
    
	ATL::CComVariant tagValue;
    
	hr = spOrder->get_Tag(ATL::CA2W("_order_id"), &tagValue);
	AssertCOMError(spOrder, hr);
	r->_order_id = tagValue.lVal;
	
	hr = spOrder->get_Tag(ATL::CA2W("_strategy_id"), &tagValue);
	AssertCOMError(spOrder, hr);
	r->_strategy_id = tagValue.lVal;

	hr = spOrder->get_Tag(ATL::CA2W("_sub_strategy_id"), &tagValue);
	AssertCOMError(spOrder, hr);
	r->_sub_strategy_id = tagValue.lVal;
	
	hr = spOrder->get_Tag(ATL::CA2W("_strategy_key"), &tagValue);
	AssertCOMError(spOrder, hr);
	r->_strategy_key = CW2A(tagValue.bstrVal);

	r->_instr = CW2A(instrumentName);
	r->_cqg_AccountID = acount;
	// account based routing
	if (r->_strategy_key.empty())
	{
		//AccountStrategyMap::iterator ir = _routing.find(r->_cqg_AccountID);
		//if (ir != _routing.end())
		//{
		r->_strategy_id = LFSecurityMaster::instance()->strategy_id(r->_instr, r->_cqg_AccountID);
		Z::report_info("EMPTY KEY routing (%s + %d) = %d", r->_instr.c_str(), r->_cqg_AccountID, r->_strategy_id);
		r->_sub_strategy_id = 3; // external
		r->_strategy_key = LF::make_strategy_key(r->_strategy_id, r->_instr);
		//}
	}

	r->_cqg_OriginalOrderId = CW2A(orderID);
	r->_order_side = static_cast<LF::Side>(side);
	r->_q = size;
	r->_order_status = static_cast<LF::OrderStatus>(status);
	r->_order_type = static_cast<LF::OrderType>(type);
	r->_limit_p = (r->_order_type == otLimit || r->_order_type == otStopLimit) ? limitPrice : 0;
	r->_stop_p  = (r->_order_type == otStop  || r->_order_type == otStopLimit) ? stopPrice : 0;
	r->_avg_fill_p = avgFillPrice;
	r->_cqg_GWOrderID = CW2A(gwOrderID);
	
	// Fill data from ICQGOrder interface

	hr = spOrder->get_FilledQuantity(&(r->_filled_q));
	AssertCOMError(spOrder, hr);
	hr = spOrder->get_RemainingQuantity(&(r->_remain_q));
	AssertCOMError(spOrder, hr);

	// Fill properties

	spProperty.Release();
	ATL::CComVariant prop;
	hr = spProperties->get_Item(opUserName, &spProperty);
	AssertCOMError(spProperties, hr);
	hr = spProperty->get_Value(&prop);
	AssertCOMError(spProperty, hr);
	r->_user_name = CW2A(prop.bstrVal);

	spProperty.Release();
	hr = spProperties->get_Item(opTraderID, &spProperty);
	AssertCOMError(spProperties, hr);
	hr = spProperty->get_Value(&prop);
	AssertCOMError(spProperty, hr);
	r->_trader_id = prop.lVal;

	spProperty.Release();
	hr = spProperties->get_Item(opDescription, &spProperty);
	AssertCOMError(spProperties, hr);
	hr = spProperty->get_Value(&prop);
	AssertCOMError(spProperty, hr);
	r->_order_id = atol(CW2A(prop.bstrVal));
	
	// Get Errors
    VARIANT_BOOL isValid;      
    hr = m_spCQGCEL->IsValid(ATL::CComVariant(cqgerr), &isValid);
	if(isValid == VARIANT_TRUE)
	{
		hr = cqgerr->get_Code(&(r->_err_code));
		AssertCOMError<ICQGError>(cqgerr, hr);

		BSTR e;
		hr = cqgerr->get_Description(&e);
		AssertCOMError<ICQGError>(cqgerr, hr);
		r->_err_desc = CW2A(e);
	}
	
	// Get Fills
	hr = m_spCQGCEL->IsValid(ATL::CComVariant(fill), &isValid);
	if(isValid == VARIANT_TRUE)
	{
		auto_ptr<LFOrderFill> u(new LFOrderFill(*r));
		u->_t = TCTimestamp::stamp();

		hr = fill->get_Price(1, &(u->_fill._p));
		AssertCOMError<ICQGFill>(fill, hr);
		
		long q;
		hr = fill->get_Quantity(1, &q);
		AssertCOMError<ICQGFill>(fill, hr);
		u->_fill._q = q;

		BSTR b;	
		hr = fill->get_Id(&b);
		AssertCOMError<ICQGFill>(fill, hr);
		u->_fill_id = CW2A(b);
		
		eFillStatus s;
		hr = fill->get_Status(&s);
		AssertCOMError<ICQGFill>(fill, hr);
		u->_status = static_cast<LF::FillStatus>(s);
		r->_fills.push_back(*u);
		BMModel::instance()->send(u.release());
	}
	BMModel::instance()->send(r.release());
}

CString COrderPlacingDlg::GetDisplayPrice(
                                 const ATL::CComPtr<ICQGOrder> spOrder,
                                 double price) const
{
   VARIANT_BOOL isValid;
   HRESULT hr = m_spCQGCEL->IsValid(ATL::CComVariant(price), &isValid);
   AssertCOMError(m_spCQGCEL, hr);

   // Get instrument to get price display format
   ATL::CComPtr<ICQGInstrument> spInstrument;
   hr = spOrder->get_Instrument(&spInstrument);
   AssertCOMError(spOrder, hr);

   CString displayPrice;
   if (isValid && spInstrument != NULL)
   {
      // Get display price
      ATL::CComBSTR displayPriceBSTR;
      hr = spInstrument->ToDisplayPrice(price, &displayPriceBSTR);
      AssertCOMError(spInstrument, hr);

      displayPrice = ATL::CW2T(displayPriceBSTR);
   }
   else
   {
      displayPrice = m_N_A;
   }

   return displayPrice;
}

CString COrderPlacingDlg::GetValueAsString(
                                 ATL::CComVariant value,
                                 VARENUM valueType/*= VT_EMPTY*/) const
{
   if (valueType != VT_EMPTY)
   {
      value.vt = valueType;
   }

   // Check validness
   VARIANT_BOOL isValid;
   HRESULT hr = m_spCQGCEL->IsValid(value, &isValid);
   AssertCOMError(m_spCQGCEL, hr);

   if (isValid)
   {
      return VariantToString(value);
   }
   return m_N_A;
}

ATL::CComPtr<ICQGAccount> COrderPlacingDlg::GetSelectedAccount(long accID) const
{
	try
	{
		ATL::CComPtr<ICQGAccounts> spAccounts;
		HRESULT hr = m_spCQGCEL->get_Accounts(&spAccounts);
		AssertCOMError(m_spCQGCEL, hr);

		if (accID == -1)
			accID = static_cast<long>(m_cmbAccounts.GetItemData(m_cmbAccounts.GetCurSel()));

		ATL::CComPtr<ICQGAccount> spAccount;
		hr = spAccounts->get_Item(accID, &spAccount);
		AssertCOMError(spAccounts, hr);

		return spAccount;
	}
	catch (...)
	{
	}

	return NULL;
}

ATL::CComPtr<ICQGInstrument> COrderPlacingDlg::GetSelectedInstrument() const
{
   try
   {
      // Get selected instrument's row
      int row = m_lsInstruments.GetCurSel();
      if (row > -1)
      {
         // Get text from the selected row
         CString curText;
         m_lsInstruments.GetText(row, curText);

         ATL::CComBSTR fullName = ATL::CT2W(curText);

         // Get instruments collection
         ATL::CComPtr<ICQGInstruments> spInstruments;
         HRESULT hr = m_spCQGCEL->get_Instruments(&spInstruments);
         AssertCOMError(m_spCQGCEL, hr);

         // Get instrument from collection by it's full name
         ATL::CComVariant idx(fullName);
         ATL::CComPtr<ICQGInstrument> spInstrument;
         hr = spInstruments->get_Item(idx, &spInstrument);
         AssertCOMError(m_spCQGCEL, hr);

         return spInstrument;
      }
   }
   catch (...)
   {
   }

   return NULL;
}

void COrderPlacingDlg::PlaceOrder(
                                 eOrderSide orderSide)
{
   ATL::CComPtr<ICQGAccount> spAccount = GetSelectedAccount();

   ATL::CComPtr<ICQGInstrument> spInstrument = GetSelectedInstrument();

   // Get order type
   eOrderType orderType = static_cast<eOrderType>
                                     (m_cmbType.GetItemData(m_cmbType.GetCurSel()));

   // Get quantity
   long quantity = GetQuantity(m_edQuantity);

   // Get limit price and stop price 
   double limitPrice = 0;
   double stopPrice = 0;

   switch(orderType)
   {
      case otLimit:
         limitPrice = GetPrice(m_edLimitPrice, spInstrument);
         break;
      case otStop:
         stopPrice = GetPrice(m_edStopPrice, spInstrument);
         break;
      case otStopLimit:
         limitPrice = GetPrice(m_edLimitPrice, spInstrument);
         stopPrice = GetPrice(m_edStopPrice, spInstrument);
   }

   if (limitPrice == m_INVALID_DOUBLE || stopPrice == m_INVALID_DOUBLE)
   {
      // Limit price or/and stop price is incorrect
	   CString errMsg("Incorrect price for '");
	   errMsg = OrderTypeToString(orderType);
	   errMsg += _T("' order");
	   Z::report_alert("%s", CT2A(errMsg));

//	   AfxMessageBox(_T("Incorrect price for '") + OrderTypeToString(orderType) + _T("' order"));
      return;
   }

   // Create order
   ATL::CComPtr<ICQGOrder> spOrder;
   HRESULT hr = m_spCQGCEL->CreateOrder(orderType, spInstrument, spAccount, 
                           quantity, orderSide, limitPrice, 
                           stopPrice, L"", &spOrder);
   AssertCOMError(m_spCQGCEL, hr);

   // Put duration type of the order
   eOrderDuration orderDuration = static_cast<eOrderDuration>
                                    (m_cmbDuration.GetItemData(m_cmbDuration.GetCurSel()));
   hr = spOrder->put_DurationType(orderDuration);
   AssertCOMError(m_spCQGCEL, hr);

   // Get order properties to change some values
   ATL::CComPtr<ICQGOrderProperties> spProperties;
   hr = spOrder->get_Properties(&spProperties);
   AssertCOMError(spOrder, hr);

   if (orderDuration == odGoodTillDate || orderDuration == odGoodTillTime)
   {
      // Set GTD time property if order duration is GTD
      COleDateTime oleDateTime;
      m_dtpGTD.GetTime(oleDateTime);

      SetPropertyValue(spProperties, opGTDTime, ATL::CComVariant(oleDateTime, VT_DATE));
   }

   // Set parked property
   SetPropertyValue(spProperties, opParked, ATL::CComVariant(m_chkParked.GetCheck() == BST_CHECKED));

   // Place order
   hr = spOrder->Place();
   AssertCOMError(spOrder, hr);
}

void COrderPlacingDlg::PlaceOrder(LFNewOrderPtr* new_order_)
{
	auto_ptr<LFNewOrderPtr> nop(new_order_);  
	LFNewOrderPtr new_order = *new_order_;
	
	if(!m_GWConnectionUp)
	{
		auto_ptr<LFOrderUpdate> o(new LFOrderUpdate(*new_order));
		o->_order_status = LF::osRejectGW;
		o->_err_code = 1;
		o->_err_desc = "GW Connection is Down";
		BMModel::instance()->send(o.release());
		return;
	}

	try{
		HRESULT hr;
			// ------ Get IAccount ----------
		ATL::CComPtr<ICQGAccount> spAccount;
		if (new_order->_cqg_AccountID > 0)
		{
			ATL::CComPtr<ICQGAccounts> spAccounts;
			HRESULT hr = m_spCQGCEL->get_Accounts(&spAccounts);
			AssertCOMError(m_spCQGCEL, hr);

			hr = spAccounts->get_Item(new_order->_cqg_AccountID, &spAccount);
			AssertCOMError(spAccounts, hr);
		}
		else
		{
			// TOBE REMOVED
			spAccount = GetSelectedAccount();
			Z::report_error("PlaceOrder -- cqg_AccountID is not set in new order request");
		}
	
		// ------ Get Instrument ----------
		// Get instruments collection
		 ATL::CComPtr<ICQGInstruments> spInstruments;
		 hr = m_spCQGCEL->get_Instruments(&spInstruments);
		 AssertCOMError(m_spCQGCEL, hr);

		 // Get instrument from collection by it's full name
		 ATL::CComVariant idx(new_order->_cqg_InstrumentID);//new_order->_instr.c_str());
		 ATL::CComPtr<ICQGInstrument> spInstrument;
		 hr = spInstruments->get_Item(idx, &spInstrument);
		 AssertCOMError(m_spCQGCEL, hr);
		// -----------------------------------

		// ------- Get order type & side
		eOrderType orderType = static_cast<eOrderType>(new_order->_order_type);
		eOrderSide orderSide = static_cast<eOrderSide>(new_order->_order_side);                            
		// ------- Get quantity
		long quantity = new_order->_q;

		// ------- Get limit price and stop price 
		double limitPrice = new_order->_limit_p;
		double stopPrice = new_order->_stop_p;

	   if (limitPrice == m_INVALID_DOUBLE || stopPrice == m_INVALID_DOUBLE)
	   {
		  // Limit price or/and stop price is incorrect
		   CString errMsg("Incorrect price for '");
		   errMsg = OrderTypeToString(orderType);
		   errMsg += _T("' order");
		   Z::report_alert("%s", CT2A(errMsg));

//		   AfxMessageBox(_T("Incorrect price for '") + OrderTypeToString(orderType) + _T("' order"));
		  return;
	   }

	   // Create order
	   std::ostringstream os;
	   os << new_order->_order_id;
	   ATL::CComPtr<ICQGOrder> spOrder;

	   hr = m_spCQGCEL->CreateOrder(orderType, spInstrument, spAccount, 
							   quantity, orderSide, limitPrice, 
							   stopPrice, ATL::CA2W(os.str().c_str()), &spOrder);
	   AssertCOMError(m_spCQGCEL, hr);

	   // Put order id
	

		hr = spOrder->put_Tag(ATL::CA2W("_order_id"), ATL::CComVariant(new_order->_order_id));
		AssertCOMError(m_spCQGCEL, hr);

		hr = spOrder->put_Tag(ATL::CA2W("_strategy_id"), ATL::CComVariant(new_order->_strategy_id));
		AssertCOMError(m_spCQGCEL, hr);

		hr = spOrder->put_Tag(ATL::CA2W("_sub_strategy_id"), ATL::CComVariant(new_order->_sub_strategy_id));
		AssertCOMError(m_spCQGCEL, hr);
	
		hr = spOrder->put_Tag(ATL::CA2W("_strategy_key"), ATL::CComVariant(new_order->_strategy_key.c_str()));
		AssertCOMError(m_spCQGCEL, hr);

	   // Put duration type of the order
		eOrderDuration orderDuration = odDay;
		   //static_cast<eOrderDuration>(m_cmbDuration.GetItemData(m_cmbDuration.GetCurSel()));
	   
		hr = spOrder->put_DurationType(orderDuration);
	   AssertCOMError(m_spCQGCEL, hr);

	   // Get order properties to change some values
	   ATL::CComPtr<ICQGOrderProperties> spProperties;
	   hr = spOrder->get_Properties(&spProperties);
	   AssertCOMError(spOrder, hr);

	   //if (orderDuration == odGoodTillDate || orderDuration == odGoodTillTime)
	   //{
		  //// Set GTD time property if order duration is GTD
		  //COleDateTime oleDateTime;
		  //m_dtpGTD.GetTime(oleDateTime);

		  //SetPropertyValue(spProperties, opGTDTime, ATL::CComVariant(oleDateTime, VT_DATE));
	   //}

	   // Set parked property
//	   SetPropertyValue(spProperties, opParked, ATL::CComVariant(m_chkParked.GetCheck() == BST_CHECKED));

	   // Set opDescription to include TAF CO order
	   SetPropertyValue(spProperties, opDescription, ATL::CComVariant(os.str().c_str()));

	   // Place order
	   hr = spOrder->Place();
	   AssertCOMError(spOrder, hr);
	}
	catch (const std::runtime_error& e)
	{
		Z::report_error("PlaceOrder error: %s", e.what());
		auto_ptr<LFOrderUpdate> o(new LFOrderUpdate(*new_order));
		o->_order_status = LF::osRejectGW;
		o->_err_code = 1;
		o->_err_desc = e.what();
		BMModel::instance()->send(o.release());
	}

}
void COrderPlacingDlg::CancelOrder(LFCancelOrderPtr* cancel_order_)
{
	auto_ptr<LFCancelOrderPtr> cop(cancel_order_);  
	LFCancelOrderPtr cancel_order = *cancel_order_;
	HRESULT hr;
	if(cancel_order->_order_id == 0)
	{
		// ------ Get IAccount ----------
		ATL::CComPtr<ICQGAccount> spAccount = GetSelectedAccount(cancel_order->_cqg_AccountID);
		// Cancel all account's orders

		hr = m_spCQGCEL->CancelAllOrders(spAccount, NULL, VARIANT_FALSE, VARIANT_FALSE, osdUndefined);
		AssertCOMError(m_spCQGCEL, hr);
	}
	else
	{
		// Get selected order
		ATL::CComPtr<ICQGOrders> spOrders;
		HRESULT hr = m_spCQGCEL->get_Orders(&spOrders);
		AssertCOMError(m_spCQGCEL, hr);

		ATL::CComBSTR gwOrderID = ATL::CA2W(cancel_order->_cqg_GWOrderID.c_str());
		ATL::CComPtr<ICQGOrder> spOrder;
		hr = spOrders->get_Item(gwOrderID, &spOrder);
		AssertCOMError(spOrders, hr);
	
		hr = spOrder->Cancel();
		AssertCOMError(spOrder, hr);
	}
	UnselectAllOrders();
}

int COrderPlacingDlg::FilterTypeToOrderStatus(
                                 eFilterType filterType)
{
   int orderStatus;

   switch(filterType)
   {
      case ftWorking:
         orderStatus = osInOrderBook;
         break;
      case ftFilled:
         orderStatus = osFilled;
         break;
      case ftCancelled:
         orderStatus = osCanceled;
         break;
      case ftExceptions:
         orderStatus = osRejectGW;
         break;
      case ftParked:
         orderStatus = osParked;
         break;
      case ftAll:
         orderStatus = m_INVALID_INTEGER;
   }

   return orderStatus;
}

CString COrderPlacingDlg::OrderDurationToString(
                                 eOrderDuration duration)
{
   CString durationString;

   switch (duration)
   {
      case odATC:
         durationString = "ATC";
         break;
      case odATO:
         durationString = "ATO";
         break;
      case odDay:
         durationString = "DAY";
         break;
      case odFAK:
         durationString = "FAK";
         break;
      case odFOK:
         durationString = "FOK";
         break;
      case odGoodTillCanceled:
         durationString = "GTC";
         break;
      case odGoodTillDate:
         durationString = "GTD";
         break;
      case odGoodTillTime:
         durationString = "GTT";
         break;
      case odUndefined:
         durationString = "Undefined";
         break;
      default:
         assert(!"Unknown order duration");
   }

   return durationString;
}

CString COrderPlacingDlg::OrderSideToString(
                                 eOrderSide side)
{
   CString sideString;

   switch (side)
   {
      case osdBuy:
         sideString = "Buy";
         break;
      case osdSell:
         sideString = "Sell";
         break;
      case osdUndefined:
         sideString = "Undefined";
         break;
      default:
         assert(!"Unknown order side");
   }

   return sideString;
}

CString COrderPlacingDlg::OrderTypeToString(
                                 eOrderType type)
{
   CString typeString;

   switch (type)
   {
      case otLimit:
         typeString = "Limit";
         break;
      case otMarket:
         typeString = "Market";
         break;
      case otStop:
         typeString = "Stop";
         break;
      case otStopLimit:
         typeString = "Stop Limit";
         break;
      case otUndefined:
         typeString = "Undefined";
         break;
      default:
         assert(!"Unknown order type");
   }

   return typeString;
}

CString COrderPlacingDlg::OrderStatusToString(
                                 eOrderStatus status)
{
   CString statusString;

   switch (status)
   {
      case osActiveAt:
         statusString = "Active At";
         break;
      case osBusted:
         statusString = "Busted";
         break;
      case osCanceled:
         statusString = "Canceled";
         break;
      case osContingent:
         statusString = "Contingent";
         break;
      case osDisconnected:
         statusString = "Disconnected";
         break;
      case osExpired:
         statusString = "Expired";
         break;
      case osFilled:
         statusString = "Filled";
         break;
      case osInCancel:
         statusString = "In Cancel";
         break;
      case osInClient:
         statusString = "In Client";
         break;
      case osInModify:
         statusString = "In Modify";
         break;
      case osInOrderBook:
         statusString = "In Order Book";
         break;
      case osInTransit:
         statusString = "In Transit";
         break;
      case osInTransitTimeout:
         statusString = "In Transit Timeout";
         break;
      case osNotSent:
         statusString = "Not Sent";
         break;
      case osParked:
         statusString = "Parked";
         break;
      case osRejectFCM:
         statusString = "Reject FCM";
         break;
      case osRejectGW:
         statusString = "Reject GW";
         break;
      default:
         assert(!"Unknown order status");
   }

   return statusString;
}

CString COrderPlacingDlg::ConnectionStatusToString(
                                 eConnectionStatus status)
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