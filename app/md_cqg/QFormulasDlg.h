// QFormulasDlg.h : header file
//

#pragma once
#include "afxwin.h"

class CQFormulasDlg;

/**
\typedef ATL::IDispEventImpl<1, CQFormulasDlg, 
                             &__uuidof(_ICQGCELEvents), 
                             &__uuidof(__CQG), 4, 0> ICQGCELQFormulasDispEventImpl
\brief  Convenient definition of IDispEventImpl instantiation
*/
typedef ATL::IDispEventImpl<1, 
                              CQFormulasDlg,
                              &__uuidof(_ICQGCELEvents),
                              &__uuidof(__CQG), 4, 0
                              >
                           ICQGCELQFormulasDispEventImpl;

// CQFormulasDlg dialog

class CQFormulasDlg : public CDialog, public ICQGCELQFormulasDispEventImpl
{

public:
	CQFormulasDlg(const ATL::CComPtr<ICQGCEL>& spCEL,
	               CWnd* pParent = NULL);   // standard constructor
   
   // Dialog Data
	enum { IDD = IDD_QFORMULAS_DIALOG };
   
   /// \brief Requests qformula definitions.
   afx_msg void OnBnClickedBtnRequestDefinition();
   
   /// \brief Updates definition data and parameters on the form.
   afx_msg void OnCbnSelchangeCmbDefinition();
   
   /// \brief Used to unadvise from CQGCEL events on dialog destroy.
   afx_msg void OnDestroy();
	
   /// \brief This map is used to declare handler function for event specified event.
   /// \brief One can use "OLE/COM Object Viewer" to open CQGCEL-4_0.dll and get get event ids. 
   /// \brief The handler signature as well as the event ids can be found in the type library 
   /// \brief or shown by "OLE/COM Object Viewer". 
   BEGIN_SINK_MAP(CQFormulasDlg)
      SINK_ENTRY_EX(1, __uuidof(_ICQGCELEvents), 42, OnQFormulaDefinitionsResolved)
   END_SINK_MAP()
	
	DECLARE_MESSAGE_MAP()

protected:
   /// \brief Generated message map functions
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	/// \brief This message is sent to the dialog box immediately before the dialog box is displayed.
   /// \return Specifies whether the application has set the input 
   /// \return focus to one of the controls in the dialog box
   virtual BOOL OnInitDialog();
	
	/// \brief Override to prevent dialog from closing on Enter.
	virtual void OnOK();

private:
   /// \enum eQFormulaType. Represents QFormula type.
   enum eQFormulaType;
   
   /// \brief Unadvises from CQGCEL events.
   void UnadviseFromCQGCEL();
   
   /// \brief Clears all data connected with instrument
   void ClearAllData();

protected:
   /// \brief Fired when the definitions of qFormula are resolved
   /// \brief or when some error occurred during request processing.
   /// \param cqg_qformula_definitions - collection of qformula definitions
   /// \param cqg_error - CQGError object describing last error occurred during the request processing
   /// \return S_OK
   STDMETHOD(OnQFormulaDefinitionsResolved)(
                  ICQGQFormulaDefinitions* cqg_qformula_definitions, 
                  ICQGError* cqg_error);

private:
   /// \brief CQGCEL object
   ATL::CComPtr<ICQGCEL> m_spCQGCEL;
   
   /// \brief Resolved qformula definitions object.
   ATL::CComPtr<ICQGQFormulaDefinitions> m_spQFormulaDefinitions;

   /// \brief QFormula definitions ComboBox control
   CComboBox m_cmbDefinition;
   
   /// \brief QFormula number label control
   CStatic m_lblQFormulaNumber;
   
   /// \brief QFormula expression edit box control
   CEdit m_edQFormulaExpression;

   /// \brief QFormula's type.
   int m_QFormulaType;
   
   /// \brief QFormula for subscription.
   CString m_qFormula;
   
   /// \brief OK button control
   CButton m_btnOk;

public:
   
   /// \brief Returns QFormula name currently selected in combobox
   /// \return Selected QFormula name according on QFormula type
   const CString& GetQFormulaName() const;
   
};

/// \enum eQFormulaType. Represents QFormula type.
enum CQFormulasDlg::eQFormulaType
{
   qftName,
   qftNumber
};