/*
Copyright 2005,2006 by the Massachusetts Institute of Technology

All rights reserved.

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of the Massachusetts
Institute of Technology (M.I.T.) not be used in advertising or publicity
pertaining to distribution of the software without specific, written
prior permission.

M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

*/

#include "kfwlogon.h"

#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <winsock2.h>
#include <lm.h>
#include <nb30.h>

static HANDLE hDLL;

static HANDLE hInitMutex = NULL;
static BOOL bInit = FALSE;


BOOLEAN APIENTRY DllEntryPoint(HANDLE dll, DWORD reason, PVOID reserved)
{
    hDLL = dll;
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        /* Initialization Mutex */
        hInitMutex = CreateMutex(NULL, FALSE, NULL);
        break;

    case DLL_PROCESS_DETACH:
        CloseHandle(hInitMutex);
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    default:
        /* Everything else succeeds but does nothing. */
        break;
    }

    return TRUE;
}



DWORD APIENTRY NPGetCaps(DWORD index)
{
    switch (index) {
    case WNNC_NET_TYPE:
        /* Don't have our own type; use somebody else's. */
        return WNNC_NET_SUN_PC_NFS;

    case WNNC_START:
        /* Say we are already started, even though we might wait after we receive NPLogonNotify */
        return 1;

    default:
        return 0;
    }
}       

static BOOL
WINAPI
UnicodeStringToANSI(UNICODE_STRING uInputString, LPSTR lpszOutputString, int nOutStringLen)
{
    CPINFO CodePageInfo;

    GetCPInfo(CP_ACP, &CodePageInfo);

    if (CodePageInfo.MaxCharSize > 1)
        // Only supporting non-Unicode strings
        return FALSE;
    
    if (uInputString.Buffer && ((LPBYTE) uInputString.Buffer)[1] == '\0')
    {
        // Looks like unicode, better translate it
        // UNICODE_STRING specifies the length of the buffer string in Bytes not WCHARS
        WideCharToMultiByte(CP_ACP, 0, (LPCWSTR) uInputString.Buffer, uInputString.Length/2,
                            lpszOutputString, nOutStringLen-1, NULL, NULL);
        lpszOutputString[min(uInputString.Length/2,nOutStringLen-1)] = '\0';
        return TRUE;
    }

    lpszOutputString[0] = '\0';
    return FALSE;
}  // UnicodeStringToANSI


DWORD APIENTRY NPLogonNotify(
	PLUID lpLogonId,
	LPCWSTR lpAuthentInfoType,
	LPVOID lpAuthentInfo,
	LPCWSTR lpPreviousAuthentInfoType,
	LPVOID lpPreviousAuthentInfo,
	LPWSTR lpStationName,
	LPVOID StationHandle,
	LPWSTR *lpLogonScript)
{
    char uname[MAX_USERNAME_LENGTH]="";
    char password[MAX_PASSWORD_LENGTH]="";
    char logonDomain[MAX_DOMAIN_LENGTH]="";
    char szLogonId[128] = "";

    MSV1_0_INTERACTIVE_LOGON *IL;

    DWORD code = 0;

    char *reason;
    char *ctemp;

    BOOLEAN interactive;
    HWND hwndOwner = (HWND)StationHandle;
    BOOLEAN lowercased_name = TRUE;

    if ( !KFW_is_available() )
	return 0;

    DebugEvent("NPLogonNotify - LoginId(%d,%d)", lpLogonId->HighPart, lpLogonId->LowPart);

    /* Initialize Logon Script to none */
    *lpLogonScript=NULL;
    
    /* MSV1_0_INTERACTIVE_LOGON and KERB_INTERACTIVE_LOGON are equivalent for
     * our purposes */

    if ( wcscmp(lpAuthentInfoType,L"MSV1_0:Interactive") && 
         wcscmp(lpAuthentInfoType,L"Kerberos:Interactive") )
    {
	char msg[64];
	WideCharToMultiByte(CP_ACP, 0, lpAuthentInfoType, 0, 
			    msg, sizeof(msg), NULL, NULL);
	msg[sizeof(msg)-1]='\0';
        DebugEvent("NPLogonNotify - Unsupported Authentication Info Type: %s", msg);
        return 0;
    }

    IL = (MSV1_0_INTERACTIVE_LOGON *) lpAuthentInfo;

    /* Are we interactive? */
    interactive = (wcscmp(lpStationName, L"WinSta0") == 0);

    /* Convert from Unicode to ANSI */

    /*TODO: Use SecureZeroMemory to erase passwords */
    if (!UnicodeStringToANSI(IL->UserName, uname, MAX_USERNAME_LENGTH) ||
	!UnicodeStringToANSI(IL->Password, password, MAX_PASSWORD_LENGTH) ||
	!UnicodeStringToANSI(IL->LogonDomainName, logonDomain, MAX_DOMAIN_LENGTH))
	return 0;

    /* Make sure AD-DOMAINS sent from login that is sent to us is stripped */
    ctemp = strchr(uname, '@');
    if (ctemp) *ctemp = 0;

    /* is the name all lowercase? */
    for ( ctemp = uname; *ctemp ; ctemp++) {
        if ( !islower(*ctemp) ) {
            lowercased_name = FALSE;
            break;
        }
    }

    code = KFW_get_cred(uname, password, 0, &reason);
    DebugEvent("NPLogonNotify - KFW_get_cred  uname=[%s] code=[%d]",uname, code);
    
    /* remove any kerberos 5 tickets currently held by the SYSTEM account
     * for this user 
     */
    if (!code) {
	sprintf(szLogonId,"kfwlogon-%d.%d",lpLogonId->HighPart, lpLogonId->LowPart);
	KFW_copy_cache_to_system_file(uname, szLogonId);
    }

    KFW_destroy_tickets_for_principal(uname);

    if (code) {
        char msg[128];
        HANDLE h;
        char *ptbuf[1];

        StringCbPrintf(msg, sizeof(msg), "Kerberos ticket acquisition failed: %s", reason);

        h = RegisterEventSource(NULL, KFW_LOGON_EVENT_NAME);
        ptbuf[0] = msg;
        ReportEvent(h, EVENTLOG_WARNING_TYPE, 0, 1008, NULL,
                     1, 0, ptbuf, NULL);
        DeregisterEventSource(h);
        SetLastError(code);
    }

    if (code)
	DebugEvent0("NPLogonNotify failure");
    else
	DebugEvent0("NPLogonNotify success");

    return code;
}       


DWORD APIENTRY NPPasswordChangeNotify(
	LPCWSTR lpAuthentInfoType,
	LPVOID lpAuthentInfo,
	LPCWSTR lpPreviousAuthentInfoType,
	LPVOID lpPreviousAuthentInfo,
	LPWSTR lpStationName,
	LPVOID StationHandle,
	DWORD dwChangeInfo)
{
    return 0;
}

#include <userenv.h>
#include <Winwlx.h>

#ifdef COMMENT
typedef struct _WLX_NOTIFICATION_INFO {  
    ULONG Size;  
    ULONG Flags;  
    PWSTR UserName;  
    PWSTR Domain;  
    PWSTR WindowStation;  
    HANDLE hToken;  
    HDESK hDesktop;  
    PFNMSGECALLBACK pStatusCallback;
} WLX_NOTIFICATION_INFO, *PWLX_NOTIFICATION_INFO;
#endif

VOID KFW_Startup_Event( PWLX_NOTIFICATION_INFO pInfo )
{
    DebugEvent0("KFW_Startup_Event");
}

static BOOL
GetSecurityLogonSessionData(HANDLE hToken, PSECURITY_LOGON_SESSION_DATA * ppSessionData)
{
    NTSTATUS Status = 0;
#if 0
    HANDLE  TokenHandle;
#endif
    TOKEN_STATISTICS Stats;
    DWORD   ReqLen;
    BOOL    Success;

    if (!ppSessionData)
        return FALSE;
    *ppSessionData = NULL;

#if 0
    Success = OpenProcessToken( HANDLE GetCurrentProcess(), TOKEN_QUERY, &TokenHandle );
    if ( !Success )
        return FALSE;
#endif

    Success = GetTokenInformation( hToken, TokenStatistics, &Stats, sizeof(TOKEN_STATISTICS), &ReqLen );
#if 0
    CloseHandle( TokenHandle );
#endif
    if ( !Success )
        return FALSE;

    Status = LsaGetLogonSessionData( &Stats.AuthenticationId, ppSessionData );
    if ( FAILED(Status) || !ppSessionData )
        return FALSE;

    return TRUE;
}

VOID KFW_Logon_Event( PWLX_NOTIFICATION_INFO pInfo )
{
    WCHAR szUserW[128] = L"";
    char  szUserA[128] = "";
    char szPath[MAX_PATH] = "";
    char szLogonId[128] = "";
    DWORD count;
    char filename[MAX_PATH] = "";
    char newfilename[MAX_PATH] = "";
    char commandline[MAX_PATH+256] = "";
    STARTUPINFO startupinfo;
    PROCESS_INFORMATION procinfo;
    HANDLE hf = NULL;

    LUID LogonId = {0, 0};
    PSECURITY_LOGON_SESSION_DATA pLogonSessionData = NULL;

    HKEY hKey1 = NULL, hKey2 = NULL;

    DebugEvent0("KFW_Logon_Event - Start");

    GetSecurityLogonSessionData( pInfo->hToken, &pLogonSessionData );

    if ( pLogonSessionData ) {
        LogonId = pLogonSessionData->LogonId;
        DebugEvent("KFW_Logon_Event - LogonId(%d,%d)", LogonId.HighPart, LogonId.LowPart);

        sprintf(szLogonId,"kfwlogon-%d.%d",LogonId.HighPart, LogonId.LowPart);
        LsaFreeReturnBuffer( pLogonSessionData );
    } else {
        DebugEvent0("KFW_Logon_Event - Unable to determine LogonId");
        return;
    }

    count = GetEnvironmentVariable("TEMP", filename, sizeof(filename));
    if ( count > sizeof(filename) || count == 0 ) {
        GetWindowsDirectory(filename, sizeof(filename));
    }

    if ( strlen(filename) + strlen(szLogonId) + 2 > sizeof(filename) ) {
        DebugEvent0("KFW_Logon_Event - filename too long");
	return;
    }

    strcat(filename, "\\");
    strcat(filename, szLogonId);    

    hf = CreateFile(filename, FILE_ALL_ACCESS, 0, NULL, OPEN_EXISTING, 
		    FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf == INVALID_HANDLE_VALUE) {
        DebugEvent0("KFW_Logon_Event - file cannot be opened");
	return;
    }
    CloseHandle(hf);

    if (KFW_set_ccache_dacl(filename, pInfo->hToken)) {
        DebugEvent0("KFW_Logon_Event - unable to set dacl");
	DeleteFile(filename);
	return;
    }

    if (KFW_obtain_user_temp_directory(pInfo->hToken, newfilename, sizeof(newfilename))) {
        DebugEvent0("KFW_Logon_Event - unable to obtain temp directory");
	return;
    }

    if ( strlen(newfilename) + strlen(szLogonId) + 2 > sizeof(newfilename) ) {
        DebugEvent0("KFW_Logon_Event - new filename too long");
	return;
    }

    strcat(newfilename, "\\");
    strcat(newfilename, szLogonId);    

    if (!MoveFileEx(filename, newfilename, 
		     MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
        DebugEvent("KFW_Logon_Event - MoveFileEx failed GLE = 0x%x", GetLastError());
	return;
    }

    sprintf(commandline, "kfwcpcc.exe \"%s\"", newfilename);

    GetStartupInfo(&startupinfo);
    if (CreateProcessAsUser( pInfo->hToken,
                             "kfwcpcc.exe",
                             commandline,
                             NULL,
                             NULL,
                             FALSE,
                             CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS,
                             NULL,
                             NULL,
                             &startupinfo,
                             &procinfo)) 
    {
	DebugEvent("KFW_Logon_Event - CommandLine %s", commandline);

	WaitForSingleObject(procinfo.hProcess, 30000);

	CloseHandle(procinfo.hThread);
	CloseHandle(procinfo.hProcess);
    } else {
	DebugEvent0("KFW_Logon_Event - CreateProcessFailed");
    }

    DeleteFile(filename);

    DebugEvent0("KFW_Logon_Event - End");
}

