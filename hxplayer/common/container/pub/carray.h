/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: carray.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#ifndef CARRAY_H_
#define CARRAY_H_

#include "hxcom.h"
#include "hxassert.h"

class HXEXPORT_CLASS CHXPtrArray {
public:
    CHXPtrArray();
    ~CHXPtrArray();

				// return num elements == 0
    BOOL IsEmpty() const;
				// return number of elements
    int GetSize() const;
				// return largest index
    int GetUpperBound() const;
				// set size and grow by size
    void SetSize(int nelems, int growSize=-1);


				// free un-assigned slots
    void FreeExtra();
				// free the entire array
    void RemoveAll();
				// return the value at the given index
    void* GetAt(int index) const;
				// set the value at the given index
    void SetAt(int index, void* value);
				// return reference to value at given index
    void*& ElementAt(int index);
				// set value, grow array if needed
    void SetAtGrow(int index, void* value);
				// add the element, ret index of added element
    int Add(void* value);
				// add the element if not already in array
    BOOL AddIfUnique(void* value);
				// same as GetAt()
    void* operator[](int index) const;
				// same as ElementAt()
    void*& operator[](int index);

				// insert value at index
    void InsertAt(int index, void* value, int repeat=1);
				// insert array at index
    void InsertAt(int index, CHXPtrArray* pPtrArray);
				// remove value(s) at index
    void RemoveAt(int index, int repeat=1);

				// search for value in array
    BOOL Find(void* value, int* index=NULL);
				// search for and remove first occurence
    BOOL FindAndRemoveOne(void* value);
				// search for and remove all occurences
    BOOL FindAndRemoveAll(void* value);

private:
				// not implemented
    CHXPtrArray(const CHXPtrArray&);
				// not implemented
    void operator=(const CHXPtrArray&);

				// resize the array to given size
    void Resize(int size);
				// get the size to grow array by
    int GetGrowSize(int newSize);
				// common code for insertions
    void InsertCommon(int index, int len);
    

				// total slots allocated
    int m_size;
				// number of elements in array
    int m_nelems;
				// use set grow size for resizing ops
    int m_userGrowSize;
				// default grow size if user does not set
    int m_defGrowSize;
				// data array
    void** m_pData;
    
};

///
/// IsEmpty() const
///
/// return num elements == 0
///
inline BOOL 
CHXPtrArray::IsEmpty() const
{
    return m_nelems == 0;
}

///
/// GetSize() const
///
/// return size of the array
///
inline int 
CHXPtrArray::GetSize() const
{
    return m_nelems;
}

///
/// GetUpperBound() const
///
/// return largest index
///
inline int 
CHXPtrArray::GetUpperBound() const
{
    return m_nelems - 1;
}

///
/// GetAt(int index) const
///
/// return the value at the given index
///
inline void* 
CHXPtrArray::GetAt(int index) const
{
    HX_ASSERT(index >= 0 && index < m_nelems);
    return m_pData[index];
}

///
/// SetAt(int index, void* value)
///
/// set the value at the given index
///
inline void
CHXPtrArray::SetAt(int index, void* value)
{
    HX_ASSERT(index >= 0 && index < m_nelems);
    m_pData[index] = value;
}

///
/// ElementAt(int index)
///
/// return reference to value at given index
///
inline void*& 
CHXPtrArray::ElementAt(int index)
{
    HX_ASSERT(index >= 0 && index < m_nelems);
    return m_pData[index];
}

///
/// Add(void* value)
///
/// append the element to the array
///
inline int
CHXPtrArray::Add(void* value)
{
    int ret = m_nelems;
    SetAtGrow(m_nelems, value);
    return ret;
}

///
/// void* operator[]
///
/// same as GetAt()
///
inline void*
CHXPtrArray::operator[] (int index) const
{
    return GetAt(index);
}

///
/// void*& operator[]
///
/// same as ElementAt()
///
inline void*&
CHXPtrArray::operator[] (int index)
{
    return ElementAt(index);
}

///
/// AddIfUnique(void* value)
///
/// add the element if not already in array
///
inline BOOL 
CHXPtrArray::AddIfUnique(void* value)
{
    int index;
    if (Find(value, &index)) return FALSE;
    Add(value);
    return TRUE;
}

///
/// Find(void* value, int* index=NULL)
///
/// search for value in array
///
inline BOOL 
CHXPtrArray::Find(void* value, int* index)
{
    int i = 0;
    for (void** cur = m_pData; i < m_nelems; ++i, ++cur)
    {
        if (*cur == value)
        {
            if (index) *index = i;
            return TRUE;
        }
    }
    return FALSE;
}

///
/// FindAndRemoveOne(void* value)
///
/// search for and remove first occurence
///
inline BOOL 
CHXPtrArray::FindAndRemoveOne(void* value)
{
    int index = -1;
    if (Find(value, &index) && index >= 0)
    {
        RemoveAt(index, 1);
        return TRUE;
    }
    return FALSE;
}

///
/// FindAndRemoveAll(void* value)
///
/// search for and remove all occurences
///
inline BOOL 
CHXPtrArray::FindAndRemoveAll(void* value)
{
    void** src = m_pData;
    void** dest = m_pData;

    for (int i = 0; i < m_nelems; ++i, ++src)
        if (value == *src) *dest++ = *src;

    if (src != dest)
    {
        SetSize(dest - m_pData);
        return TRUE;
    }

    return FALSE;
}

#endif /* CARRAY_H_ */
