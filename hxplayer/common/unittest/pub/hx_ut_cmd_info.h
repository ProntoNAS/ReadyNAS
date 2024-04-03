/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hx_ut_cmd_info.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#ifndef HLX_UT_CMD_INFO_H
#define HLX_UT_CMD_INFO_H

#include "ut_vector.h"
#include "ut_string.h"

class HLXUnitTestCmdInfo
{
public:
    virtual ~HLXUnitTestCmdInfo();
    
    virtual int ArgCount() const = 0;
    virtual int ArgCountMax() const = 0;
    virtual const UTString& CommandName() const = 0;
    virtual bool Execute(const UTVector<UTString>& info) = 0;
    virtual HLXUnitTestCmdInfo* Clone() const = 0;
};

template <class T>
class HLXUnitTestCmdInfoDisp : public HLXUnitTestCmdInfo
{
public:
    typedef bool (T::*TestFunc)(const UTVector<UTString>& info);

    HLXUnitTestCmdInfoDisp(T* pObj, const UTString& cmdName,
			   TestFunc pTestFunc, int argCount,
                           int argCountMax = -1);

    virtual ~HLXUnitTestCmdInfoDisp();

    virtual int ArgCount() const;
    virtual int ArgCountMax() const;
    virtual const UTString& CommandName() const;
    virtual bool Execute(const UTVector<UTString>& info);
    virtual HLXUnitTestCmdInfo* Clone() const;

private:
    T* m_pObj;
    UTString m_cmdName;
    TestFunc m_pTestFunc;
    int m_argCount;
    int m_argCountMax;
};

template <class T>
inline
HLXUnitTestCmdInfoDisp<T>::HLXUnitTestCmdInfoDisp(T* pObj, 
						  const UTString& cmdName,
						  TestFunc pTestFunc, 
						  int argCount,
                                                  int argCountMax) :
    m_pObj(pObj),
    m_cmdName(cmdName),
    m_pTestFunc(pTestFunc),
    m_argCount(argCount),
    m_argCountMax(argCountMax > argCount ? argCountMax : argCount)
{}

template <class T>
inline
HLXUnitTestCmdInfoDisp<T>::~HLXUnitTestCmdInfoDisp()
{}

template <class T>
inline
int HLXUnitTestCmdInfoDisp<T>::ArgCount() const
{
    return m_argCount;
}

template <class T>
inline
int HLXUnitTestCmdInfoDisp<T>::ArgCountMax() const
{
    return m_argCountMax;
}

template <class T>
inline
const UTString& HLXUnitTestCmdInfoDisp<T>::CommandName() const
{
    return m_cmdName;
}

template <class T>
inline
bool HLXUnitTestCmdInfoDisp<T>::Execute(const UTVector<UTString>& info)
{
    return (m_pObj->*m_pTestFunc)(info);
}

template <class T>
inline
HLXUnitTestCmdInfo* HLXUnitTestCmdInfoDisp<T>::Clone() const
{
    return new HLXUnitTestCmdInfoDisp<T>(m_pObj, m_cmdName, m_pTestFunc, 
					 m_argCount, m_argCountMax);
}

#endif // HLX_UT_CMD_INFO_H
