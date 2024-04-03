/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: ut_vector.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#ifndef UT_VECTOR_H
#define UT_VECTOR_H

template <class T>
class UTVector
{
public:
    UTVector();
    UTVector(int i);
    ~UTVector();

    void Resize(int newSize);
    int Nelements() const;

    T& operator[](int i);
    const T& operator[](int i) const;

private:
    UTVector(const UTVector<T>& rhs);
    UTVector& operator=(const UTVector<T>& rhs);

    T* m_pVec;
    int m_size;
};


template <class T>
inline
UTVector<T>::UTVector() :
    m_pVec(0),
    m_size(0)
{}

template <class T>
inline
UTVector<T>::UTVector(int size) :
    m_pVec(new T[size]),
    m_size(size)
{}

template <class T>
inline
UTVector<T>::~UTVector()
{
    delete [] m_pVec;
    m_pVec = 0;
}

template <class T>
inline
void UTVector<T>::Resize(int newSize)
{
    if (newSize != m_size)
    {
	T* pNewVec = new T[newSize];
	
	int numToCopy = (newSize > m_size) ? m_size : newSize;

	for (int i = 0; i < numToCopy; i++)
	{
	    pNewVec[i] = m_pVec[i];
	}

	m_size = newSize;
	delete [] m_pVec;
	m_pVec = pNewVec;
    }
}

template <class T>
inline
int UTVector<T>::Nelements() const
{
    return m_size;
}

template <class T>
inline
T& UTVector<T>::operator[](int i)
{
    return m_pVec[i];
}

template <class T>
inline
const T& UTVector<T>::operator[](int i) const
{
    return m_pVec[i];
}

#endif /* UT_VECTOR_H */
