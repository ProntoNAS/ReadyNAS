/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: ut_param_util.cpp,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
 * 
 * Portions Copyright (c) 1995-2004 RealNetworks, Inc. All Rights Reserved.
 * 
 * The contents of this file, and the files included with this file,
 * are subject to the current version of the RealNetworks Public
 * Source License (the "RPSL") available at
 * http://www.helixcommunity.org/content/rpsl unless you have licensed
 * the file under the current version of the RealNetworks Community
 * Source License (the "RCSL") available at
 * http://www.helixcommunity.org/content/rcsl, in which case the RCSL
 * will apply. You may also obtain the license terms directly from
 * RealNetworks.  You may not use this file except in compliance with
 * the RPSL or, if you have a valid RCSL with RealNetworks applicable
 * to this file, the RCSL.  Please see the applicable RPSL or RCSL for
 * the rights, obligations and limitations governing use of the
 * contents of the file.
 * 
 * Alternatively, the contents of this file may be used under the
 * terms of the GNU General Public License Version 2 or later (the
 * "GPL") in which case the provisions of the GPL are applicable
 * instead of those above. If you wish to allow use of your version of
 * this file only under the terms of the GPL, and not to allow others
 * to use your version of this file under the terms of either the RPSL
 * or RCSL, indicate your decision by deleting the provisions above
 * and replace them with the notice and other provisions required by
 * the GPL. If you do not delete the provisions above, a recipient may
 * use your version of this file under the terms of any one of the
 * RPSL, the RCSL or the GPL.
 * 
 * This file is part of the Helix DNA Technology. RealNetworks is the
 * developer of the Original Code and owns the copyrights in the
 * portions it created.
 * 
 * This file, and the files included with this file, is distributed
 * and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS
 * ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET
 * ENJOYMENT OR NON-INFRINGEMENT.
 * 
 * Technology Compatibility Kit Test Suite(s) Location:
 *    http://www.helixcommunity.org/content/tck
 * 
 * Contributor(s):
 * 
 * ***** END LICENSE BLOCK ***** */

#include "ut_param_util.h"

#include <string.h>
#include <stdlib.h>
#include "ut_string.h"


// Some implementations of strtoul() are broken on platforms like Symbian.
// A replacement for strtoul() is also defined in string.h and string.cpp in 
// runtim/pub/hlxclib. It is also here because we are trying to test certain 
// units without dependancies on others or the same one we are testing.
static unsigned long ut_strtoul(const char*s, char**end, int base)
{
    /* Some platforms like Symbian don't like leading +/- signs. So, 
     * strip and apply later.
     */
    int t=1;
    if(s && (*s=='-' || *s=='+'))
    {
        if(*s=='-')
            t=-1;
        ++s;
    }
    return t * strtoul(s, end, base);
}

bool UTParamUtil::GetInt(const char* pStrRep, int& value)
{
    char* pEnd = 0;

    value = (int)strtol(pStrRep, &pEnd, 0);

    return (*pStrRep && *pEnd == '\0');
}

bool UTParamUtil::GetUInt(const char* pStrRep, unsigned int& value)
{
    char* pEnd = 0;

    value = (unsigned int)ut_strtoul(pStrRep, &pEnd, 0);

    return (*pStrRep && *pEnd == '\0');
}

bool UTParamUtil::GetLong(const char* pStrRep, long& value)
{
    char* pEnd = 0;

    value = strtol(pStrRep, &pEnd, 0);

    return (*pStrRep && *pEnd == '\0');
}

bool UTParamUtil::GetULong(const char* pStrRep, unsigned long& value)
{
    char* pEnd = 0;

    value = ut_strtoul(pStrRep, &pEnd, 0);

    return (*pStrRep && *pEnd == '\0');
}

bool UTParamUtil::GetBool(const char* pStrRep, bool& value)
{
    bool ret = false;

    int val = 0;

    if (GetInt(pStrRep, val) &&
	((val == 0) || (val == 1)))
    {
	value = (val == 1);
	ret = true;
    }

    return ret;
}

bool UTParamUtil::GetChar(const char* pStrRep, char& value)
{
    bool ret = false;

    size_t len = strlen(pStrRep);
    if (len == 0)
    {
        value = '\0';
        ret = true;
    }
    else if (len == 1)
    {
	value = *pStrRep;
	ret = true;
    }
    else if ((len == 2) && (pStrRep[0] == '\\'))
    {
	switch (pStrRep[1]) {
	case '0' :
	    value = '\0';
	    break;
	case 'n':
	    value = '\n';
	    break;
	case 'r':
	    value = '\r';
	    break;
	case 't':
	    value = '\t';
	    break;
	default:
	    value = pStrRep[1];
	}

	ret = true;
    }

    return ret;
}

bool UTParamUtil::GetDouble(const char* pStrRep, double& value)
{
    char* pEnd = 0;

    value = strtod(pStrRep, &pEnd);

    return (*pStrRep && *pEnd == '\0');
}

