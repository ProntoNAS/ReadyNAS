/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: kv_store_inl.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#ifndef KV_STORE_I
#define KV_STORE_I

template <class K, class V>
inline
KeyValueStore<K,V>::KeyValueStore()
{}

template <class K, class V>
inline
KeyValueStore<K,V>::~KeyValueStore()
{
    Clear();
}

template <class K, class V>
inline
int KeyValueStore<K,V>::GetCount() const
{
    return m_array.GetSize();
}

template <class K, class V>
inline
void KeyValueStore<K,V>::Clear()
{}

template <class K, class V>
inline
void KeyValueStore<K,V>::Create(int index)
{
    m_array.SetAtGrow(index, KeyValuePair<K, V>());

/*
    const KeyValuePair<K, V>& kvPair = m_array[index];
    char* pKeyStr = ClassOps<K>().Print(kvPair.Key());
    char* pValueStr = ClassOps<V>().Print(kvPair.Value());
    DPRINTF (D_ERROR,("KeyValueStore<K,V>::Create(%d) : '%s' = '%s'\n",
                      index,
                      pKeyStr,
                      pValueStr));
	
    delete [] pKeyStr;
    delete [] pValueStr;
*/
}

template <class K, class V>
inline
bool KeyValueStore<K,V>::IsSet(int index) const
{
    bool ret = false;

    if (m_array.IsSet(index))
	ret = true;
    
    return ret;
}

template <class K, class V>
inline
const K& KeyValueStore<K,V>::GetKey(int index) const
{
    return m_array.GetAt(index).Key();
}

template <class K, class V>
inline
const V& KeyValueStore<K,V>::GetValue(int index) const
{
    return m_array.GetAt(index).Value();
}

template <class K, class V>
inline
void KeyValueStore<K,V>::Print() const
{
    DPRINTF (D_ERROR, ("KeyValueStore<K,V>::Print()\n"));
    for (int i = 0; i < m_array.GetSize(); i++)
    {
	if (m_array.IsSet(i))
            m_array.GetAt(i).Print();
	else
	    DPRINTF (D_ERROR, ("KeyValueStore<K,V>::Print() : %d is empty\n",
			       i));
    }
}

#endif // KV_STORE_I
