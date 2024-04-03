/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM nsIScriptableTimer.idl
 */

#ifndef __gen_nsIScriptableTimer_h__
#define __gen_nsIScriptableTimer_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

#ifndef __gen_nsIObserver_h__
#include "nsIObserver.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    nsIScriptableTimer */
#define NS_ISCRIPTABLETIMER_IID_STR "84271f22-c023-4b01-8050-d71c0c6a6235"

#define NS_ISCRIPTABLETIMER_IID \
  {0x84271f22, 0xc023, 0x4b01, \
    { 0x80, 0x50, 0xd7, 0x1c, 0x0c, 0x6a, 0x62, 0x35 }}

class NS_NO_VTABLE nsIScriptableTimer : public nsISupports {
 public: 

  NS_DEFINE_STATIC_IID_ACCESSOR(NS_ISCRIPTABLETIMER_IID)

  enum { PRIORITY_HIGHEST = 10 };

  enum { PRIORITY_HIGH = 8 };

  enum { PRIORITY_NORMAL = 5 };

  enum { PRIORITY_LOW = 2 };

  enum { PRIORITY_LOWEST = 0 };

  enum { TYPE_ONE_SHOT = 0 };

  enum { TYPE_REPEATING_SLACK = 1 };

  enum { TYPE_REPEATING_PRECISE = 2 };

  /**
   * Initialize a timer that will fire after the said delay.
   * A user must keep a reference to this timer till it is 
   * is no longer needed or has been cancelled.
   *
   * @param aObserver   the callback object that observes the 
   *                    ``timer-callback'' topic with the subject being
   *                    the timer itself when the timer fires:
   *
   *                      observe(nsISupports aSubject, => nsIScriptableTimer
   *                              string aTopic,        => ``timer-callback''
   *                              wstring data          =>  null
   *
   * @param aDelay      delay in milliseconds for timer to fire
   * @param aPriority   timer priority per PRIORITY* consts defined above
   * @param aType       timer type per TYPE* consts defined above
   */
  /* void init (in nsIObserver aObserver, in unsigned long aDelay, in unsigned long aPriority, in unsigned long aType); */
  NS_IMETHOD Init(nsIObserver *aObserver, PRUint32 aDelay, PRUint32 aPriority, PRUint32 aType) = 0;

  /**
   * Cancellation of timers applies to repeating timers 
   * (i.e., init()ed with aType=TYPE_REPEATING*).
   */
  /* void cancel (); */
  NS_IMETHOD Cancel(void) = 0;

};

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISCRIPTABLETIMER \
  NS_IMETHOD Init(nsIObserver *aObserver, PRUint32 aDelay, PRUint32 aPriority, PRUint32 aType); \
  NS_IMETHOD Cancel(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISCRIPTABLETIMER(_to) \
  NS_IMETHOD Init(nsIObserver *aObserver, PRUint32 aDelay, PRUint32 aPriority, PRUint32 aType) { return _to Init(aObserver, aDelay, aPriority, aType); } \
  NS_IMETHOD Cancel(void) { return _to Cancel(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISCRIPTABLETIMER(_to) \
  NS_IMETHOD Init(nsIObserver *aObserver, PRUint32 aDelay, PRUint32 aPriority, PRUint32 aType) { return !_to ? NS_ERROR_NULL_POINTER : _to->Init(aObserver, aDelay, aPriority, aType); } \
  NS_IMETHOD Cancel(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Cancel(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsScriptableTimer : public nsIScriptableTimer
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISCRIPTABLETIMER

  nsScriptableTimer();
  virtual ~nsScriptableTimer();
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsScriptableTimer, nsIScriptableTimer)

nsScriptableTimer::nsScriptableTimer()
{
  NS_INIT_ISUPPORTS();
  /* member initializers and constructor code */
}

nsScriptableTimer::~nsScriptableTimer()
{
  /* destructor code */
}

/* void init (in nsIObserver aObserver, in unsigned long aDelay, in unsigned long aPriority, in unsigned long aType); */
NS_IMETHODIMP nsScriptableTimer::Init(nsIObserver *aObserver, PRUint32 aDelay, PRUint32 aPriority, PRUint32 aType)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void cancel (); */
NS_IMETHODIMP nsScriptableTimer::Cancel()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif

#ifndef NS_TIMER_CONTRACTID
#define NS_TIMER_CONTRACTID "@mozilla.org/timer;1"
#endif
#define NS_TIMER_CALLBACK_TOPIC "timer-callback"

#endif /* __gen_nsIScriptableTimer_h__ */
