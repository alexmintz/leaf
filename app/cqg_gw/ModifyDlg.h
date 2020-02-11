// ModifyDlg.h : header file
//

#pragma once

// CModifyDlg dialog
class CModifyDlg : public CDialog
{

public:
   CModifyDlg(
            const ATL::CComPtr<ICQGOrder>& spOrder,
            const CString& limitDisplayPrice,
            const CString& stopDisplayPrice,
            CWnd* pParent = NULL);

   /// \enum Dialog Data.
   enum { IDD = IDD_MODIFY_DIALOG };

   /// \brief Changes limit price.
   /// \param pNMHDR - Contains information about a notification message
   /// \param pResult - Signed result of message processing
   afx_msg void OnDeltaposSpMdfLimitPrice(
                     NMHDR *pNMHDR,
                     LRESULT *pResult);

   /// \brief Changes stop price.
   /// \param pNMHDR - Contains information about a notification message
   /// \param pResult - Signed result of message processing
   afx_msg void OnDeltaposSpMdfStopPrice(
                     NMHDR *pNMHDR,
                     LRESULT *pResult);

   /// \brief Changes and validates quantity.
   /// \param pNMHDR - Contains information about a notification message
   /// \param pResult - Signed result of message processing
   afx_msg void OnDeltaposSpMdfQuantity(
                     NMHDR *pNMHDR,
                     LRESULT *pResult);

   /// \brief Validates quantity.
   afx_msg void OnEnKillfocusEdMdfQuantity();

   /// \brief Declares that the class defines a message map.
   DECLARE_MESSAGE_MAP()

protected:
   
   /// \brief DDX/DDV support.
   virtual void DoDataExchange(
                     CDataExchange* pDX);

   /// \brief This message is sent to the dialog box immediately before the dialog box is displayed.
   /// \return Specifies whether the framework shall set the input 
   /// \return focus to the first control of the dialog box
   virtual BOOL OnInitDialog();

   /// \brief Override to prevent dialog from closing on Enter.
   virtual void OnOK();

private:

   /// \brief Checks, which properties can be modified.
   void InitializeProperties();

   /// \brief Initializes all controls.
   void InitializeControls();

private:

   /// \brief Quantity edit box control.
   CEdit m_edMdfQuantity;

   /// \brief Limit price edit box control.
   CEdit m_edMdfLimitPrice;

   /// \brief Stop price edit box control.
   CEdit m_edMdfStopPrice;

   /// \brief Quantity spin button control.
   CSpinButtonCtrl m_spnMdfQuantity;

   /// \brief Limit price spin button control.
   CSpinButtonCtrl m_spnMdfLimitPrice;

   /// \brief Stop price spin button control.
   CSpinButtonCtrl m_spnMdfStopPrice;

private:

   /// \brief CQGOrder object.
   ATL::CComPtr<ICQGOrder> m_spOrder;

   /// \brief CQGOrderModify object.
   ATL::CComPtr<ICQGOrderModify> m_spOrderModify;

   /// \brief Limit price in display format.
   CString m_LimitDisplayPrice;

   /// \brief Stop price in display format.
   CString m_StopDisplayPrice;

   /// \brief Defines if quantity can be modified.
   bool m_QuantityCBM;

   /// \brief Defines if limit price can be modified.
   bool m_LimitPriceCBM;

   /// \brief Defines if stop price can be modified.
   bool m_StopPriceCBM;
};
