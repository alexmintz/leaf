// ModifyDlg.cpp : implementation file
//

#include "stdafx.h"
#include <stdexcept>
#include "Common.h"
#include "OrderPlacing.h"
#include "OrderPlacingDlg.h"
#include "ModifyDlg.h"

CModifyDlg::CModifyDlg(
                  const ATL::CComPtr<ICQGOrder>& spOrder,
                  const CString& limitDisplayPrice,
                  const CString& stopDisplayPrice,
                  CWnd* pParent/*=NULL*/)
                     : CDialog(CModifyDlg::IDD, pParent),
                       m_spOrder(spOrder),
                       m_LimitDisplayPrice(limitDisplayPrice),
                       m_StopDisplayPrice(stopDisplayPrice)
{
}

void CModifyDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Control(pDX, IDC_ED_MDF_LIMIT_PRICE, m_edMdfLimitPrice);
   DDX_Control(pDX, IDC_ED_MDF_STOP_PRICE, m_edMdfStopPrice);
   DDX_Control(pDX, IDC_ED_MDF_QUANTITY, m_edMdfQuantity);
   DDX_Control(pDX, IDC_SPN_MDF_QUANTITY, m_spnMdfQuantity);
   DDX_Control(pDX, IDC_SPN_MDF_LIMIT_PRICE, m_spnMdfLimitPrice);
   DDX_Control(pDX, IDC_SPN_MDF_STOP_PRICE, m_spnMdfStopPrice);
}

BOOL CModifyDlg::OnInitDialog()
{
   try
   {
      CDialog::OnInitDialog();

      // Prepare modify order
      HRESULT hr = m_spOrder->PrepareModify(&m_spOrderModify);
      AssertCOMError(m_spOrder, hr);

      InitializeProperties();
      InitializeControls();
   }
   catch (std::exception& e) 
   {
	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   return TRUE;
}

void CModifyDlg::InitializeProperties()
{
   ATL::CComPtr<ICQGOrderProperties> spProperties;
   HRESULT hr = m_spOrderModify->get_Properties(&spProperties);
   AssertCOMError(m_spOrderModify, hr);

   // Check, which properties can be modified
   ATL::CComPtr<ICQGOrderProperty> spProperty;
   hr = spProperties->get_Item(opQuantity, &spProperty);
   AssertCOMError(spProperties, hr);
   m_QuantityCBM = (spProperty != NULL);

   spProperty.Release();
   hr = spProperties->get_Item(opLimitPrice, &spProperty);
   AssertCOMError(spProperties, hr);
   m_LimitPriceCBM = (spProperty != NULL);

   spProperty.Release();
   hr = spProperties->get_Item(opStopPrice, &spProperty);
   AssertCOMError(spProperties, hr);
   m_StopPriceCBM = (spProperty != NULL);
}

void CModifyDlg::InitializeControls()
{
   long quantity;
   HRESULT hr = m_spOrder->get_Quantity(&quantity);
   AssertCOMError(m_spOrder, hr);

   // Set quantity
   COrderPlacingDlg::SetQuantity(m_edMdfQuantity, quantity);

   // Disable quantity controls, if cannot be modified
   m_edMdfQuantity.EnableWindow(m_QuantityCBM);
   m_spnMdfQuantity.EnableWindow(m_QuantityCBM);

   // Set limit price
   m_edMdfLimitPrice.SetWindowText(m_LimitDisplayPrice);

   // Disable limit price controls, if cannot be modified
   m_edMdfLimitPrice.EnableWindow(m_LimitPriceCBM);
   m_spnMdfLimitPrice.EnableWindow(m_LimitPriceCBM);

   // Set stop price
   m_edMdfStopPrice.SetWindowText(m_StopDisplayPrice);

   // Disable stop price controls, if cannot be modified
   m_edMdfStopPrice.EnableWindow(m_StopPriceCBM);
   m_spnMdfStopPrice.EnableWindow(m_StopPriceCBM);
}

BEGIN_MESSAGE_MAP(CModifyDlg, CDialog)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPN_MDF_LIMIT_PRICE, OnDeltaposSpMdfLimitPrice)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPN_MDF_STOP_PRICE, OnDeltaposSpMdfStopPrice)
   ON_NOTIFY(UDN_DELTAPOS, IDC_SPN_MDF_QUANTITY, OnDeltaposSpMdfQuantity)
   ON_EN_KILLFOCUS(IDC_ED_MDF_QUANTITY, OnEnKillfocusEdMdfQuantity)
END_MESSAGE_MAP()

void CModifyDlg::OnOK()
{
   try
   {
      ATL::CComPtr<ICQGInstrument> spInstrument;
      HRESULT hr = m_spOrder->get_Instrument(&spInstrument);
      AssertCOMError(m_spOrder, hr);

      // Get quantity
      long quantity = COrderPlacingDlg::GetQuantity(m_edMdfQuantity);

      // Get limit price
      double limitPrice = COrderPlacingDlg::GetPrice(m_edMdfLimitPrice, spInstrument);

      // Get stop price
      double stopPrice = COrderPlacingDlg::GetPrice(m_edMdfStopPrice, spInstrument);

      ATL::CComPtr<ICQGOrderProperties> spProperties;
      hr = m_spOrderModify->get_Properties(&spProperties);
      AssertCOMError(m_spOrderModify, hr);

      if (m_QuantityCBM)
      {
         // Set quantity value, if can be modified
         COrderPlacingDlg::SetPropertyValue(spProperties, opQuantity, ATL::CComVariant(quantity));
      }

      if (m_LimitPriceCBM)
      {
         // Set limit price value, if can be modified
         COrderPlacingDlg::SetPropertyValue(spProperties, opLimitPrice, ATL::CComVariant(limitPrice));
      }

      if (m_StopPriceCBM)
      {
         // Set stop price value, if can be modified
         COrderPlacingDlg::SetPropertyValue(spProperties, opStopPrice, ATL::CComVariant(stopPrice));
      }
   
      // Check, if selected order can be modified
      VARIANT_BOOL canBeModified;
      hr = m_spOrder->get_CanBeModified(&canBeModified);
      AssertCOMError(m_spOrder, hr);

      if (canBeModified)
      {
         // Modify selected order
         hr = m_spOrder->Modify(m_spOrderModify);
         AssertCOMError(m_spOrder, hr);
      }

      CDialog::OnOK();
   }
   catch (std::exception& e)
   {
	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void CModifyDlg::OnDeltaposSpMdfQuantity(NMHDR *pNMHDR, LRESULT *pResult)
{
   try
   {
      LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
      long quantity = COrderPlacingDlg::GetQuantity(m_edMdfQuantity);
      quantity -= pNMUpDown->iDelta;

      COrderPlacingDlg::SetQuantity(m_edMdfQuantity, COrderPlacingDlg::GetValidQuantity(quantity));
   }
   catch (std::exception& e)
   {
	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   *pResult = 0;
}

void CModifyDlg::OnEnKillfocusEdMdfQuantity()
{
   try
   {
      long validQuantity = COrderPlacingDlg::GetValidQuantity(COrderPlacingDlg::GetQuantity(m_edMdfQuantity));
      COrderPlacingDlg::SetQuantity(m_edMdfQuantity, validQuantity);
   }
   catch (std::exception& e)
   {
	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }
}

void CModifyDlg::OnDeltaposSpMdfLimitPrice(NMHDR *pNMHDR, LRESULT *pResult)
{
   try
   {
      ATL::CComPtr<ICQGInstrument> spInstrument;
      HRESULT hr = m_spOrder->get_Instrument(&spInstrument);
      AssertCOMError(m_spOrder, hr);

      LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
      COrderPlacingDlg::ChangePrice(m_edMdfLimitPrice, spInstrument, pNMUpDown->iDelta);
   }
   catch (std::exception& e)
   {
	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   *pResult = 0;
}

void CModifyDlg::OnDeltaposSpMdfStopPrice(NMHDR *pNMHDR, LRESULT *pResult)
{
   try
   {
      ATL::CComPtr<ICQGInstrument> spInstrument;
      HRESULT hr = m_spOrder->get_Instrument(&spInstrument);
      AssertCOMError(m_spOrder, hr);

      LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
      COrderPlacingDlg::ChangePrice(m_edMdfStopPrice, spInstrument, pNMUpDown->iDelta);
   }
   catch (std::exception& e)
   {
	   AfxMessageBox(ATL::CA2T(e.what()), MB_ICONSTOP);
   }

   *pResult = 0;
}
