// Common.cpp : implementation file
//

#include "stdafx.h"
#include <stdexcept>
#include "Common.h"

CString DateTimeToString(
                  DATE dateTime, 
                  bool withTime/* =true*/,
                  bool withDate/* =true*/)
{
   SYSTEMTIME stTime;
   VariantTimeToSystemTime(dateTime, &stTime);

   CString dateTimeString;
   if (withDate)
   {
      dateTimeString.Format(_T("%.2d/%.2d/%.2d"),
                        stTime.wMonth,
                        stTime.wDay,
                        stTime.wYear);
   }

   if (withTime)
   {
      CString timeString;
      timeString.Format(_T("%.2d:%.2d:%.2d"),
                        stTime.wHour,
                        stTime.wMinute,
                        stTime.wSecond);
      dateTimeString += withDate ? _T(" ") : _T("");
      dateTimeString += timeString;
   }

   return dateTimeString;
}

CString VariantToString(
                  const ATL::CComVariant& value)
{
   CString strValue;
   switch (value.vt)
   {
      case VT_I4:
         strValue.Format(_T("%d"), value.intVal);
         break;
      case VT_UI4:
         strValue.Format(_T("%d"), value.uintVal);
         break;
      case VT_R8:
         strValue.Format(_T("%.2f"), value.dblVal);
         break;
      case VT_DATE:
         strValue = DateTimeToString(value.date);
         break;
      case VT_BSTR:
         strValue = value.bstrVal;
         break;
      case VT_BOOL:
         strValue = (value.boolVal == VARIANT_TRUE) ? _T("True") : _T("False");
         break;
      default:
         throw std::invalid_argument("Unknown variant type");
   }

   return strValue;
}
