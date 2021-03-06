// OrderPlacingDlg.h : header file
//

#include "znet/ZNet.h"
#include "znet/BMStrategy.h"
#include "leaf/LFData.h"

#include <map>
#pragma once
class COrderPlacingDlg;

/**
\typedef ATL::IDispEventImpl<1, COrderPlacingDlg, 
                              &__uuidof(_ICQGCELEvents), 
                              &__uuidof(__CQG), 4, 0> ICQGCELDispEventImpl
\brief  Convenient definition of IDispEventImpl instantiation
*/
typedef ATL::IDispEventImpl<1,
                              COrderPlacingDlg,
                              &__uuidof(_ICQGCELEvents),
                              &__uuidof(__CQG), 4, 0
                           >
                           ICQGCELDispEventImpl;

// COrderPlacingDlg dialog
class COrderPlacingDlg : public CDialog, public ICQGCELDispEventImpl
{

public:

   COrderPlacingDlg(CWnd* pParent = NULL);
   virtual ~COrderPlacingDlg();
   /// \enum Dialog Data
   enum { IDD = IDD_ORDERPLACING_DIALOG };

protected:

   /// \brief Dialog icon.
   HICON m_hIcon;

   /// \brief DDX/DDV support.
   virtual void DoDataExchange(
                  CDataExchange* pDX);

   /// \brief This message is sent to the dialog box immediately before the dialog box is displayed.
   /// \return Specifies whether the framework shall set the input 
   /// \return focus to the first control of the dialog box
   virtual BOOL OnInitDialog();

   /// \brief Override to prevent dialog from closing on Enter.
   virtual void OnOK();

protected:

   /// \brief Prepares the container for painting.
   afx_msg void OnPaint();

   /// \brief The system calls this function to obtain the cursor to
   /// \brief display while the user drags the minimized window.
   afx_msg HCURSOR OnQueryDragIcon();

   /// \brief Used to unadvise from CQGCEL events and shutdown it.
   afx_msg void OnDestroy();

   /// \brief Subscribes instrument.
   afx_msg void OnBnClickedBtnSubscribe();

   /// \brief Removes selected instrument from CQGCEL and list view control.
   afx_msg void OnBnClickedBtnRemoveInstr();

   /// \brief Removes all instruments from CQGCEL and list view control.
   afx_msg void OnBnClickedBtnRemoveAllInstr();

   /// \brief Changes selected instrument, and shows it's data.
   afx_msg void OnLbnSelChangeLstInstruments();

   /// \brief Changes filter type, and shows orders with specified filter type.
   /// \param pNMHDR - Contains information about a notification message
   /// \param pResult - Signed result of message processing
   afx_msg void OnTcnSelChangeTabOrders(
                  NMHDR *pNMHDR,
                  LRESULT *pResult);

   /// \brief Changes selected account, and shows it's orders.
   afx_msg void OnCbnSelChangeCmbAccounts();

   /// \brief Opens dialog to modify selected order.
   afx_msg void OnBnClickedBtnModify();

   /// \brief Activate selected order.
   afx_msg void OnBnClickedBtnActivate();

   /// \brief Activates all orders.
   afx_msg void OnBnClickedBtnActivateAll();

   /// \brief Cancels selected order.
   afx_msg void OnBnClickedBtnCancel();

   /// \brief Cancels all orders.
   afx_msg void OnBnClickedBtnCancelAll();

   /// \brief Changes and validates quantity.
   /// \param pNMHDR - Contains information about a notification message
   /// \param pResult - Signed result of message processing
   afx_msg void OnDeltaposSpQuantity(
                  NMHDR *pNMHDR,
                  LRESULT *pResult);

   /// \brief Validates quantity.
   afx_msg void OnEnKillfocusEdQuantity();

   /// \brief Changes stop price.
   /// \param pNMHDR - Contains information about a notification message
   /// \param pResult - Signed result of message processing
   afx_msg void OnDeltaposSpStopPrice(
                  NMHDR *pNMHDR,
                  LRESULT *pResult);

   /// \brief Changes limit price.
   /// \param pNMHDR - Contains information about a notification message
   /// \param pResult - Signed result of message processing
   afx_msg void OnDeltaposSpLimitPrice(
                  NMHDR *pNMHDR,
                  LRESULT *pResult);

   /// \brief Places sell order.
   afx_msg void OnBnClickedBtnSell();

   /// \brief Places buy order.
   afx_msg void OnBnClickedBtnBuy();

   /// \brief Changes selected order.
   /// \param pNMHDR - Contains information about a notification message
   /// \param pResult - Signed result of message processing
   afx_msg void OnLvnItemChangedLsvOrders(
                  NMHDR *pNMHDR,
                  LRESULT *pResult);

   /// \brief Changes order type.
   afx_msg void OnCbnSelChangeCmbType();

   /// \brief Changes order duration type.
   afx_msg void OnCbnSelChangeCmbDuration();

   /// \brief Declares that the class defines a message map.
   DECLARE_MESSAGE_MAP()

public:

   /// \brief This map is used to declare handler function for specified event.
   /// \brief You can use "OLE/COM Object Viewer" to open CQGCEL-4_0.dll and get event ids.
   /// \brief The handler signature as well as the event ids can be found in the type library
   /// \brief or shown by "OLE/COM Object Viewer".
   BEGIN_SINK_MAP(COrderPlacingDlg)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 10, OnDataError)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 2, OnGWConnectionStatusChanged)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 3, OnDataConnectionStatusChanged)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 7, OnAccountChanged)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 4, OnInstrumentSubscribed)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 5, OnInstrumentChanged)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 12, OnIncorrectSymbol)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 17, OnOrderChanged)
   END_SINK_MAP()

protected:

   /// \brief Handle event of some abnormal discrepancy
   /// \brief between data expected and data received from CQG.
   /// \brief This event also can be fired when CQGCEL startup fails.
   /// \param obj - Object where the error occurred
   /// \param errorDescription - String describing the error
   /// \return S_OK
   STDMETHOD(OnDataError)(
                  LPDISPATCH obj,
                  BSTR errorDescription);

   /// \brief Fired when some changes occurred
   /// \brief in the connection with the CQG Gateway.
   /// \param newStatus - New connection status
   /// \return S_OK
   STDMETHOD(OnGWConnectionStatusChanged)(
                  eConnectionStatus newStatus);

   /// \brief Fired when some changes occurred
   /// \brief in the connection with the CQG data server.
   /// \param newStatus - New connection status
   /// \return S_OK
   STDMETHOD(OnDataConnectionStatusChanged)(
                  eConnectionStatus newStatus);

   /// \brief Fired when new account is added, changed or removed.
   /// \param change - Account change type
   /// \param account - Changed account instance
   /// \param position - Changed position instance
   /// \return S_OK
   STDMETHOD(OnAccountChanged)(
                  eAccountChangeType change,
                  ICQGAccount* account,
                  ICQGPosition* position);

   /// \brief Fired when new instrument is resolved and subscribed.
   /// \param symbol - Requested symbol
   /// \param instrument - Subscribed instrument object
   /// \return S_OK
   STDMETHOD(OnInstrumentSubscribed)(
                  BSTR symbol, 
                  ICQGInstrument* instrument);

   /// \brief Fired when any of instrument quotes or
   /// \brief dynamic instrument properties are changed.
   /// \param instrument - Changed instrument object
   /// \param quotes - Changed quotes collection
   /// \param props - Changed properties collection
   /// \return S_OK
   STDMETHOD(OnInstrumentChanged)(
                  ICQGInstrument* instrument,
                  ICQGQuotes* quotes,
                  ICQGInstrumentProperties* props);

   /// \brief This event is fired when a not tradable symbol name has been 
   /// \brief passed to the NewInstrument method.
   /// \param symbl - Requested symbol
   /// \return S_OK
   STDMETHOD(OnIncorrectSymbol)(
                  BSTR symbl);

   /// \brief Fired when new order was added, order status was 
   /// \brief changed or the order was removed.
   /// \param change - The type of the occurred change
   /// \param order - CQGOrder object representing the order to which the change refers
   /// \param oldProperties - CQGOrderProperties collection representing 
   /// \param the old values of the changed order properties
   /// \param fill - CQGFill object representing the last fill of the order
   /// \param cqgerr - CQGError object to describe the last error, if any
   /// \return S_OK
   STDMETHOD(OnOrderChanged)(
                  eChangeType change, 
                  ICQGOrder* order, 
                  ICQGOrderProperties* oldProperties, 
                  ICQGFill* fill, 
                  ICQGError* cqgerr);

private:

   /// \enum eOrdersColumn. Represents orders columns in the list.
   enum eOrdersColumn;

   /// \enum eFilterType. Filter statuses.
   enum eFilterType;

private:

	/// \brief Initialize zNet subscriptions
	void InitializeZSubs();
	void UninitializeZSubs();
	bool process_rt_run();
	void process_order(LFNewOrderPtr*);
	void process_cancel(LFCancelOrderPtr*);
//	void process_amend(LFOrderModifyPtr*);

   /// \brief Initializes CQGCEL object, and starts the CQGCEL.
   void InitializeCQGCEL();

   /// \brief Initializes all enable/disable booleans.
   void InitializeBooleans();

   /// \brief Initializes all controls.
   void InitializeControls();

   /// \brief Initializes orders tab control.
   void InitializeOrdersTab();

   /// \brief Initializes orders list control.
   void InitializeOrdersList();

   /// \brief Initializes types combo control.
   void InitializeTypesCombo();

   /// \brief Initializes durations combo control.
   void InitializeDurationsCombo();

   /// \brief Clears quotes data shown on the form.
   void ClearQuotes();

   /// \brief Clears instrument's description label control.
   void ClearDescription();

   /// \brief Clears instrument's all data shown on the form.
   void ClearAllInstrumentData();

   /// \brief Clears all orders from the list control.
   void ClearOrders();

   /// \brief Shows all current quotes on the form for specified instrument.
   /// \param spInstrument - ICQGInstrument instance
   void ShowQuotes(
                  const ATL::CComPtr<ICQGInstrument>& spInstrument);

   /// \brief Shows specified quote in the labels.
   /// \param spQuotes - ICQGQuotes instance
   /// \param quoteType - Quote type to show
   /// \param priceLabel - Price label control
   /// \param volumeLabel - Volume label control
   void ShowQuote(
                  const ATL::CComPtr<ICQGQuotes>& spQuotes,
                  eQuoteType quoteType,
                  CStatic& priceLabel,
                  CStatic& volumeLabel);
   
   /// \brief Shows specified instrument's description in the label control.
   /// \param spInstrument - ICQGInstrument instance
   void ShowDescription(
                  const ATL::CComPtr<ICQGInstrument>& spInstrument);

   /// \brief Returns price of specified instrument in Trade/Ask/Bid quotes order.
   /// \param spInstrument - ICQGInstrument instance
   double GetValidPrice(
                  const ATL::CComPtr<ICQGInstrument>& spInstrument);

   /// \brief Shows specified instrument's valid price in LimitPrice and/or StopPrice label control(s).
   /// \param spInstrument - ICQGInstrument instance
   void COrderPlacingDlg::ShowValidPrice(
                                 const ATL::CComPtr<ICQGInstrument>& spInstrument);

   /// \brief Enables/disables controls on the form.
   void EnableControls();

   /// \brief Returns display format of the price and volume from specified quote.
   /// \param spQuote [in] - ICQGQuote instance
   /// \param displayPrice [out] - Formatted price string
   /// \param displayVolume [out] - Formatted volume string
   static void GetQuoteDisplayValues(
                  const ATL::CComPtr<ICQGQuote>& spQuote,
                  CString& displayPrice,
                  CString& displayVolume);

   /// \brief Fills all accounts to combo box control and changes 
   /// \brief their subscription levels.
   void ReloadAccounts();

   /// \brief Adds account's description to the accounts combo box control.
   /// \param spAccount - ICQGAccount instance
   void InsertAccount(
                  const ATL::CComPtr<ICQGAccount>& spAccount);

   /// \brief Returns instrument from CQGCEL currently selected in the list.
   /// \return ICQGInstrument instance
   ATL::CComPtr<ICQGInstrument> GetSelectedInstrument() const;

   /// \brief Returns selected account in the combo.
   /// \return ICQGAccount instance
   ATL::CComPtr<ICQGAccount> GetSelectedAccount(long accId = -1) const;
   
   /// \brief Shows orders by selected status in the list control.
   void ShowFilteredOrders();

   /// \brief Unselects all orders.
   void UnselectAllOrders();

   /// \brief Shows specified orders in the list control.
   /// \param spOrders - ICQGOrders instance
   void ShowOrders(
                  const ATL::CComPtr<ICQGOrders>& spOrders);

   /// \brief Returns specified orders row in the list control.
   /// \param spOrder - ICQGOrder instance
   /// \return Orders row as integer
   int GetOrderRow(
                  const ATL::CComPtr<ICQGOrder>& spOrder);

   /// \brief Adds, updates or removes specified order in the list control.
   /// \param spOrder - ICQGOrder instance
   /// \param change - Change type
   void ShowOrder(
                  const ATL::CComPtr<ICQGOrder>& spOrder,
                  eChangeType change,
				  ICQGFill* fill = 0, 
                  ICQGError* cqgerr = 0,
				  long account = 0);

   void SendOrder(
                  const ATL::CComPtr<ICQGOrder>& spOrder,
                  eChangeType change,
				  ICQGFill* fill, 
                  ICQGError* cqgerr,
				  long account);

   /// \brief Places order.
   /// \param orderSide - Placed order side.
   void PlaceOrder(
                  eOrderSide orderSide);
   void PlaceOrder(LFNewOrderPtr* new_order_);
   void CancelOrder(LFCancelOrderPtr* cancel_order_);
   /// \brief Validates and returns display format of specified price.
   /// \param spOrder - Order which price is formated 
   /// \param price - Price which is formated
   /// \return String representation of price
   CString GetDisplayPrice(
                  const ATL::CComPtr<ICQGOrder> spOrder,
                  double price) const;

   /// \brief Used for getting CQGCEL properties values string representation.
   /// \brief If specified value is invalid(see CQGCEL.IsValid() method) then "N/A" is returned.
   /// \param value - variant value
   /// \param valueType - variant type
   /// \return String representation of the value
   CString GetValueAsString(
                  ATL::CComVariant value,
                  VARENUM valueType = VT_EMPTY) const;

   /// \brief Returns order status, by which orders will be filtered.
   /// \param filterType - filter type
   /// \return Order status as integer
   int FilterTypeToOrderStatus(
                  eFilterType filterType);

   /// \brief Returns string representation of the specified order type.
   /// \param type - order type
   /// \return String representation of the order type
   static CString OrderTypeToString(
                  eOrderType type);

   /// \brief Returns string representation of the specified order status.
   /// \param status - order status
   /// \return String representation of the order status
   static CString OrderStatusToString(
                  eOrderStatus status);

   /// \brief Returns string representation of the specified order duration.
   /// \param duration - order duration
   /// \return String representation of the order duration
   static CString OrderDurationToString(
                  eOrderDuration duration);

   /// \brief Returns string representation of the specified order side.
   /// \param side - order side
   /// \return String representation of the order side
   static CString OrderSideToString(
                  eOrderSide side);

   /// \brief Returns string representation of the connection status.    
   /// \param status - connection status
   /// \return String representation of the connection status
   static CString ConnectionStatusToString(
                  eConnectionStatus status);

public:

   /// \brief Changes specified property's value.
   /// \param spProperties - ICQGOrderProperties instance
   /// \param propertyType - Property type, which value must be changed
   /// \param propertyValue - Property value
   static void SetPropertyValue(
                  ATL::CComPtr<ICQGOrderProperties>& spProperties,
                  eOrderProperty propertyType,
                  ATL::CComVariant propertyValue);

   /// \brief Changes specified edit box's price.
   /// \param editCtrl - Edit box control, which price will be changed
   /// \param spInstrument - ICQGInstrument instance
   /// \param delta - Change type
   static void ChangePrice(
                  CEdit& editCtrl,
                  ATL::CComPtr<ICQGInstrument> spInstrument,
                  int delta);

   /// \brief Returns specified edit box's price.
   /// \param editCtrl - Edit box control, which price will be returned
   /// \param spInstrument - ICQGInstrument instance
   /// \return Price
   static double GetPrice(
                  CEdit& editCtrl,
                  ATL::CComPtr<ICQGInstrument> spInstrument);

   /// \brief Returns quantity from the specified edit box.
   /// \param editCtrl - Edit box control, which quantity will be returned.
   /// \return Quantity
   static long GetQuantity(
                  CEdit& editCtrl);

   /// \brief Returns valid value for specified quantity.
   /// \param quantity - Quantity.
   /// \return Valid quantity
   static long GetValidQuantity(
                  long quantity);

   /// \brief Sets quantity to edit box.
   /// \param editCtrl - Edit box control, which quantity will be changed.
   /// \param quantity - Quantity.
   static void SetQuantity(
                  CEdit& editCtrl,
                  long quantity);

private:

   /// \brief CQGCEL object.
   ATL::CComPtr<ICQGCEL> m_spCQGCEL;

   /// \brief Selected order object.
   ATL::CComPtr<ICQGOrder> m_spSelectedOrder;

   /// \brief Not available value.
   static const CString m_N_A;

   /// \brief Not short date format string.
   static const CString m_SHORT_DATE;
   
   /// \brief Quantity maximum value.
   static const long m_MAX_QUANTITY;

   /// \brief Quantity minimum value.
   static const long m_MIN_QUANTITY;

   /// \brief Invalid double value.
   static const double m_INVALID_DOUBLE;

   /// \brief Invalid integer value.
   static const int m_INVALID_INTEGER;

   /// \brief Used to enable/disable controls,
   /// \brief True, when data connection is up.
   bool m_DataConnectionUp;

   /// \brief Used to enable/disable controls,
   /// \brief True, when GW connection is up.
   bool m_GWConnectionUp;

   /// \brief Used to enable/disable controls,
   /// \brief True, when limit price is enabled.
   bool m_LimitPriceEnabled;

   /// \brief Used to enable/disable controls,
   /// \brief True, when stop price is enabled.
   bool m_StopPriceEnabled;

   /// \brief Used to enable/disable controls,
   /// \brief True, when good till date is enabled.
   bool m_GTDEnabled;

private:

   /// \brief Data connection indicator label control.
   CStatic m_lblDataConnection;

   /// \brief GW connection indicator label control.
   CStatic m_lblGWConnection;

   /// \brief Accounts combo box control
   /// \brief Contains account's GW names, as well as the GWID as item data.
   /// \brief GWID is used to get account instance from CQGCEL accounts collection.
   CComboBox m_cmbAccounts;

   /// \brief Ask price label control.
   CStatic m_lblAskPrice;

   /// \brief Trade price label control.
   CStatic m_lblTradePrice;

   /// \brief Bid price label control.
   CStatic m_lblBidPrice;

   /// \brief Ask volume label control.
   CStatic m_lblAskVolume;

   /// \brief Trade volume label control.
   CStatic m_lblTradeVolume;

   /// \brief Bid volume label control.
   CStatic m_lblBidVolume;

   /// \brief Symbol to subscribe edit box control.
   CEdit m_edSymbolName;

   /// \brief Instrument subscribe button control.
   CButton m_btnSubscribe;

   /// \brief All subscribed instruments list box control.
   CListBox m_lsInstruments;

   /// \brief Instrument description label control.
   CStatic m_lblInstrDescription;

   /// \brief Orders tab control.
   CTabCtrl m_tabOrders;

   /// \brief Orders list view control.
   CListCtrl m_lvOrders;

   /// \brief Orders types combo box control.
   CComboBox m_cmbType;

   /// \brief Orders durations combo box control.
   CComboBox m_cmbDuration;

   /// \brief Limit price spin button control.
   CSpinButtonCtrl m_spnLimitPrice;

   /// \brief Stop price spin button control.
   CSpinButtonCtrl m_spnStopPrice;

   /// \brief Quantity spin button control.
   CSpinButtonCtrl m_spnQuantity;

   /// \brief Parked check box control.
   CButton m_chkParked;

   /// \brief Modify order button control.
   CButton m_btnModify;

   /// \brief Activate order button control.
   CButton m_btnActivate;

   /// \brief Activate all orders button control.
   CButton m_btnActivateAll;

   /// \brief Cancel order button control.
   CButton m_btnCancel;

   /// \brief Cancel all orders button control.
   CButton m_btnCancelAll;

   /// \brief Remove instrument button control.
   CButton m_btnRemove;

   /// \brief Remove all instruments button control.
   CButton m_btnRemoveAll;

   /// \brief Good till date datetime control.
   CDateTimeCtrl m_dtpGTD;

   /// \brief Order quantity edit box control.
   CEdit m_edQuantity;

   /// \brief Order limit price edit box control.
   CEdit m_edLimitPrice;

   /// \brief Order stop price edit box control.
   CEdit m_edStopPrice;

   /// \brief Buy order button control.
   CButton m_btnBuy;

   /// \brief Sell order button control.
   CButton m_btnSell;

private:
	auto_ptr<BMStrategy>		_s;
	auto_ptr<Z::ActiveQueue>	_star;
//	typedef std::map<long, long> AccountStrategyMap;
//	AccountStrategyMap _routing;
};

enum COrderPlacingDlg::eFilterType
{
   ftWorking,
   ftFilled,
   ftCancelled,
   ftExceptions,
   ftParked,
   ftAll
};

enum COrderPlacingDlg::eOrdersColumn
{
   ocID,
   ocSide,
   ocSize,
   ocState,
   ocInstrument,
   ocType,
   ocLimitPrice,
   ocStopPrice,
   ocDuration,
   ocAvgFillPrice,
   ocPlaceTime,
   ocGWOrderID
};
