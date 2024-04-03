/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM d:/mozilla/embedding/components/windowwatcher/public/nsIPromptService.idl
 */

#ifndef __gen_nsIPromptService_h__
#define __gen_nsIPromptService_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif
class nsIDOMWindow; /* forward declaration */


/* starting interface:    nsIPromptService */
#define NS_IPROMPTSERVICE_IID_STR "1630c61a-325e-49ca-8759-a31b16c47aa5"

#define NS_IPROMPTSERVICE_IID \
  {0x1630c61a, 0x325e, 0x49ca, \
    { 0x87, 0x59, 0xa3, 0x1b, 0x16, 0xc4, 0x7a, 0xa5 }}

class NS_NO_VTABLE nsIPromptService : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_IPROMPTSERVICE_IID)

  /**
   * Puts up an alert dialog with an OK button.
   */
  /* void alert (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text); */
  NS_IMETHOD Alert(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text) = 0;

  /**
   * Puts up an alert dialog with an OK button and
   * a message with a checkbox.
   */
  /* void alertCheck (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, in wstring checkMsg, inout boolean checkValue); */
  NS_IMETHOD AlertCheck(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, const PRUnichar *checkMsg, PRBool *checkValue) = 0;

  /**
   * Puts up a dialog with OK and Cancel buttons.
   * @return true for OK, false for Cancel
   */
  /* boolean confirm (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text); */
  NS_IMETHOD Confirm(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRBool *_retval) = 0;

  /**
   * Puts up a dialog with OK and Cancel buttons, and
   * a message with a single checkbox.
   * @return true for OK, false for Cancel
   */
  /* boolean confirmCheck (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, in wstring checkMsg, inout boolean checkValue); */
  NS_IMETHOD ConfirmCheck(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval) = 0;

  /**
   * Puts up a dialog with up to 3 buttons and an optional checkbox.
   *
   * @param dialogTitle
   * @param text
   * @param buttonFlags       Title flags for each button.
   * @param button0Title      Used when button 0 uses TITLE_IS_STRING
   * @param button1Title      Used when button 1 uses TITLE_IS_STRING
   * @param button2Title      Used when button 2 uses TITLE_IS_STRING
   * @param checkMsg          null if no checkbox
   * @param checkValue    
   * @return buttonPressed
   *
   * Buttons are numbered 0 - 2. The implementation can decide whether
   * the sequence goes from right to left or left to right.
   * Button 0 will be the default button.
   *
   * A button may use a predefined title, specified by one of the
   * constants below. Each title constant can be multiplied by a
   * position constant to assign the title to a particular button.
   * If BUTTON_TITLE_IS_STRING is used for a button, the string
   * parameter for that button will be used. If the value for a button
   * position is zero, the button will not be shown
   *
   */
  enum { BUTTON_POS_0 = 1U };

  enum { BUTTON_POS_1 = 256U };

  enum { BUTTON_POS_2 = 65536U };

  enum { BUTTON_TITLE_OK = 1U };

  enum { BUTTON_TITLE_CANCEL = 2U };

  enum { BUTTON_TITLE_YES = 3U };

  enum { BUTTON_TITLE_NO = 4U };

  enum { BUTTON_TITLE_SAVE = 5U };

  enum { BUTTON_TITLE_DONT_SAVE = 6U };

  enum { BUTTON_TITLE_REVERT = 7U };

  enum { BUTTON_TITLE_IS_STRING = 127U };

  enum { STD_OK_CANCEL_BUTTONS = 513U };

  /* PRInt32 confirmEx (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, in unsigned long buttonFlags, in wstring button0Title, in wstring button1Title, in wstring button2Title, in wstring checkMsg, inout boolean checkValue); */
  NS_IMETHOD ConfirmEx(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUint32 buttonFlags, const PRUnichar *button0Title, const PRUnichar *button1Title, const PRUnichar *button2Title, const PRUnichar *checkMsg, PRBool *checkValue, PRInt32 *_retval) = 0;

  /**
   * Puts up a dialog with an edit field and an optional checkbox.
   *
   * @param dialogTitle
   * @param text
   * @param value         in: Pre-fills the dialog field if non-null
   *                      out: If result is true, a newly allocated
   *                      string. If result is false, in string is not
   *                      touched.
   * @param checkMsg      if null, check box will not be shown
   * @param checkValue       
   * @return true for OK, false for Cancel
   */
  /* boolean prompt (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, inout wstring value, in wstring checkMsg, inout boolean checkValue); */
  NS_IMETHOD Prompt(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **value, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval) = 0;

  /**
   * Puts up a dialog with an edit field, a password field, and an optional checkbox.
   *
   * @param dialogTitle
   * @param text
   * @param username      in: Pre-fills the dialog field if non-null
   *                      out: If result is true, a newly allocated
   *                      string. If result is false, in string is not
   *                      touched.
   * @param password      in: Pre-fills the dialog field if non-null
   *                      out: If result is true, a newly allocated
   *                      string. If result is false, in string is not
   *                      touched.
   * @param checkMsg      if null, check box will not be shown
   * @param checkValue   
   * @return true for OK, false for Cancel
   */
  /* boolean promptUsernameAndPassword (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, inout wstring username, inout wstring password, in wstring checkMsg, inout boolean checkValue); */
  NS_IMETHOD PromptUsernameAndPassword(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **username, PRUnichar **password, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval) = 0;

  /**
   * Puts up a dialog with a password field and an optional checkbox.
   *
   * @param dialogTitle
   * @param text
   * @param password      in: Pre-fills the dialog field if non-null
   *                      out: If result is true, a newly allocated
   *                      string. If result is false, in string is not
   *                      touched.
   * @param checkMsg      if null, check box will not be shown
   * @param checkValue    
   * @return true for OK, false for Cancel
   */
  /* boolean promptPassword (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, inout wstring password, in wstring checkMsg, inout boolean checkValue); */
  NS_IMETHOD PromptPassword(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **password, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval) = 0;

  /**
   * Puts up a dialog box which has a list box of strings
   */
  /* boolean select (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, in PRUint32 count, [array, size_is (count)] in wstring selectList, out long outSelection); */
  NS_IMETHOD Select(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUint32 count, const PRUnichar **selectList, PRInt32 *outSelection, PRBool *_retval) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSIPROMPTSERVICE \
  NS_IMETHOD Alert(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text); \
  NS_IMETHOD AlertCheck(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, const PRUnichar *checkMsg, PRBool *checkValue); \
  NS_IMETHOD Confirm(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRBool *_retval); \
  NS_IMETHOD ConfirmCheck(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval); \
  NS_IMETHOD ConfirmEx(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUint32 buttonFlags, const PRUnichar *button0Title, const PRUnichar *button1Title, const PRUnichar *button2Title, const PRUnichar *checkMsg, PRBool *checkValue, PRInt32 *_retval); \
  NS_IMETHOD Prompt(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **value, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval); \
  NS_IMETHOD PromptUsernameAndPassword(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **username, PRUnichar **password, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval); \
  NS_IMETHOD PromptPassword(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **password, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval); \
  NS_IMETHOD Select(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUint32 count, const PRUnichar **selectList, PRInt32 *outSelection, PRBool *_retval); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSIPROMPTSERVICE(_to) \
  NS_IMETHOD Alert(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text) { return _to Alert(parent, dialogTitle, text); } \
  NS_IMETHOD AlertCheck(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, const PRUnichar *checkMsg, PRBool *checkValue) { return _to AlertCheck(parent, dialogTitle, text, checkMsg, checkValue); } \
  NS_IMETHOD Confirm(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRBool *_retval) { return _to Confirm(parent, dialogTitle, text, _retval); } \
  NS_IMETHOD ConfirmCheck(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval) { return _to ConfirmCheck(parent, dialogTitle, text, checkMsg, checkValue, _retval); } \
  NS_IMETHOD ConfirmEx(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUint32 buttonFlags, const PRUnichar *button0Title, const PRUnichar *button1Title, const PRUnichar *button2Title, const PRUnichar *checkMsg, PRBool *checkValue, PRInt32 *_retval) { return _to ConfirmEx(parent, dialogTitle, text, buttonFlags, button0Title, button1Title, button2Title, checkMsg, checkValue, _retval); } \
  NS_IMETHOD Prompt(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **value, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval) { return _to Prompt(parent, dialogTitle, text, value, checkMsg, checkValue, _retval); } \
  NS_IMETHOD PromptUsernameAndPassword(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **username, PRUnichar **password, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval) { return _to PromptUsernameAndPassword(parent, dialogTitle, text, username, password, checkMsg, checkValue, _retval); } \
  NS_IMETHOD PromptPassword(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **password, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval) { return _to PromptPassword(parent, dialogTitle, text, password, checkMsg, checkValue, _retval); } \
  NS_IMETHOD Select(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUint32 count, const PRUnichar **selectList, PRInt32 *outSelection, PRBool *_retval) { return _to Select(parent, dialogTitle, text, count, selectList, outSelection, _retval); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSIPROMPTSERVICE(_to) \
  NS_IMETHOD Alert(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text) { return !_to ? NS_ERROR_NULL_POINTER : _to->Alert(parent, dialogTitle, text); } \
  NS_IMETHOD AlertCheck(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, const PRUnichar *checkMsg, PRBool *checkValue) { return !_to ? NS_ERROR_NULL_POINTER : _to->AlertCheck(parent, dialogTitle, text, checkMsg, checkValue); } \
  NS_IMETHOD Confirm(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Confirm(parent, dialogTitle, text, _retval); } \
  NS_IMETHOD ConfirmCheck(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ConfirmCheck(parent, dialogTitle, text, checkMsg, checkValue, _retval); } \
  NS_IMETHOD ConfirmEx(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUint32 buttonFlags, const PRUnichar *button0Title, const PRUnichar *button1Title, const PRUnichar *button2Title, const PRUnichar *checkMsg, PRBool *checkValue, PRInt32 *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->ConfirmEx(parent, dialogTitle, text, buttonFlags, button0Title, button1Title, button2Title, checkMsg, checkValue, _retval); } \
  NS_IMETHOD Prompt(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **value, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Prompt(parent, dialogTitle, text, value, checkMsg, checkValue, _retval); } \
  NS_IMETHOD PromptUsernameAndPassword(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **username, PRUnichar **password, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->PromptUsernameAndPassword(parent, dialogTitle, text, username, password, checkMsg, checkValue, _retval); } \
  NS_IMETHOD PromptPassword(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **password, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->PromptPassword(parent, dialogTitle, text, password, checkMsg, checkValue, _retval); } \
  NS_IMETHOD Select(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUint32 count, const PRUnichar **selectList, PRInt32 *outSelection, PRBool *_retval) { return !_to ? NS_ERROR_NULL_POINTER : _to->Select(parent, dialogTitle, text, count, selectList, outSelection, _retval); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsPromptService : public nsIPromptService
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIPROMPTSERVICE

  nsPromptService();
  virtual ~nsPromptService();
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsPromptService, nsIPromptService)

nsPromptService::nsPromptService()
{
  /* member initializers and constructor code */
}

nsPromptService::~nsPromptService()
{
  /* destructor code */
}

/* void alert (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text); */
NS_IMETHODIMP nsPromptService::Alert(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void alertCheck (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, in wstring checkMsg, inout boolean checkValue); */
NS_IMETHODIMP nsPromptService::AlertCheck(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, const PRUnichar *checkMsg, PRBool *checkValue)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean confirm (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text); */
NS_IMETHODIMP nsPromptService::Confirm(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean confirmCheck (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, in wstring checkMsg, inout boolean checkValue); */
NS_IMETHODIMP nsPromptService::ConfirmCheck(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* PRInt32 confirmEx (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, in unsigned long buttonFlags, in wstring button0Title, in wstring button1Title, in wstring button2Title, in wstring checkMsg, inout boolean checkValue); */
NS_IMETHODIMP nsPromptService::ConfirmEx(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUint32 buttonFlags, const PRUnichar *button0Title, const PRUnichar *button1Title, const PRUnichar *button2Title, const PRUnichar *checkMsg, PRBool *checkValue, PRInt32 *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean prompt (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, inout wstring value, in wstring checkMsg, inout boolean checkValue); */
NS_IMETHODIMP nsPromptService::Prompt(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **value, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean promptUsernameAndPassword (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, inout wstring username, inout wstring password, in wstring checkMsg, inout boolean checkValue); */
NS_IMETHODIMP nsPromptService::PromptUsernameAndPassword(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **username, PRUnichar **password, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean promptPassword (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, inout wstring password, in wstring checkMsg, inout boolean checkValue); */
NS_IMETHODIMP nsPromptService::PromptPassword(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUnichar **password, const PRUnichar *checkMsg, PRBool *checkValue, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean select (in nsIDOMWindow parent, in wstring dialogTitle, in wstring text, in PRUint32 count, [array, size_is (count)] in wstring selectList, out long outSelection); */
NS_IMETHODIMP nsPromptService::Select(nsIDOMWindow *parent, const PRUnichar *dialogTitle, const PRUnichar *text, PRUint32 count, const PRUnichar **selectList, PRInt32 *outSelection, PRBool *_retval)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

// {1630C61A-325E-49ca-8759-A31B16C47AA5}
#define NS_PROMPTSERVICE_IID \
 {0x1630C61A, 0x325E, 0x49ca, {0x87, 0x59, 0xA3, 0x1B, 0x16, 0xC4, 0x7A, 0xA5}}

#endif /* __gen_nsIPromptService_h__ */
