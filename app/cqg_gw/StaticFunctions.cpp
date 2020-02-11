#include "stdafx.h"
#include <stdexcept>
#include <cassert>
#include <sstream>
#include "Common.h"
#include "OrderPlacing.h"
#include "OrderPlacingDlg.h"

void COrderPlacingDlg::SetPropertyValue(
                                 ATL::CComPtr<ICQGOrderProperties>& spProperties,
                                 eOrderProperty propertyType,
                                 ATL::CComVariant propertyValue)
{
   // Get property
   ATL::CComPtr<ICQGOrderProperty> spProperty;
   HRESULT hr = spProperties->get_Item(propertyType, &spProperty);
   AssertCOMError(spProperties, hr);

   // Set property's value
   hr = spProperty->put_Value(propertyValue);
   AssertCOMError(spProperty, hr);
}

void COrderPlacingDlg::ChangePrice(
                                 CEdit& editCtrl,
                                 ATL::CComPtr<ICQGInstrument> spInstrument,
                                 int delta)
{
   // Get price from edit control
   double price = GetPrice(editCtrl, spInstrument);

   if (price == m_INVALID_DOUBLE)
   {
      // Price is invalid
      editCtrl.SetWindowText(m_N_A);
      return;
   }

   // Get tick size from instrument
   double tickSize;
   HRESULT hr = spInstrument->get_TickSize(&tickSize);
   AssertCOMError(spInstrument, hr);

   // Add tick size to price, minus is coming from spin button delta
   price -= tickSize * delta;

   // Get new price display format
   ATL::CComBSTR displayPriceBSTR;
   hr = spInstrument->ToDisplayPrice(price, &displayPriceBSTR);
   AssertCOMError(spInstrument, hr);

   // Show new price in edit control
   CString displayPrice = ATL::CW2T(displayPriceBSTR);

   editCtrl.SetWindowText(displayPrice);
}

double COrderPlacingDlg::GetPrice(
                                 CEdit& editCtrl,
                                 ATL::CComPtr<ICQGInstrument> spInstrument)
{
   // Get price display format from edit control
   CString priceStr;
   editCtrl.GetWindowText(priceStr);

   if (priceStr == m_N_A || priceStr.GetLength() == 0)
   {
      // Price is invalid
      return m_INVALID_DOUBLE;
   }

   // Get price from display price
   double price;
   HRESULT hr = spInstrument->FromDisplayPrice(ATL::CT2W(priceStr), &price);
   AssertCOMError(spInstrument, hr);
   
   return price;
}

long COrderPlacingDlg::GetQuantity(
                                 CEdit& editCtrl)
{
   CString quantityStr;
   editCtrl.GetWindowText(quantityStr);

   long quantity;
   std::stringstream strm;
   strm << quantityStr.GetString();
   strm >> quantity;

   return quantity;
}

long COrderPlacingDlg::GetValidQuantity(
                                 long quantity)
{
   long validQuantity;

   // Validate quantity for maximum and minimum values
   if (quantity < m_MIN_QUANTITY)
   {
      validQuantity = m_MIN_QUANTITY;
   }
   else if (quantity > m_MAX_QUANTITY)
   {
      validQuantity = m_MAX_QUANTITY;
   }
   else
   {
      validQuantity = quantity;
   }

   return validQuantity;
}

void COrderPlacingDlg::SetQuantity(
                                 CEdit& editCtrl,
                                 long quantity)
{
   CString quantityStr;
   quantityStr.Format(_T("%d"), quantity);

   editCtrl.SetWindowText(quantityStr);
}
