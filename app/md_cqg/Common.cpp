#include "stdafx.h"
#include "common.h"


CString TsTimeToString(DATE tsTime, 
                       bool withTime/* =true*/, 
                       bool withDate/* =true*/)
{
   using std::setw;

   SYSTEMTIME stTime;
   VariantTimeToSystemTime(tsTime, &stTime);

   CString timeString;
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
	   timeString.Format(_T("%.2d:%.2d:%.2d.%d"),
                           stTime.wHour,
                           stTime.wMinute,
						   stTime.wSecond,
						   stTime.wMilliseconds);
      dateTimeString += withDate ? _T(" ") : _T("");
      dateTimeString += timeString;
   }

   return dateTimeString;
}

CString VariantToString(const ATL::CComVariant& value)
{
   CString strValue;
   switch (value.vt)
   {
      case VT_I4:
		  strValue.Format(_T("%d"), value.intVal);
         break;
      case VT_I2:
		  strValue.Format(_T("%d"), value.iVal);
         break;
      case VT_UI4:
		  strValue.Format(_T("%d"), value.uintVal);
         break;
      case VT_R8:
		  strValue.Format(_T("%f"), value.dblVal);
         break;
      case VT_DATE:
         strValue = TsTimeToString(value.date);
         break;
      case VT_BSTR:
         strValue = ATL::CW2T(value.bstrVal);
         break;
      case VT_BOOL:
         strValue = (value.boolVal == VARIANT_TRUE) ? _T("True") : _T("False");
         break;
      default:
         throw std::runtime_error(std::string("Unknown type"));
   }

   return strValue;
}