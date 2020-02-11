// QFormulasDlg.cpp : implementation file
//

#include "stdafx.h"

#include <cassert>

#include "Common.h"
#include "InstrumentProperties.h"
#include "InstrumentPropertiesDlg.h"
#include "QFormulasDlg.h"

// CQFormulasDlg dialog

CQFormulasDlg::CQFormulasDlg(const ATL::CComPtr<ICQGCEL>& spCEL,
                              CWnd* pParent /*=NULL*/)
	: CDialog(CQFormulasDlg::IDD, pParent),
	   m_spCQGCEL(spCEL),
	   m_QFormulaType(qftName)
{
   // Now advise the connection, to get events
   assert(m_spCQGCEL != NULL);
   HRESULT hr = ICQGCELQFormulasDispEventImpl::DispEventAdvise(m_spCQGCEL);
   AssertCOMError(m_spCQGCEL, hr);
}

void CQFormulasDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_CMB_DEFINITION, m_cmbDefinition);
   DDX_Control(pDX, IDC_STQFORM_NUM, m_lblQFormulaNumber);
   DDX_Control(pDX, IDC_EDQFORM_EXP, m_edQFormulaExpression);
   DDX_Radio(pDX, IDC_RD_QFORM_BY_NAME, m_QFormulaType);
   DDX_Control(pDX, IDOK, m_btnOk);
}

void CQFormulasDlg::UnadviseFromCQGCEL()
{
   if (m_spCQGCEL)
   {
      HRESULT hr = ICQGCELQFormulasDispEventImpl::DispEventUnadvise(m_spCQGCEL);
      assert(SUCCEEDED(hr));
   }
}

BOOL CQFormulasDlg::OnInitDialog()
{
   CDialog::OnInitDialog();
   
   try
   {
      ClearAllData();
   }
   catch (const std::exception& e)
   {
      AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
   
   return TRUE;
}

BEGIN_MESSAGE_MAP(CQFormulasDlg, CDialog)
   ON_BN_CLICKED(IDC_BTN_REQUEST_DEFINITION, OnBnClickedBtnRequestDefinition)
   ON_CBN_SELCHANGE(IDC_CMB_DEFINITION, OnCbnSelchangeCmbDefinition)
   ON_WM_CLOSE()
   ON_WM_DESTROY()
END_MESSAGE_MAP()

void CQFormulasDlg::OnOK()
{
   UpdateData();
   
   try
   {
      CString qFormulaName;
      
      if (m_QFormulaType == qftName)
      {
         m_cmbDefinition.GetWindowText(qFormulaName);
      }
      else if (m_QFormulaType == qftNumber)
      {
         m_lblQFormulaNumber.GetWindowText(qFormulaName);
      }
      
      if (qFormulaName != "" && qFormulaName != CInstrumentPropertiesDlg::m_N_A)
      {
         m_qFormula = "Q";
         
         if (m_QFormulaType == qftName)
         {
            m_qFormula += ".";
         }
         
         m_qFormula += qFormulaName;
         
         CDialog::OnOK();
      }
      else
      {
         AfxMessageBox(_T("Select QFormula definition"));
      }
   }
   catch (std::exception& e)
   {
	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void CQFormulasDlg::OnDestroy()
{
   try
   {
      UnadviseFromCQGCEL();
   }
   catch (std::exception& e) 
   {
	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   CDialog::OnDestroy();
}

// CQFormulasDlg message handlers

void CQFormulasDlg::OnBnClickedBtnRequestDefinition()
{
   try
   {
      HRESULT hr = m_spCQGCEL->RequestQFormulaDefinitions();
      AssertCOMError(m_spCQGCEL, hr);

      ClearAllData();
   }
   catch (const std::exception& e)
   {
      AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void CQFormulasDlg::OnCbnSelchangeCmbDefinition()
{

   try
   {
      const long idx = static_cast<long>(m_cmbDefinition.GetItemData(m_cmbDefinition.GetCurSel()));

      // Get selected qformula definition
      ATL::CComPtr<ICQGQFormulaDefinition> spQFormulaDefinition;
      HRESULT hr = m_spQFormulaDefinitions->get_Item(idx, &spQFormulaDefinition);
      AssertCOMError(m_spQFormulaDefinitions, hr);
    
      // Set definition number in the label
      long number;
      hr = spQFormulaDefinition->get_Number(&number);
      AssertCOMError(spQFormulaDefinition, hr);

      CString numberStr;
      numberStr.Format(_T("%d"), number);
      m_lblQFormulaNumber.SetWindowText(numberStr);
      
      // Set definition expression in the label
      ATL::CComBSTR expressionStr;
      hr = spQFormulaDefinition->get_Expression(&expressionStr);
      AssertCOMError(spQFormulaDefinition, hr);

      m_edQFormulaExpression.SetWindowText(ATL::CW2T(expressionStr));
      
   }
   catch (const std::exception& e)
   {
      AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}
