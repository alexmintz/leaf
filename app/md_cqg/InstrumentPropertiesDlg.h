// InstrumentPropertiesDlg.h : header file
//

#pragma once
#include "afxwin.h"

class CInstrumentPropertiesDlg;

/**
\typedef ATL::IDispEventImpl<1, CInstrumentPropertiesDlg, 
                             &__uuidof(_ICQGCELEvents), 
                             &__uuidof(__CQG), 4, 0> ICQGCELDispEventImpl
\brief  Convenient definition of IDispEventImpl instantiation
*/
typedef ATL::IDispEventImpl<1, 
                              CInstrumentPropertiesDlg,
                              &__uuidof(_ICQGCELEvents),
                              &__uuidof(__CQG), 4, 0
                              >
                           ICQGCELDispEventImpl;

/// \brief CInstrumentPropertiesDlg dialog
class CInstrumentPropertiesDlg : public CDialog, public ICQGCELDispEventImpl
{
public:
   CInstrumentPropertiesDlg(CWnd* pParent = NULL);	// standard constructor

   /// \brief Dialog Data
   enum { IDD = IDD_INSTRUMENTPROPERTIES_DIALOG };
   
   /// \brief Opens QFormula Dialog.
   afx_msg void OnBnClickedBtnQFormula();
   
   /// \brief Removes selected instrument from the list and the CQGCEL.
   afx_msg void OnBnClickedBtnRemove();
   
   /// \brief Removes all instrument from the list and the CQGCEL.
   afx_msg void OnBnClickedBtnRemoveAll();
   
   /// \brief Subscribes to the specified instrument.
   afx_msg void OnBnClickedBtnSubscribe();

   /// \brief Shows selected instrument data.
   afx_msg void OnLbnSelChangeLstInstruments();
   
   /// \brief Prepares the container for painting.
   afx_msg void OnPaint();

   /// \brief Called by a minimized (iconic) window that does not
   /// \brief have an icon defined for its class.
   afx_msg HCURSOR OnQueryDragIcon();

   /// \brief Used to unadvise from CQCEL events and shutdown it.
   afx_msg void OnDestroy();

   /// \brief Declares that the class defines a message map.
   DECLARE_MESSAGE_MAP()

protected:
   HICON m_hIcon;

   /// \brief Generated message map functions
   virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

   /// \brief The system calls this function to obtain the cursor to display while the user drags
   /// \brief the minimized window.
   virtual BOOL OnInitDialog();

   /// \brief Override to prevent dialog closing on Enter
   virtual void OnOK();

public:
   /// \brief This map is used to declare handler function for event specified event.
   /// \brief One can use "OLE/COM Object Viewer" to open CQGCEL-4_0.dll and get get event ids. 
   /// \brief The handler signature as well as the event ids can be found in the type library 
   /// \brief or shown by "OLE/COM Object Viewer". 
   BEGIN_SINK_MAP(CInstrumentPropertiesDlg)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 10, OnDataError)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 11, OnCELStarted)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 3, OnDataConnectionStatusChanged)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 4, OnInstrumentSubscribed)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 5, OnInstrumentChanged)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 6, OnInstrumentDOMChanged)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 12, OnIncorrectSymbol)
   END_SINK_MAP()

private:
   /// \brief Handle event of some abnormal discrepancy
   /// \brief between data expected and data received from CQG.
   /// \brief This event also can be fired when CQGCEL startup fails.
   /// \param obj - Object where the error occurred
   /// \param errorDescription - String describing the error
   /// \return S_OK on success
   STDMETHOD(OnDataError)(
                     LPDISPATCH obj, 
                     BSTR errorDescription);

   /// \brief OnCELStarted is fired when CQGCEL object 
   /// \brief is successfully started up.
   STDMETHOD(OnCELStarted)();

   /// \brief Fired when some changes occurred 
   /// \brief in the connection with the CQG data server.
   /// \param newStatus - New connection status
   /// \return S_OK on success
   STDMETHOD(OnDataConnectionStatusChanged)(eConnectionStatus newStatus);

   /// \brief Fired when new instrument is resolved and subscribed.
   /// \param symbl - Requested symbol
   /// \param instrument - Subscribed instrument object
   /// \return S_OK on success
   STDMETHOD(OnInstrumentSubscribed)(
                     BSTR symbl, 
                     ICQGInstrument* instrument);

   /// \brief Fired when any of instrument quotes or 
   /// \brief dynamic instrument properties are changed.
   /// \param instrument - Changed instrument object
   /// \param Quotes - Changed quotes collection
   /// \param props - Changed properties collection
   /// \return S_OK on success
   STDMETHOD(OnInstrumentChanged)(
                     ICQGInstrument* instrument, 
                     ICQGQuotes* quotes, 
                     ICQGInstrumentProperties* props);

   /// \brief Fired whenever instrument DOM is updated.
   /// \param instrument - Changed instrument object
   /// \param prevAsks - Asks old values collection
   /// \param prevBids - Bids old values collection
   /// \return S_OK on success
   STDMETHOD(OnInstrumentDOMChanged)(
                     ICQGInstrument* instrument, 
                     ICQGDOMQuotes* prevAsks, 
                     ICQGDOMQuotes* prevBids);

   /// \brief Fired when the request by the incorrect symbol is made.
   /// \param symbl - Requested symbol
   /// \return S_OK on success
   STDMETHOD(OnIncorrectSymbol)(BSTR symbl);
   
private:
   /// \brief Initialize all controls
   void InitializeControls();
   
   /// \brief Initialize properties list
   void InitializeProperties();

   /// \brief Initialize DOM list
//   void InitializeDOM();
   
   /// \brief Initialize sessions list
   void InitializeSessions();
   
   /// \brief Initialize CQGCEL object, and starts the CQGCEL
   void InitializeCQGCEL();

   /// \brief Clears all data connected with instrument
   void ClearAllData();
   
   /// \brief Clears quotes data shown on the form
//   void ClearQuotes();
   
   /// \brief Clears DOM data shown on the form
//   void ClearDOM();
   
   /// \brief Clears sessions data shown on the form
   void ClearSessions();
   
   /// \brief Clears properties data shown on the form
   void ClearProperties();

   /// \brief Shows all current quotes for specified instrument
   /// \param spInstrument - instrument object
   void ShowQuotes(const ATL::CComPtr<ICQGInstrument>& spInstrument);

   /// \brief Updates quotes on the form for specified instrument
   /// \param spInstrument - instrument object
/*
   void UpdateQuotes(   
                  const ATL::CComPtr<ICQGInstrument>& spInstrument,
                  const ATL::CComPtr<ICQGQuotes>& spQuotes);
*/
   /// \brief Publishes trades data through LEAF channel
   /// \param spInstrument - instrument object
   void PublishTrades(   
                  const ATL::CComPtr<ICQGInstrument>& spInstrument,
                  const ATL::CComPtr<ICQGQuotes>& spQuotes);
   /// \brief Publishes settlement prices through LEAF channel
   /// \param spInstrument - instrument object
   void PublishSettlement(
	   const ATL::CComPtr<ICQGInstrument>& spInstrument,
	   const ATL::CComPtr<ICQGQuotes>& spQuotes);
   /// \brief Returns display format of the price and volume from specified quote
   /// \param spInstrument [in,out] - instrument object
   /// \param quoteType [in,out]- Type of the quote
   /// \param displayPrice [out] - Formated price string
   /// \param displayVolume [out] - Formated price string
   void GetQuoteDisplayValues(
                        const ATL::CComPtr<ICQGInstrument>& spInstrument,
                        const ATL::CComPtr<ICQGQuote>& spQuote,
                        CString& displayPrice,
                        CString& displayVolume) const;

   /// \brief Updates DOM data on the form for specified instrument
   /// \param spInstrument - instrument object
 //  void UpdateDOM(const ATL::CComPtr<ICQGInstrument>& spInstrument);

   /// \brief Publishes DOM data through LEAF channel
   /// \param spInstrument - instrument object
   void PublishDOM(const ATL::CComPtr<ICQGInstrument>& spInstrument);

   /// \brief Shows sessions on the form for specified instrument
   /// \param spInstrument - instrument object
   void ShowSessions(const ATL::CComPtr<ICQGInstrument>& spInstrument);

   /// \brief Shows all properties on the form for specified instrument
   /// \param spInstrument - instrument object
   void ShowAllProperties(const ATL::CComPtr<ICQGInstrument>& spInstrument);

   /// \brief Updates specified property value on the form
   /// \param spProperty - Property to update
   void UpdateProperty(
                  const ATL::CComPtr<ICQGInstrument>& spInstrument,
                  const ATL::CComPtr<ICQGInstrumentProperty>& spProperty);

   /// \brief Updates all properties within specified collection on the form
   /// \param spProperties - Properties collection
   void UpdateProperties(
                  const ATL::CComPtr<ICQGInstrument>& spInstrument,
                  const ATL::CComPtr<ICQGInstrumentProperties>& spProperties);

   /// \brief Retrieves property from specified collection by specified type
   /// \param spProperties - instrument object
   /// \param propType - Property type to retrieve
   /// \return Property object
   ATL::CComPtr<ICQGInstrumentProperty> GetProperty(
                        const ATL::CComPtr<ICQGInstrumentProperties>& spProperties,
                        eInstrumentProperty propType) const;

   /// \brief Enable/disables controls. Connection affects on 
   /// \brief controls statuses.
   /// \param enable - Controls status
   void EnableControls(BOOL enable = TRUE);

   /// \brief Returns instrument name currently selected in list
   /// \return Selected row string
   ATL::CComBSTR GetSelectedFullName() const;

   /// \brief Returns instrument from CQGCEL by fullname
   /// \param fullname - Name of the instrument to retrieve
   /// \return instrument object
   ATL::CComPtr<ICQGInstrument> GetInstrument(const ATL::CComBSTR& fullname) const;

   /// \brief Returns instrument type string
   /// \param instrType
   /// \return String with type name
   static CString InstrumentTypeToString(eInstrumentType instrType);

   /// \brief Returns working days string from mask
   /// \param daysMask - Working days mask 
   /// \return String with working days 
   static CString WorkingDaysToString(eSessionWeekDays daysMask);

   /// \brief Returns string representation of the connection status.    
   /// \param status - connection status
   /// \return String representation of the connection status
   static CString ConnectionStatusToString(eConnectionStatus status);

private:
   /// \brief CQGCEL object
   ATL::CComPtr<ICQGCEL> m_spCQGCEL;
   
   /// \brief Data connection label control
   CStatic m_lbDataConnection;

   /// \brief Ask price label control
   CStatic m_lbAskPrice;

   /// \brief Trade price label control
   CStatic m_lbTradePrice;

   /// \brief Bid price label control
   CStatic m_lbBidPrice;

   /// \brief Ask volume label control
   CStatic m_lbAskVolume;

   /// \brief Trade volume label control
   CStatic m_lbTradeVolume;

   /// \brief Bid volume label control
   CStatic m_lbBidVolume;

   /// \brief Instrument description label control
   CStatic m_lblInstrDescription;

   /// \brief Instrument subscribe button control
   CButton m_btSubscribe;

   /// \brief Instrument remove button control
   CButton m_btRemove;

   /// \brief All instruments remove button control
   CButton m_btRemoveAll;

   /// \brief Symbol to subscribe edit box control
   CEdit m_edSymbolName;

   /// \brief All subscribed instruments list box control
   CListBox m_lsInstruments;

   /// \brief Selected instruments' properties list control
   CListCtrl m_lvProperties;

   /// \brief Selected instruments' DOM list control
//   CListCtrl m_lvDOM;

   /// \brief Selected instruments' sessions list control
   CListCtrl m_lvSessions;

    /// \brief QFormula button control
   CButton m_btnQFormula;
   
   /// \enum ePropertiesRow Properties rows in the list, this is used 
   /// to update some properties values

   /// \brief Properties rows count
   static const int m_stPropsCount; 

   /// \brief Properties rows count
   static const int m_stDOMCount;

public:
   /// \brief Not available value
   static const CString m_N_A;
   
};