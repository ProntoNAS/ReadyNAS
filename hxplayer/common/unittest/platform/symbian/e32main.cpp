/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: e32main.cpp,v 1.1.1.1 2006/03/29 16:45:28 hagi Exp $
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
#include <stdio.h>
#include <string.h>
#include <e32base.h>
#include <bacline.h>
#include "hx_simple_test_driver.h"
#include "hx_ut_debug.h"

// Detects whether we were launched from the file browser or not
static bool LaunchedFromFileBrowser(int argc, char* argv[])
{
    return (strcmp(argv[0], argv[1]) == 0);
}

TInt E32Main()
{
    TInt ret = -1;
    CTrapCleanup* pCleanupStack = CTrapCleanup::New();

    CCommandLineArguments* pCmdLine = 0;
    TRAPD(trapErr, pCmdLine = CCommandLineArguments::NewL());

    if ((trapErr == KErrNone) && pCmdLine && pCleanupStack)
    {
	int argc = pCmdLine->Count();
	char** argv = new char*[argc];

	if (argc && argv)
	{
	    for (int i = 0; i < argc; i++)
	    {
		TPtrC arg = pCmdLine->Arg(i);

		argv[i] = new char[arg.Length() + 1];

		if (argv[i])
		{
		    for (int j = 0; j < arg.Length(); j++)
		    {
			argv[i][j] = (char)arg[j];
		    }

		    argv[i][arg.Length()] = '\0';
		}
	    }


	    CHXSimpleTestDriver testDriver;
	    
	    if ((argc > 1) &&
		!LaunchedFromFileBrowser(argc, argv))
	    {
		if (testDriver.Run(argc, argv))
		    ret = 0;
	    }
	    else if (testDriver.Run())
		ret = 0;
	}

	for (int k = 0; k < argc; k++)
	{
	    delete [] argv[k];
	}

	delete [] argv;
    }

    // Wait for a keypress before exitting
    fgetc(stdin);

    delete pCleanupStack;
    delete pCmdLine;    

    return ret;
}
