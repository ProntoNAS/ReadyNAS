/* ***** BEGIN LICENSE BLOCK *****
 * Source last modified: $Id: maclibrary.cp,v 1.1.1.1 2006/03/29 16:45:31 hagi Exp $
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

////////////////////
//
//	This file contains functions to load & release Shared Libraries
//

#include "platform/mac/maclibrary.h"
#include "hxresult.h"
#include "platform/mac/HX_MoreProcesses.h"
#include "hxstring.h"
#ifndef _MAC_MACHO
#include <CodeFragments.h>
#endif
/////////////////////////////////////////////////////////////////////////////
//
//	Function:
//
//		FindLibrary()
//
//	Purpose:
//
//		Called to get the fragment/file name of a Mac Shared Library given
//		the library's creator and codec ID. The Codec ID is stored in STR#9000;
//		item #1 of the library.
//
//	Parameters:
//
//		HX_MOFTAG moftCodecTag
//		The four character codec identifier (RV10, dnet, ...)
//
//		OSType codecCreator
//		The Creator type of the codec; currently kDecoderCreator or kEncoderCreator
//		
//		char* pLibName
//		Allocate this before calling FindLibrary. Library file name is
//		returned in it. Library's fragment name should be same as file name!!
//
//	Return:
//
//		HX_RESULT
//		Error code indicating the success or failure of the function.
//
#if defined(_CARBON) || defined(_MAC_UNIX)
HX_RESULT FindLibrary(ULONG32 moftCodecTag, OSType codecCreator, char* pLibName, FSSpec &fsLibSpec)
{
	// FindLibrary searches messily through files in the Extensions folder; this is obviously
	// pointless for Mac OS X
	
	HX_ASSERT(!"Greg sez: FindLibrary needs to be rethought and rewritten for Carbon");
	
	return HXR_DEC_NOT_FOUND;
}

short OpenLibraryRes(FSSpec *libSpec, OSType codecCreator)
{
	HX_ASSERT(!"Wow, OpenLibraryRes did not appear to be used");
	return 0;
}

#else // !defined _CARBON

HX_RESULT FindLibrary(HX_MOFTAG moftCodecTag, OSType codecCreator, char* pLibName, FSSpec &fsLibSpec)
{
	HX_RESULT err = HXR_DEC_NOT_FOUND;
	//find the Sys Ä/Extensions/RealAudio folder
	FSSpec raSpec;
	if (noErr == ::FindFolder (-1, kExtensionFolderType, kDontCreateFolder, &raSpec.vRefNum, &raSpec.parID))
	{		
		::FSMakeFSSpec (raSpec.vRefNum, raSpec.parID, "\pReal", &raSpec); //get str fm resource?
		
		//FSSpec libSpec;
		int i=1;
		short rfRefNum;
		Str255 strID;
		short currRes = ::CurResFile();
		Handle hRsrc;
		short iCnt;
		
		//iterate through all files in RealAudio directory
		while (	FSpIterateDirectory(&raSpec, i, &fsLibSpec))
		{
			Boolean	tempbool=false;
		
		    ResolveAliasFile(&fsLibSpec,true,&tempbool,&tempbool);
		
			rfRefNum = OpenLibraryRes(&fsLibSpec, codecCreator);	
			if (rfRefNum)
			{

				strID[0] = 0;
				// check if file has expected STR# resources defining codec ID
				iCnt = ::Count1Resources('STR#');
				if (iCnt)
				{
					hRsrc = ::Get1Resource('STR#',kCodecIDStrResID);
					if (hRsrc)
						::GetIndString(strID,kCodecIDStrResID,kCodecIDStrItem);
					if (strID[0])
					{
						long lValue = 0;             
						memcpy(&lValue, &strID[1], sizeof(long));
						if (lValue == moftCodecTag)
						{
							strncpy(pLibName, (char*)&fsLibSpec.name[1], fsLibSpec.name[0]); 
							pLibName[fsLibSpec.name[0]] = 0;
							err = HXR_OK;
							break;
						}
						
					} //if (strID[0])
				} // if iCnt
				::CloseResFile(rfRefNum);
				::UseResFile(currRes);
				
			} //if rfRefNum
			i++;
		}	//while
	} //if FindFolder	
	
	if (err == HXR_DEC_INIT_FAILED)
	{
		// look elsewhere?
	}
	
	return err;
} //end FindLibrary

short OpenLibraryRes(FSSpec *libSpec, OSType codecCreator)
{
	FInfo fndrInfo;
	short rfRefNum = 0;
	AliasHandle alias = NULL;
	
	OSErr err = ::FSpGetFInfo(libSpec,&fndrInfo);
	
	if (err == noErr && fndrInfo.fdCreator == codecCreator)
	{
		const Boolean kResolveAliasChains = true;
		Boolean isFolder, wasAliased;
		
		err = ResolveAliasFile(libSpec, kResolveAliasChains, &isFolder, &wasAliased);
		check_noerr(err);
		
		rfRefNum = ::FSpOpenResFile(libSpec, fsRdPerm);
		if (rfRefNum == -1)
		{
			rfRefNum = 0;
		}
	}
	return rfRefNum;
}
#endif // !defined _CARBON




/////////////////////////////////////////////////////////////////////////////
//
//	Function:
//
//		LoadLibrary()
//
//	Purpose:
//
//		Called to load a Mac Shared Library given the library's fragment name.
//
//	Parameters:
//
//		char* dllname
//		The fragment name of the library.
//
//	Return:
//
//		ULONG32
//		Returns the ConnectionID
//

#if defined(_CARBON) || defined(_MAC_UNIX)

#include "filespecutils.h"

#if defined(_DEBUG) && !defined(_MAC_UNIX)
static void DebugReportAnyLoadFailure(OSErr err, StringPtr errMsgPascStr, 
	const char *pLoadCallName, ConstStr255Param libNameStr, FSSpec* fileFSSpec)
{
//	if (err != cfragUnresolvedErr && err != cfragNoLibraryErr) return;  naw, we'll report any errors now

	if (err == noErr) return;
	
	CHXString strErrMsg, strLibName, strPath, msg;
	Str255 pascMsg;

	strErrMsg.SetFromStr255(errMsgPascStr);
	strLibName.SetFromStr255(libNameStr);
	
	if (fileFSSpec)
	{
		strPath = *fileFSSpec;
	}
	
	msg.Format("%s failed, err=%d  lib: %s  path: %s  errMsg: %s",
		pLoadCallName, err, 
		(const char *) strLibName, (const char *) strPath,
		(const char *) strErrMsg);
	msg.MakeStr255(pascMsg);
	DebugStr(pascMsg);
}
#else
static inline void DebugReportAnyLoadFailure(OSErr, StringPtr , const char *, ConstStr255Param, FSSpec* ) 
{ 
}
#endif


#ifdef _MAC_MACHO

HINSTANCE LoadLibrary(const char* dllname)
{
    // developer.apple.com has a bunch of stuff about bundles.
    // I don't have the URL handy but this is from a page
    // with "Locating and Opening Bundles" as a title.
    
    // xxxbobclark assume for this instand that dll bundles will live alongside the
    // app package, i.e. so you can run something straight from the release folder
    // when you're done with the build.
    
    CFURLRef theBundleURL;
    CFBundleRef myBundle;
    CFStringRef theBundleName = ::CFStringCreateWithCString(NULL, dllname, kCFStringEncodingMacRoman);
    
    if (strstr(dllname, "/"))
    {
        // hmmm, it's a path. At least it has slashes.
        
        theBundleURL = ::CFURLCreateWithFileSystemPath(NULL, theBundleName, kCFURLPOSIXPathStyle, true);
    }
    else
    {
        // just the library name, so assume it's alongside the app package.
        CFBundleRef mainBundle;
        CFURLRef mainBundleURL;
        CFURLRef updirURL;
        
        // get the main bundle for the app
        mainBundle = ::CFBundleGetMainBundle();
        
        mainBundleURL = ::CFBundleCopyBundleURL( mainBundle );
        updirURL = ::CFURLCreateCopyDeletingLastPathComponent(NULL, mainBundleURL);
        theBundleURL = ::CFURLCreateCopyAppendingPathComponent(NULL, updirURL, theBundleName, true);
        
        ::CFRelease(updirURL);
        ::CFRelease(mainBundleURL);
    }
    
    CFStringRef bundlePath = ::CFURLCopyPath(theBundleURL);
    
    // make a bundle instance using the URLRef
    myBundle = ::CFBundleCreate( kCFAllocatorDefault, theBundleURL );
    
    // release that which must (may?) be released...
    ::CFRelease(theBundleName);
    ::CFRelease(theBundleURL);
    
    return (HINSTANCE)myBundle;
}

#else

HINSTANCE LoadLibrary(const char* dllname) // Carbon (CFM) version
{
	CFragConnectionID		connID = 0; // reference ID to shared lib
	Ptr				mainAddr = nil;
	Str255 				errMsg;
	OSErr				err = noErr;
	CHXString			strFullName;
	
	check(dllname != NULL && dllname[0] != 0);
	
	strFullName = dllname;
	
	// if there's a colon, it's a full path; if there's no colon, 
	// it's a file name to be loaded using the implicitly search path
	
	if (strFullName.Find(':') == -1)
	{
		// we were given a simple library name; load it with the implicit system search path

		Str255 pascFullName;		
		
		c2pstrcpy(pascFullName, (const char *) strFullName);
		
		err = ::GetSharedLibrary(pascFullName, kPowerPCCFragArch, kLoadCFrag, &connID, &mainAddr, errMsg);	
		
		// xxxbobclark GetSharedLibrary is case-sensitive! so we may have just asked for
		// i.e. "RV30.shlb" and failed, while "rv30.shlb" exists.
		// Since we tend to use lower-case we'll force it to lower-case and see whether
		// we can find it this time.
		if (err == -2804)
		{
		    strFullName.MakeLower();
		    c2pstrcpy(pascFullName, (const char *) strFullName);
		    err = ::GetSharedLibrary(pascFullName, kPowerPCCFragArch, kLoadCFrag, &connID, &mainAddr, errMsg);	
		}

		DebugReportAnyLoadFailure(err, errMsg, "GetDiskFragment()", pascFullName, nil);
	}
	else
	{
		// we were given a path to a file
		
		// change shortcut prefix like ÇASUPÈ and ÇAPPLÈ into the full correct paths
		ResolveIndependentPath(strFullName);
		
		// make a file spec from the path, resolve if it if it points to an alias file,
		// and load the library from disk
		CHXFileSpecifier dllFileSpec = strFullName;
		
		if (SUCCEEDED(CHXFileSpecUtils::ResolveFileSpecifierAlias(dllFileSpec)))
		{
                        FSSpec dllFSSpec = (FSSpec) dllFileSpec;
                        err = ::GetDiskFragment (&dllFSSpec, 0, kCFragGoesToEOF, dllFSSpec.name, 
                                                 kLoadCFrag, &connID, &mainAddr, errMsg);
		
                        DebugReportAnyLoadFailure(err, errMsg, "GetDiskFragment()", dllFSSpec.name, &dllFSSpec);
                }
	}
	
	return (HINSTANCE) connID;
}

#endif // else block from _MAC_MACHO

#else // !defined _CARBON
HINSTANCE LoadLibrary(const char* dllname) // pre-Carbon version
{
	CFragConnectionID	connID = 0; //reference ID to shared lib
	Ptr					mainAddr = nil;
	Str255 				errMsg;
	OSErr				err=noErr;
	CHXString			fullpath;
	FSSpec				spec;
	Boolean				tempbool=false;
	
	HX_ASSERT(dllname);
	
	fullpath = dllname; 
	// replace special prefixes (APPL, ASUP,..) with actual path
	ResolveIndependentPath(fullpath);
	spec = fullpath;
	
	err = ResolveAliasFileWithMountFlags(&spec,true,&tempbool,&tempbool,kResolveAliasFileNoUI);
	
	
	err = GetDiskFragment (&spec, 0, kCFragGoesToEOF, spec.name, kLoadCFrag, &connID, &mainAddr, errMsg);

	if (err) 
	{
		if (0 == spec.vRefNum)
		{
			// if spec is not valid (ie. spec.name is empty) use file name
			// so that it tries to load lib from System folder or app folder
			// this code
			char fileName[256];
			int copyLen = strlen(dllname);
			char* pFileStart = strrchr(dllname, ':');
			if (pFileStart)
			{
				pFileStart++; // skip over ':'
				copyLen -= (pFileStart - dllname);
			}
			else
			{
				pFileStart = (char*)dllname;
			}
			
			strncpy(&fileName[1], pFileStart, copyLen);
			fileName[0] = copyLen;
			err = ::GetSharedLibrary((ConstStr63Param)fileName, kPowerPCCFragArch, kLoadCFrag, &connID, &mainAddr, errMsg);	
		}
		else
		{
			err = ::GetSharedLibrary(spec.name, kPowerPCCFragArch, kLoadCFrag, &connID, &mainAddr, errMsg);	
		}
	}


	return (HINSTANCE)connID;
}
#endif

/////////////////////////////////////////////////////////////////////////////
//
//	Function:
//
//		FreeLibrary()
//
//	Purpose:
//
//		Called to free a Mac Shared Library. If this is not called the library
//		will be freed when the application quits.
//
//	Parameters:
//
//		HMODULE lib
//		This is actually the ConnectionID.
//
//	Return:
//
//		none
//

#ifdef _MAC_MACHO
void FreeLibrary(HINSTANCE lib)
{
    ::CFRelease((HINSTANCE)lib);
}
#else
void FreeLibrary(HINSTANCE lib)
{
	CFragConnectionID	connID = (CFragConnectionID)lib;

	::CloseConnection(&connID);
}
#endif

/////////////////////////////////////////////////////////////////////////////
//
//	Function:
//
//		GetProcAddress()
//
//	Purpose:
//
//		Called to get a function pointer in a Mac Shared Library.
//
//	Parameters:
//
//		HMODULE lib
//		This is the ConnectionID that is returned from LoadLibrary
//
//		char* function
//		The function name
//
//	Return:
//
//		void*
//		The address of the function.
//
#ifdef _MAC_MACHO
void* GetProcAddress(HMODULE lib, char* function)
{
    void* theAddr = NULL;
    bool didLoad = false;
    
    CFStringRef functionString = ::CFStringCreateWithCString(NULL, function, kCFStringEncodingMacRoman);
    
    didLoad = ::CFBundleLoadExecutable((CFBundleRef)lib);
    
    if (didLoad)
    {
        theAddr = (void*)::CFBundleGetFunctionPointerForName((CFBundleRef)lib, functionString);
    }
    else
    {
    }
    return theAddr;
}
#else
void* GetProcAddress(HMODULE lib, char* function)
{
	Ptr					symAddr = nil;
	CFragSymbolClass	symClass;
	CFragConnectionID	connID = (CFragConnectionID)lib;
	
	Str255 strFuncName;
	strcpy((char *)&strFuncName[1], function);
	strFuncName[0] = strlen(function);


//		FindSymbol doesn't actually return the address of the function;
//		it returns a pointer to a TOC entry in the code fragment
	OSErr err = ::FindSymbol(connID, (ConstStr63Param)((Str255*)strFuncName), &symAddr, &symClass);

	return symAddr;
}
#endif

#if !defined(_MAC_UNIX)

//
// ResolveIndependentPath checks the supplied full path to see
// if it's prefixed by a location indicator (like 'ÇAPPLÈ:'
// or 'ÇasupÈ:'.  If so, it changes the prefix to the appropriate
// path beginning.
//
// If the supplied path does not contain a prefix, or the folder type
// isn't recognized by FindFolder, the string is not changed.
//
// Returns TRUE if the path string was changed, FALSE otherwise.
//

BOOL ResolveIndependentPath(CHXString& strPath)
{
	BOOL		result;
	CHXString 	startpath;
	
	const short	kPrefixLength = 6;	// ÇcodeÈ
		
	// assume we aren't altering the string
	result = FALSE;
	
	// look for proper characters preceding the first colon;
	// folder should be bracketed by Ç È
	
	startpath = strPath.NthField(':', 1);
	if (startpath.GetLength() == kPrefixLength
		&& startpath[0] == 'Ç' 
		&& startpath[5] == 'È')
	{
		FSSpec		folderSpec;
		short		foundVRefNum;
		long		foundDirID;
		OSType		foldType;
		OSErr		err;
		
		// extract the folder type
		BlockMoveData(1 + (const char *) startpath, &foldType, sizeof(OSType));
		
		// handle APPL as a the path to the current application,
		// otherwise use FindFolder
		
		if (foldType == 'APPL')
		{
			// our desired dirID and vRefNum are those of the app's directory
			(void) GetCurrentAppSpec(&folderSpec);
			foundVRefNum = folderSpec.vRefNum;
			foundDirID = folderSpec.parID;
			err = noErr;
		}
		else
		{
			// use FindFolder to locate the desired directory
			err = FindFolder(kOnSystemDisk, foldType, kCreateFolder,
				  			 &foundVRefNum, &foundDirID);
			HX_ASSERT(err == noErr);
		}
		
		if (err == noErr)
		{
			CHXString	restpath;
			
			// no problem finding the desired folder
			//
			// make a path up to the desired folder, ending in a colon
			
			err = FSMakeFSSpec(foundVRefNum, foundDirID, "\p", &folderSpec);
			HX_ASSERT(err == noErr);
			
			startpath = folderSpec; // coerce the file spec to a path
			
			if (startpath[startpath.GetLength() - 1] != ':')
			{
				startpath += ':';
			}
			
			// make a full path from the new startpath plus the rest of the original
			restpath = strPath.Mid(kPrefixLength + 1);
			
			strPath = startpath + restpath;
			
			result = TRUE;	// indicate that the string's been changed
		}
	}
	return result;
}

#endif
