/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hx_cmd_disp_test_inl.h,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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

#ifndef HLX_CMD_DISP_TEST_I
#define HLX_CMD_DISP_TEST_I

#include <string.h>
#include "hx_cmd_disp_test.h"
#include "hx_ut_debug.h"
#include "ut_vector.h"

template <class Parser>
inline
HLXCommandDispatchTest<Parser>::HLXCommandDispatchTest(
        const HLXCmdBasedTest& test) :	
    m_pTest(test.Clone()),
    m_trace(false),
    m_executeAll(false)
{}

template <class Parser>
inline
HLXCommandDispatchTest<Parser>::~HLXCommandDispatchTest()
{
    delete m_pTest;
    m_pTest = 0;
}

template <class Parser>
inline
const char* HLXCommandDispatchTest<Parser>::DefaultCommandLine() const
{
    return m_pTest->DefaultCommandLine();
}

template <class Parser>
inline
bool HLXCommandDispatchTest<Parser>::Init(int argc, char* argv[])
{
    bool ret = false;

    // Test parameters
    // Usage: appname <command filename> [-t] [-a]

    if (argc > 1)
    {
	if (m_parser.Init(argv[1]))
	{
	    ret = (m_pTest && m_parser.More());
	    
	    if (ret)
	    {
		// Handle options
		for (int i = 2; i < argc; ++i)
		{
		    if (strcmp(argv[i], "-t") == 0) 
			m_trace = true;
		    else if (strcmp(argv[i], "-a") == 0)
			m_executeAll = true;
		}
		
		ret = (m_pTest && m_parser.More());
	    }
	}
	else
	{
	    DPRINTF(D_ERROR, ("HLXCommandDispatchTest<Parser>::Init() : Failed to initialize parser with '%s'.\n",
			      argv[1]));
	}
    }
    else
    {
	DPRINTF(D_ERROR, ("HLXCommandDispatchTest<Parser>::Init() : Not enough parameters\n"));
    }

    return ret; 
}

template <class Parser>
inline
bool HLXCommandDispatchTest<Parser>::Start()
{
    bool ret = false;

    UTVector<HLXUnitTestCmdInfo*> cmds;

    m_pTest->GetCommandInfo(cmds);

    if (cmds.Nelements() > 0)
    {
	ret = true;

	for(; (ret || m_executeAll) && m_parser.More(); m_parser.Next())
	{
	    if (m_parser.Current().Valid() &&
		(m_parser.Current().Count() >= 1))
	    {
		bool foundCmd = false;
		
		for (int i = 0; !foundCmd && (i < cmds.Nelements()); i++)
		{
		    if (m_parser.Current()[0] != cmds[i]->CommandName())
		    {
                        // This is not the command we're looking for
		    }
		    else if (m_parser.Current().Count() < cmds[i]->ArgCount() ||
                             m_parser.Current().Count() > cmds[i]->ArgCountMax())
		    {
			// The command did not have the correct
			// number of arguments.
			DPRINTF(D_ERROR,
				("Line %d : Incorrect number of arguments (got %d; expected %d..%d)\n",
				 m_parser.LineNum(),
                                 m_parser.Current().Count(),
                                 cmds[i]->ArgCount(),
                                 cmds[i]->ArgCountMax()));
			ret = false;
		    }
		    else
		    {
                        // We found the command!
			foundCmd = true;
			
			// Copy the tokens into a parameter vector
			int paramCount = m_parser.Current().Count();
			UTVector<UTString> params(paramCount);
			for (int j = 0; j < paramCount; j++)
			    params[j] = m_parser.Current()[j];

                        if (m_trace)
                        {
                            DPRINTF(D_INFO,
                                    ("Line %d : Executing '%s' command\n",
                                     m_parser.LineNum(),
                                     (const char*)params[0]));
                        }

			bool bResult = cmds[i]->Execute(params);

			if (!bResult)
                        {
                            DPRINTF(D_ERROR,
				    ("Line %d : Test command '%s' FAILED\n",
				     m_parser.LineNum(),
                                     (const char*)params[0]));

			    ret = false;
                        }
	
		    }
		}

		if (!foundCmd)
                {
                    DPRINTF(D_ERROR,
                            ("Line %d : Failed to find command '%s'\n",
                             m_parser.LineNum(),
                             (const char*)m_parser.Current()[0]));
		    ret = false;
                }
	    }
	}
    }

    // Clean up all the command info objects
    for (int k = 0; k < cmds.Nelements(); k++)
    {
        delete cmds[k];
	cmds[k] = 0;
    }

    return ret;
}

template <class Parser>
inline
void HLXCommandDispatchTest<Parser>::Reset()
{
    m_parser.Reset();
}

#endif // HLX_CMD_DISP_TEST_I
