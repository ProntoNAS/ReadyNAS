/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hxwinreg.h,v 1.1.1.1 2006/03/29 16:45:32 hagi Exp $
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

class CHXString;

#include "hlxclib/windows.h"

class _CListOfCHXString_Node
{
public:
    _CListOfCHXString_Node();
    ~_CListOfCHXString_Node();

    CHXString& value();
    const CHXString& value() const;
    void value(const CHXString& rclsNewValue);
    _CListOfCHXString_Node& operator=(const CHXString& rclsNewValue); 
    _CListOfCHXString_Node* next() const;
    void next(_CListOfCHXString_Node* plocnNew);
    _CListOfCHXString_Node* prev() const;
    void prev(_CListOfCHXString_Node* plocnNew);
    void Remove();
    void Insert(_CListOfCHXString_Node& rlocnNew);
						
protected:
    CHXString m_clsValue;
    _CListOfCHXString_Node* m_plocPrev;
    _CListOfCHXString_Node* m_plocNext;
    _CListOfCHXString_Node(const _CListOfCHXString_Node& rlocnOther){}
};

class _CListIteratorCHXString_;
class _CListReverseIteratorCHXString_;

class _CListOfCHXString_
{
public:
    typedef _CListIteratorCHXString_ iterator;
    typedef _CListReverseIteratorCHXString_ reverse_iterator;
    typedef const _CListReverseIteratorCHXString_ const_reverse_iterator;

    _CListOfCHXString_();
    _CListOfCHXString_(const _CListOfCHXString_& rlocOther);
    ~_CListOfCHXString_();
    _CListOfCHXString_& operator=(const _CListOfCHXString_& rlocOther); 

    iterator begin();
    const iterator begin() const;
    iterator end();
    const iterator end() const;

    reverse_iterator rbegin();
    const reverse_iterator rbegin() const;
    reverse_iterator rend();
    const reverse_iterator rend() const;

    iterator insert(iterator itBefore, const CHXString&);
    void insert
    (
	iterator itBefore,
	const iterator itFirst,
	const iterator itLast
    );
    void remove(iterator itThis);
    void remove(iterator itFirst, iterator itLast);

    void empty();

protected:
    _CListOfCHXString_Node m_locnREnd;
    _CListOfCHXString_Node m_locnEnd;

    void _copy(const _CListOfCHXString_& rlocOther);
};

class _CListIteratorCHXString_
{
public:
    _CListIteratorCHXString_();
    _CListIteratorCHXString_
    (
        const _CListOfCHXString_Node& rlocnNewLocation
    );
    _CListIteratorCHXString_(const _CListIteratorCHXString_& rliocOther);
    ~_CListIteratorCHXString_();

    _CListIteratorCHXString_& operator=
    (
	const _CListIteratorCHXString_& rliocOther
    );

    CHXString& operator*();
    _CListIteratorCHXString_& operator=(const CHXString& rclsNewValue);

    _CListIteratorCHXString_& operator++();
    const _CListIteratorCHXString_ operator++(int);

    _CListIteratorCHXString_& operator--();
    const _CListIteratorCHXString_ operator--(int);

private:
    _CListOfCHXString_Node* m_plocCurrent;

    friend class _CListOfCHXString_;
    friend BOOL operator==
    (
	const _CListIteratorCHXString_& rliocLeft,
	const _CListIteratorCHXString_& rliocRight
    );
    friend BOOL operator!=
    (
	const _CListIteratorCHXString_& rliocLeft,
	const _CListIteratorCHXString_& rliocRight
    );
};

BOOL operator==
(
    const _CListIteratorCHXString_& rliocLeft,
    const _CListIteratorCHXString_& rliocRight
);

BOOL operator!=
(
    const _CListIteratorCHXString_& rliocLeft,
    const _CListIteratorCHXString_& rliocRight
);

class _CListReverseIteratorCHXString_
{
public:
    _CListReverseIteratorCHXString_();
    _CListReverseIteratorCHXString_
    (
        const _CListOfCHXString_Node& rlocnNewLocation
    );
    _CListReverseIteratorCHXString_
    (
	_CListReverseIteratorCHXString_& rlriocOther
    );
    ~_CListReverseIteratorCHXString_();

    _CListReverseIteratorCHXString_& operator=
    (
	const _CListReverseIteratorCHXString_& rlriocOther
    );

    CHXString& operator*();
    _CListReverseIteratorCHXString_& operator=(const CHXString& rclsNewValue);

    _CListReverseIteratorCHXString_& operator++();
    const _CListReverseIteratorCHXString_ operator++(int);
    _CListReverseIteratorCHXString_& operator--();
    const _CListReverseIteratorCHXString_ operator--(int);

private:
    _CListOfCHXString_Node* m_plocCurrent;
    friend class _CListOfCHXString_;
    friend BOOL operator==
    (
	const _CListReverseIteratorCHXString_& rlriocLeft,
	const _CListReverseIteratorCHXString_& rlriocRight
    );
    friend BOOL operator!=
    (
	const _CListReverseIteratorCHXString_& rlriocLeft,
	const _CListReverseIteratorCHXString_& rlriocRight
    );
};

BOOL operator==
(
    const _CListReverseIteratorCHXString_& rlriocLeft,
    const _CListReverseIteratorCHXString_& rlriocRight
);
BOOL operator!=
(
    const _CListReverseIteratorCHXString_& rlriocLeft,
    const _CListReverseIteratorCHXString_& rlriocRight
);								    

class AWinRegValue
{
public:
    AWinRegValue(const char* szName, HKEY hkParent)
	: m_hkParent(hkParent)
	, m_sName(szName)
    {
    }
    virtual ~AWinRegValue(){}

    HX_RESULT Delete()
    {
	if
	(
	    RegDeleteValue
	    (
		m_hkParent,
		OS_STRING(m_sName)
	    ) == ERROR_SUCCESS
	)
	{
	    return HXR_OK;
	}

        return HXR_FAIL;
    }

    void SetName(const char* szName){m_sName = szName;}
    CHXString& GetName(){return m_sName;}

    void SetParent(HKEY hkParent){m_hkParent = hkParent;}
    
    virtual BOOL DoesExist() =0;

    virtual BOOL IsDWORD(){return FALSE;}
    virtual BOOL AsDWORD(UINT32*){return FALSE;}
    virtual BOOL FromDWORD(UINT32){return FALSE;}

    virtual BOOL IsString(){return FALSE;}
    virtual BOOL AsString(CHXString&){return FALSE;}
    virtual BOOL FromString(const char*){return FALSE;}

    virtual BOOL IsStringArray(){return FALSE;}
    virtual BOOL AsStringArray(_CListOfCHXString_&){return FALSE;}
    virtual BOOL AsStringArray(char**){return FALSE;}
    virtual BOOL FromStringArray (const _CListOfCHXString_&){return FALSE;}
    virtual BOOL FromStringArray(const char*, UINT32){return FALSE;}

    void FreeStringArray
    (
	char*& rszValue
    ){delete [] rszValue;rszValue=NULL;}

protected:
    HKEY m_hkParent;
    CHXString m_sName;
    AWinRegValue(){}
private:
};

class CWinRegStringValue : public AWinRegValue
{
public:
    CWinRegStringValue(const char* szName, HKEY hkParent);

    virtual ~CWinRegStringValue();

    CWinRegStringValue& operator=(const char* szNewValue);

    BOOL IsString();
    BOOL AsString(CHXString&);
    BOOL FromString(const char*);

    HX_RESULT Delete();
    
    BOOL DoesExist();

private:
    CWinRegStringValue(){}
    CWinRegStringValue(const CWinRegStringValue&){}
};

class CWinRegDWORDValue : public AWinRegValue
{
public:
    CWinRegDWORDValue(const char* szName, HKEY hkParent);

    virtual ~CWinRegDWORDValue();

    CWinRegDWORDValue& operator=(UINT32 dwNewValue);

    BOOL IsDWORD();
    BOOL AsDWORD(UINT32*);
    BOOL FromDWORD(UINT32);

    HX_RESULT Delete();
    
    BOOL DoesExist();

private:
    CWinRegDWORDValue(){}
    CWinRegDWORDValue(const CWinRegDWORDValue&){}
};

class CWinRegStringArrayValue : public AWinRegValue
{
public:
    CWinRegStringArrayValue(const char* szName, HKEY hkParent);

    virtual ~CWinRegStringArrayValue();

    CWinRegStringArrayValue& operator=(const _CListOfCHXString_&);

    BOOL IsStringArray();
    BOOL AsStringArray(_CListOfCHXString_&);
    BOOL AsStringArray(char**);
    BOOL FromStringArray(const _CListOfCHXString_&);
    BOOL FromStringArray(const char*, UINT32);

    HX_RESULT Delete();
    
    BOOL DoesExist();

private:
    CWinRegStringArrayValue(){}
    CWinRegStringArrayValue(const CWinRegStringArrayValue&){}
};

class CWinRegKey
{
public:
    CWinRegKey();
    virtual ~CWinRegKey();

    HX_RESULT Open();
    HX_RESULT Create
    (
	const char* szClass=NULL, 
	DWORD dwOptions=REG_OPTION_NON_VOLATILE
    );
    HX_RESULT Close();
    HX_RESULT Flush();
    HX_RESULT DeleteSubKey(const char* szName);
    
    BOOL DoesExist();

    BOOL SetDesiredAccess(REGSAM rsNew);
    REGSAM GetDesiredAccess();

    BOOL SetRootKey(HKEY hkRoot);
    HKEY GetRootKey();

    HKEY GetHandle();

    BOOL SetRelativePath(const char* szPath);
    CHXString& GetRelativePath();

    BOOL GetValue(const char* szName, AWinRegValue** ppwrvOut, UINT32 ulType=0);

    BOOL ResetKeyEnumerator();
    BOOL GetNextKey(CWinRegKey&);

    BOOL ResetValueEnumerator();
    BOOL GetNextValue(AWinRegValue** ppwrvNext);

    void FreeValue(AWinRegValue*&);
protected:

private:
    HKEY	m_hkRoot;
    HKEY	m_hkThis;
    CHXString	m_sPath;
    DWORD	m_dwKeyEnumPos;
    DWORD	m_dwValueEnumPos;
    REGSAM	m_rsAccess;
    BOOL	m_bOpen;
};

