/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM nsIScriptableUConv.idl
 */

#ifndef __gen_nsIScriptableUConv_h__
#define __gen_nsIScriptableUConv_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
// {0A698C44-3BFF-11d4-9649-00C0CA135B4E}
#define NS_ISCRIPTABLEUNICODECONVERTER_CID { 0x0A698C44, 0x3BFF, 0x11d4, { 0x96, 0x49, 0x00, 0xC0, 0xCA, 0x13, 0x5B, 0x4E } }
#define NS_ISCRIPTABLEUNICODECONVERTER_CONTRACTID "@mozilla.org/intl/scriptableunicodeconverter"

/* starting interface:    nsIScriptableUnicodeConverter */
#define NS_ISCRIPTABLEUNICODECONVERTER_IID_STR "d395ef67-a613-47b9-b5e3-dc7ce1bdb71a"

#define NS_ISCRIPTABLEUNICODECONVERTER_IID \
  {0xd395ef67, 0xa613, 0x47b9, \
    { 0xb5, 0xe3, 0xdc, 0x7c, 0xe1, 0xbd, 0xb7, 0x1a }}

/**
 * This interface is unicode encoder using from script
 *
 * @created         8/Jun/2000
 * @author          Makoto Kato [m_kato@ga2.so-net.ne.jp]
 */
class NS_NO_VTABLE nsIScriptableUnicodeConverter : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ISCRIPTABLEUNICODECONVERTER_IID)

  /**
   * Converts the data from Unicode to one Charset.
   */
  /* string ConvertFromUnicode (in wstring aSrc); */
  NS_IMETHOD ConvertFromUnicode(const PRUnichar *aSrc, char **_retval) = 0;

  /**
   * Returns the terminator string.
   * Should be called after ConvertFromUnicode().
   */
  /* string Finish (); */
  NS_IMETHOD Finish(char **_retval) = 0;

  /**
   * Converts the data from one Charset to Unicode.
   */
  /* wstring ConvertToUnicode ([const] in string aSrc); */
  NS_IMETHOD ConvertToUnicode(const char *aSrc, PRUnichar **_retval) = 0;

  /**
   * Current charactor set
   */
  /* attribute string charset; */
  NS_IMETHOD GetCharset(char * *aCharset) = 0;
  NS_IMETHOD SetCharset(const char * aCharset) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISCRIPTABLEUNICODECONVERTER \
  NS_IMETHOD ConvertFromUnicode(const PRUnichar *aSrc, char **_retval); \
  NS_IMETHOD Finish(char **_retval); \
  NS_IMETHOD ConvertToUnicode(const char *aSrc, PRUnichar **_retval); \
  NS_IMETHOD GetCharset(char * *aCharset); \
  NS_IMETHOD SetCharset(const char * aCharset); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISCRIPTABLEUNICODECONVERTER(_to) \
  NS_IMETHOD ConvertFromUnicode(const PRUnichar *aSrc, char **_retval) { return _to ConvertFromUnicode(aSrc, _retval); } \
  NS_IMETHOD Finish(char **_retval) { return _to Finish(_retval); } \
  NS_IMETHOD ConvertToUnicode(const char *aSrc, PRUnichar **_retval) { return _to ConvertToUnicode(aSrc, _retval); } \
  NS_IMETHOD GetCharset(char * *aCharset) { return _to GetCharset(aCharset); } \
  NS_IMETHOD SetCharset(const char * aCharset) { return _to SetCharset(aCharset); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISCRIPTABLEUNICODECONVERTER(_to) \
  NS_IMETHOD ConvertFromUnicode(const PRUnichar *aSrc, char **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ConvertFromUnicode(aSrc, _retval); } \
  NS_IMETHOD Finish(char **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Finish(_retval); } \
  NS_IMETHOD ConvertToUnicode(const char *aSrc, PRUnichar **_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ConvertToUnicode(aSrc, _retval); } \
  NS_IMETHOD GetCharset(char * *aCharset) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetCharset(aCharset); } \
  NS_IMETHOD SetCharset(const char * aCharset) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetCharset(aCharset); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsScriptableUnicodeConverter : public nsIScriptableUnicodeConverter
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISCRIPTABLEUNICODECONVERTER

  nsScriptableUnicodeConverter();
  virtual ~nsScriptableUnicodeConverter();
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsScriptableUnicodeConverter, nsIScriptableUnicodeConverter)

nsScriptableUnicodeConverter::nsScriptableUnicodeConverter()
{
  /* member initializers and constructor code */
}

nsScriptableUnicodeConverter::~nsScriptableUnicodeConverter()
{
  /* destructor code */
}

/* string ConvertFromUnicode (in wstring aSrc); */
NS_IMETHODIMP nsScriptableUnicodeConverter::ConvertFromUnicode(const PRUnichar *aSrc, char **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* string Finish (); */
NS_IMETHODIMP nsScriptableUnicodeConverter::Finish(char **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* wstring ConvertToUnicode ([const] in string aSrc); */
NS_IMETHODIMP nsScriptableUnicodeConverter::ConvertToUnicode(const char *aSrc, PRUnichar **_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute string charset; */
NS_IMETHODIMP nsScriptableUnicodeConverter::GetCharset(char * *aCharset)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsScriptableUnicodeConverter::SetCharset(const char * aCharset)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIScriptableUConv_h__ */
