/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: xmlwrite.h,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

// $Id: xmlwrite.h,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $

/* 
 * Abstraction:
 * This file contains the declaration of the XMLWriter class.  This class will be used
 * to write out to file properly structured XML according to the XML 1.0 Spec by W3C.
 *
 */
#ifndef _XMLWRITER_H_
#define _XMLWRITER_H_

// Includes for this file...
#include "hxwintyp.h"
#include "looseprs.h"
#include "hxslist.h"
#include "hxcom.h"
#include "hxbuffer.h"

// Forward declarations...
class XMLWriterTag;
class XMLWriterAttribute;
class XMLWriter;
class XMLWriterElement;

// XMLWriterTag class declaration...
class XMLWriterTag 
{
public:
    friend class XMLWriterAttribute;
    friend class XMLWriter;

public:
    XMLWriterTag(void);
    virtual ~XMLWriterTag(void);

    XMLWriterTag *CreateTag(const char *name = NULL);
    XMLWriterAttribute *AddAttribute(const char *name, const char *value);
    XMLWriterAttribute *CreateAttribute(void);
    void SetName(const char *name);
    void SetComment(const char *comment);
    void SetProcessingInstruction(const char *instruction);

protected:
    void WriteAttributes(IHXBuffer *buffer, INT32& loc);
    void WriteTags(IHXBuffer *buffer, INT32& loc);
    void Write(IHXBuffer *buffer, INT32& loc);
    INT32 GetLength(void) const;

    XMLTagType m_type;
    char *m_comment;
    char *m_name;
    CHXSimpleList m_tags;
    CHXSimpleList m_attributes;    
    INT32 m_depth;

};


// XMLWriterAttribute class declaration...
class XMLWriterAttribute 
{
public:
    friend class XMLWriterTag;
    friend class XMLWriter;

public:
    XMLWriterAttribute(void);
    virtual ~XMLWriterAttribute(void);

    void SetName(const char *name);
    void SetValue(const char *value);

protected:
    void Write(IHXBuffer *buffer, INT32& loc);   
    INT32 GetLength(void) const;
    char *m_name;
    char *m_value;
    INT32 m_depth;

};


// XMLWriter class declaration...
class XMLWriter
{
public:
    friend class XMLWriterTag;
    friend class XMLWriterAttribute;

public:
    XMLWriter(void);
    virtual ~XMLWriter(void);

    XMLWriterTag *CreateTag(const char *name = NULL);
    BOOL Write(IHXBuffer *buffer, INT32& loc);
    void Clear(void);

protected:
    INT32 GetLength(void) const;
    CHXSimpleList m_tags;


};



#endif // _XMLWRITER_H_

