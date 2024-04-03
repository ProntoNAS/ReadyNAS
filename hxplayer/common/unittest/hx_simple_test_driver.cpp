/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: hx_simple_test_driver.cpp,v 1.1.1.1 2006/03/29 16:45:27 hagi Exp $
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

#include "hx_ut_debug.h"
#include "hx_simple_test_driver.h"
#include "ut_cmd_line.h"

CHXSimpleTestDriver::CHXSimpleTestDriver()
{}

CHXSimpleTestDriver::~CHXSimpleTestDriver()
{}

bool CHXSimpleTestDriver::Run(int argc, char* argv[])
{
    bool ret = false;
    bool allocatedArgv = false;

    if ((argc < 1) || (!argv))
    {
	// Construct default parameters
	argc = 1;
	argv = new char*[argc];
	argv[0] = "testdriver";

	allocatedArgv = true;
    }

    HLXUnitTest* pTest = BuildTest();
    
    if (pTest)
    {
	ret = RunTest(pTest, argc, argv);

	delete pTest;
    }
    else
    {
	DPRINTF(D_ERROR, ("Unit test creation FAILED\n"));
    }
    
    if (allocatedArgv)
    {
	delete [] argv;
    }
    
    return ret;
}

bool CHXSimpleTestDriver::Run(const char* pCmdLine)
{
    HLXUnitTest* pTest = BuildTest();

    bool ret = false;

    if (pTest)
    {
	ret = RunTest(pTest, pCmdLine);

	delete pTest;
    }
    else
    {
	DPRINTF(D_ERROR, ("Unit test creation FAILED\n"));
    }

    return ret;
}

bool CHXSimpleTestDriver::Run()
{
    HLXUnitTest* pTest = BuildTest();

    bool ret = false;

    DPRINTF(D_ERROR, ("Using default command-line\n"));

    if (pTest)
    {
	ret = RunTest(pTest, pTest->DefaultCommandLine());

	delete pTest;
    }
    else
    {
	DPRINTF(D_ERROR, ("Unit test creation FAILED\n"));
    }

    return ret;
}

bool CHXSimpleTestDriver::RunTest(HLXUnitTest* pTest, const char* pCmdLine)
{
    bool ret = false;

    UTCommandLine cmdLine;
    
    if (cmdLine.Parse(pCmdLine))
    {
	ret = RunTest(pTest, cmdLine.Argc(), cmdLine.Argv());
    }
    else
    {
	DPRINTF(D_ERROR, ("Failed to parse command line\n"));
    }

    return ret;
}

bool CHXSimpleTestDriver::RunTest(HLXUnitTest* pTest, int argc, char** argv)
{
    bool ret = false;

    if ((argc <= 0) || !argv)
    {
	DPRINTF(D_ERROR, ("CHXSimpleTestDriver::RunTest(): Invalid parameters\n"));
    }
    else if( !pTest->Init(argc, argv))
    {
	DPRINTF(D_ERROR, ("Unit test initialization FAILED\n"));
    }
    else if (!pTest->Start())
    {
	DPRINTF(D_ERROR, ("Unit test FAILED\n"));
    }
    else
    {
	DPRINTF(D_ERROR, ("Unit test PASSED\n"));
	ret = true;
    }

    return ret;
}

