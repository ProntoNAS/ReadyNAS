/**************************************************
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 31.JAN.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 **************************************************/

#include <odbcinst.h>
#include <odbcinstext.h>

#ifdef QT_V4LAYOUT
#include <Qt/qapplication.h>
#include <Qt/qmessagebox.h>
#else
#include <qapp.h>
#include <qmessagebox.h>
#endif

int main( int argc, char **argv )
{
    QApplication::setDesktopSettingsAware( true );             // try to use desktop colors

    QApplication    oApplication( argc, argv );
    int             nReturn;
    ODBCINSTWND     odbcinstwnd;

#ifdef QT_V4LAYOUT
	strcpy( odbcinstwnd.szUI, "odbcinstQ4" );
#else
	strcpy( odbcinstwnd.szUI, "odbcinstQ" );
#endif
    odbcinstwnd.hWnd = qApp->desktop();

    nReturn = SQLManageDataSources( (HWND)(&odbcinstwnd) );
    if ( !nReturn )
    {
        for ( WORD nError = 1; nError < 10; nError++ )
        {
            DWORD   nErrorCode;
            char    szErrorMsg[SQL_MAX_MESSAGE_LENGTH];
            RETCODE nRetCode = SQLInstallerError( nError, &nErrorCode, szErrorMsg, SQL_MAX_MESSAGE_LENGTH, NULL );
            if ( !SQL_SUCCEEDED( nRetCode ) )
            {
                QMessageBox::critical( 0, "ODBCConfig",  "failed: no more errors to report" );
                break;
            }
            QMessageBox::critical( 0, "ODBCConfig",  szErrorMsg );
        }
    }

    return nReturn;
}

