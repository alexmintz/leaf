#pragma once

/// \brief Returns latest COM error description
/// \param spClass [in,out] - Pointer to COM object which raised error
/// \param descr [out] - Error description
/// \return Result code
template <class IFaceT>
STDMETHODIMP GetErrorString(const ATL::CComPtr<IFaceT>& spClass, ATL::CComBSTR& descr)
{
   HRESULT hr = S_OK;

   ATL::CComPtr<ISupportErrorInfo> spSupportErrorInfo;
   hr = spClass->QueryInterface(__uuidof(ISupportErrorInfo), (void** )&spSupportErrorInfo);

   if (SUCCEEDED(hr))
   {
      hr = spSupportErrorInfo->InterfaceSupportsErrorInfo(__uuidof(IFaceT));
      if (hr == S_OK)
      {
         ATL::CComPtr<IErrorInfo> spErrorInfo;
         hr = GetErrorInfo(0, &spErrorInfo);

         if (hr == S_OK)
         {
            spErrorInfo->GetDescription(&descr);
         }
      }
   }
   return hr;
}

/// \brief Throws runtime exception depending on passed result code
/// \param spIFace - Pointer to object which is the source result code
/// \param hr - Result code 
template <class IFaceT>
void AssertCOMError(const ATL::CComPtr<IFaceT>& spIFace, 
                    HRESULT hr)
{
   if (FAILED(hr)) 
   {
      HRESULT hResult;
      ATL::CComBSTR errMessage;

      ATL::CComBSTR errDescCOM;
      hResult = GetErrorString(spIFace, errDescCOM);
      if (hResult == S_OK)
      {
         errMessage.Append(L"COM error occurred. Description: ");
         errMessage.Append(errDescCOM);
      }
      else
      {
         char* errDesc = NULL;
         FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&errDesc,
            0,
            NULL);
         errMessage.Append(errDesc);
         LocalFree(errDesc);
      }
      throw std::runtime_error(std::string(ATL::CW2A(errMessage.m_str)));
   }
}

/// \brief Converts raw date format to string
/// \param tsTime - Raw date
/// \param withTime - Add time to string
/// \param withDate - Add date to string
/// \return withDate - Formated string of date and time
CString TsTimeToString(DATE tsTime, 
                       bool withTime = true, 
                       bool withDate = true);

/// \brief Converts variant value to string
/// \param value - Variant value
/// \return withDate - String representation of the value
CString VariantToString(const ATL::CComVariant& value);

