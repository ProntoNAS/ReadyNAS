@ECHO OFF
REM 
REM ***** BEGIN LICENSE BLOCK *****
REM Source last modified: $Id: build.bat,v 1.1.1.1 2006/03/29 16:45:39 hagi Exp $
REM 
REM Portions Copyright (c) 1995-2004 RealNetworks, Inc. All Rights Reserved.
REM 
REM The contents of this file, and the files included with this file,
REM are subject to the current version of the RealNetworks Public
REM Source License (the "RPSL") available at
REM http://www.helixcommunity.org/content/rpsl unless you have licensed
REM the file under the current version of the RealNetworks Community
REM Source License (the "RCSL") available at
REM http://www.helixcommunity.org/content/rcsl, in which case the RCSL
REM will apply. You may also obtain the license terms directly from
REM RealNetworks.  You may not use this file except in compliance with
REM the RPSL or, if you have a valid RCSL with RealNetworks applicable
REM to this file, the RCSL.  Please see the applicable RPSL or RCSL for
REM the rights, obligations and limitations governing use of the
REM contents of the file.
REM 
REM Alternatively, the contents of this file may be used under the
REM terms of the GNU General Public License Version 2 or later (the
REM "GPL") in which case the provisions of the GPL are applicable
REM instead of those above. If you wish to allow use of your version of
REM this file only under the terms of the GPL, and not to allow others
REM to use your version of this file under the terms of either the RPSL
REM or RCSL, indicate your decision by deleting the provisions above
REM and replace them with the notice and other provisions required by
REM the GPL. If you do not delete the provisions above, a recipient may
REM use your version of this file under the terms of any one of the
REM RPSL, the RCSL or the GPL.
REM 
REM This file is part of the Helix DNA Technology. RealNetworks is the
REM developer of the Original Code and owns the copyrights in the
REM portions it created.
REM 
REM This file, and the files included with this file, is distributed
REM and made available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY
REM KIND, EITHER EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS
REM ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES
REM OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET
REM ENJOYMENT OR NON-INFRINGEMENT.
REM 
REM Technology Compatibility Kit Test Suite(s) Location:
REM    http://www.helixcommunity.org/content/tck
REM 
REM Contributor(s):
REM 
REM ***** END LICENSE BLOCK *****
REM 
REM $Id: build.bat,v 1.1.1.1 2006/03/29 16:45:39 hagi Exp $
REM bootstrap the build system on Windows
REM uncomment these for Win95
REM if "%OS%" == "Windows_NT" goto main

set args1=%1 %2 %3 %4 %5 %6 %7 %8 %9
shift
shift
shift
shift
shift
shift
shift
shift
shift
set args2=%1 %2 %3 %4 %5 %6 %7 %8 %9
shift
shift
shift
shift
shift
shift
shift
shift
shift
set args3=%1 %2 %3 %4 %5 %6 %7 %8 %9
python %BUILD_ROOT%\bin\build %args1% %args2% %args3%
