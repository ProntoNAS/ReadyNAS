#include "syshdrs.h"
#ifdef PRAGMA_HDRSTOP
#	pragma no_pch
#	pragma hdrstop
#endif

/******************************************************************************/

const char gVersion[] = "@(#) NcFTP 3.2.4/465 Apr 07 2010, 06:24 PM";

/******************************************************************************/

#ifdef O_S
const char gOS[] = O_S;
#elif (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
const char gOS[] = "Windows";
#elif defined(__CYGWIN__)
const char gOS[] = "Cygwin";
#else
const char gOS[] = "UNIX";
#endif

/******************************************************************************/

const char gCopyright[] = "@(#) \
Copyright (c) 1992-2009 by Mike Gleason.\n\
All rights reserved.\n\
";

/******************************************************************************/
