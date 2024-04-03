/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#include <ini.h>

#include <QtGui>

#include "CDriverList.h"

#include "CPropertiesDialog.h"

#define ODBC_HELP_DRIVER_NAME "*Unique* driver name."
#define ODBC_HELP_DRIVER_DESC "Driver description."
#define ODBC_HELP_DRIVER_DRIVER "The driver. Its a share library and the filename will probably have odbc in it and it will probably end with *.so."
#define ODBC_HELP_DRIVER_DRIVER64 "The driver for 64 bit environments. Its a share library and the filename will probably have odbc in it and it will probably end with *.so."
#define ODBC_HELP_DRIVER_SETUP "The setup routines. Its a share library used to provide this program with DSN properties which can be presented to the user to edit. This library filename usually ends with a *S.so."
#define ODBC_HELP_DRIVER_SETUP64 "The setup routines for 64 bit environments. Its a share library used to provide this program with DSN properties which can be presented to the user to edit. This library filename usually ends with a *S.so."
#define ODBC_HELP_DRIVER_USAGECOUNT "The number of installs that use this driver. This driver entry should be removed when < 1."
#define ODBC_HELP_DRIVER_CPTIMEOUT "Number of seconds before a connection timesout when in a Connection Pool. Leave this value blank to disable Connection Pooling. 0-n (0=default)."
#define ODBC_HELP_DRIVER_CPTIMETOLIVE "Max number of seconds a pooled connection may be used. 0-n (0=default)."
#define ODBC_HELP_DRIVER_CPREUSE "The maximum number of times a connection can be reused in a Connection Pool. Set to a lower number when dealing with drivers which have stability issues or memory leaks."
#define ODBC_HELP_DRIVER_CPPROBE "A simple, benign, SQL statement which can be used to verify that the connection is still valid. Text (empty=default)."
#define ODBC_HELP_DRIVER_DGF "Enable/Disable use of SQLGetFunctions by DM. 0-1 (0=default)."
#define ODBC_HELP_DRIVER_DLCLOSE "Enable to tell DM to skip explicit unload request of driver. 0-1 (1=default)."
#define ODBC_HELP_DRIVER_FETCHMAP "Enable/Disable SQLFetch -> SQLExtendedFetch mapping. 0-1 (1=default)."
#define ODBC_HELP_DRIVER_THREADING "Thread level: 0-3 (3=default)."
#define ODBC_HELP_DRIVER_FAKEUNICODE "Fake UNICODE: 0-1 (0=default)."
#define ODBC_HELP_DRIVER_ICONV "Iconv option. Default is set at build time and is usually 'auto-search'."
#define ODBC_HELP_DRIVER_TRACE "Turn trace on/off. 0-1 (0=default)."
#define ODBC_HELP_DRIVER_TRACEFILE "Place to write trace output (/tmp/Trace.txt=default)."
#define ODBC_HELP_DRIVER_TRACELIBRARY "Library to create trace output (odbctrac=default)."
#define ODBC_HELP_DRIVER_UNKNOWN "No help for this driver specific property. Please check with the vendor of the driver... perhaps their web site"

CDriverList::CDriverList( QWidget* pwidgetParent )
    : QTableWidget( pwidgetParent )
{
    setToolTip( tr( "list of registered drivers" ) );
    setWhatsThis( tr( "This is a list of registered drivers. Registered drivers are drivers which the Driver Manager knows about. A well behaved driver installer will automatically register a driver - but not all driver installers do this last step. If you know your driver is installed but is not registered here then it can be registered manually providing you knwo the file names." ) );
    QStringList stringlistHeaderLabels;

    setColumnCount( 4 );
    setHorizontalHeaderLabels( stringlistHeaderLabels << tr( "Name" ) << tr( "Description" ) << tr( "Driver" ) << tr( "Setup" ) );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::SingleSelection );
    verticalHeader()->setVisible( false );

    connect( this, SIGNAL(itemDoubleClicked( QTableWidgetItem * )), this, SLOT(slotDoubleClick( QTableWidgetItem * )));

    slotLoad();
}

CDriverList::~CDriverList()
{
}

QString CDriverList::getFriendlyName()
{
    QList<QTableWidgetItem*> listSelectedItems = selectedItems();
    if ( !listSelectedItems.count() )
        return QString::null;

    return item( row( listSelectedItems.at( 0 ) ), 0 )->text();
}

QString CDriverList::getDescription()
{
    QList<QTableWidgetItem*> listSelectedItems = selectedItems();
    if ( !listSelectedItems.count() )
        return QString::null;

    return item( row( listSelectedItems.at( 0 ) ), 1 )->text();
}

QString CDriverList::getDriver()
{
    QList<QTableWidgetItem*> listSelectedItems = selectedItems();
    if ( !listSelectedItems.count() )
        return QString::null;

    return item( row( listSelectedItems.at( 0 ) ), 2 )->text();
}

QString CDriverList::getSetup()
{
    QList<QTableWidgetItem*> listSelectedItems = selectedItems();
    if ( !listSelectedItems.count() )
        return QString::null;

    return item( row( listSelectedItems.at( 0 ) ), 3 )->text();
}

void CDriverList::slotAdd()
{
    HODBCINSTPROPERTY   hFirstProperty      = NULL;
    HODBCINSTPROPERTY   hCurProperty        = NULL;
    HODBCINSTPROPERTY   hLastProperty;

    // SET UP PROPERTIES LIST
    hFirstProperty                  = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    memset( hFirstProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hFirstProperty->nPromptType     = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hFirstProperty->pNext           = NULL;
    hFirstProperty->bRefresh        = 0;
    hFirstProperty->hDLL            = NULL;
    hFirstProperty->pWidget         = NULL;
    hFirstProperty->pszHelp         = strdup( ODBC_HELP_DRIVER_NAME );
    hFirstProperty->aPromptData     = NULL;
    strncpy( hFirstProperty->szName, "Name", INI_MAX_PROPERTY_NAME );
    strcpy( hFirstProperty->szValue, "" );
    hLastProperty = hFirstProperty;

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_DESC );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "Description", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "" );

    /*
     * Don't have a deflib for 64 bit, the user may not want a driver entry, just a driver64
     */

#ifdef PLATFORM64
#undef DEFLIB_PATH
#endif

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_FILENAME;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_DRIVER );
    strncpy( hLastProperty->szName, "Driver", INI_MAX_PROPERTY_NAME );
#ifdef DEFLIB_PATH
    strncpy( hLastProperty->szValue, DEFLIB_PATH, INI_MAX_PROPERTY_VALUE );
#else
    strncpy( hLastProperty->szValue, "", INI_MAX_PROPERTY_VALUE );
#endif

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_FILENAME;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_DRIVER64 );
    strncpy( hLastProperty->szName, "Driver64", INI_MAX_PROPERTY_NAME );
#ifdef DEFLIB_PATH
    strncpy( hLastProperty->szValue, DEFLIB_PATH, INI_MAX_PROPERTY_VALUE );
#else
    strncpy( hLastProperty->szValue, "", INI_MAX_PROPERTY_VALUE );
#endif

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_FILENAME;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_SETUP );
    strncpy( hLastProperty->szName, "Setup", INI_MAX_PROPERTY_NAME );
#ifdef DEFLIB_PATH
    strncpy( hLastProperty->szValue, DEFLIB_PATH, INI_MAX_PROPERTY_VALUE );
#else
    strncpy( hLastProperty->szValue, "", INI_MAX_PROPERTY_VALUE );
#endif

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_FILENAME;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_SETUP64 );
    strncpy( hLastProperty->szName, "Setup64", INI_MAX_PROPERTY_NAME );
#ifdef DEFLIB_PATH
    strncpy( hLastProperty->szValue, DEFLIB_PATH, INI_MAX_PROPERTY_VALUE );
#else
    strncpy( hLastProperty->szValue, "", INI_MAX_PROPERTY_VALUE );
#endif

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_USAGECOUNT );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "UsageCount", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "1" );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_CPTIMEOUT );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "CPTimeout", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "0" );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_CPTIMETOLIVE );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "CPTimeToLive", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "0" );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_CPPROBE );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "CPProbe", INI_MAX_PROPERTY_NAME );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_DGF );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "DisableGetFunctions", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "0" );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_DLCLOSE );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "DontDLCLose", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "1" );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_FETCHMAP );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "ExFetchMapping", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "1" );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_THREADING );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "Threading", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "3" );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_FAKEUNICODE );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "FakeUnicode", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "0" );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_ICONV );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "IconvEncoding", INI_MAX_PROPERTY_NAME );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_TRACE );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "Trace", INI_MAX_PROPERTY_NAME );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_TRACEFILE );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "TraceFile", INI_MAX_PROPERTY_NAME );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_TRACELIBRARY );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "TraceLibrary", INI_MAX_PROPERTY_NAME );

    // present properties for editing...
    {
        CPropertiesDialog propertiesdialog( this, hFirstProperty );
        propertiesdialog.setWindowTitle( tr( "Driver Properties (new)" ) );
        if ( !propertiesdialog.exec() )
        {
            ODBCINSTDestructProperties( &hFirstProperty );
            return;
        }
    }

    // save changes...
    {
        // get our ini file
        char szINI[FILENAME_MAX+1];
		char b1[ 256 ], b2[ 256 ];

        sprintf( szINI, "%s/%s", odbcinst_system_file_path( b1 ), odbcinst_system_file_name( b1 ) );

        // ensure section does not exist...
        if ( SQLWritePrivateProfileString( hFirstProperty->szValue, NULL, NULL, szINI ) == FALSE )
        {
            ODBCINSTDestructProperties( &hFirstProperty );
            CODBCInst::showErrors( this, QString( "Could not write to (%1)\nYou probably need to do this while logged in as root." ).arg( szINI ) );
            return;
        }

        // create section (first call) and add key/value pairs...
        for ( hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
        {
            SQLWritePrivateProfileString( hFirstProperty->szValue, hCurProperty->szName, hCurProperty->szValue, szINI );
        }

        // free mem used by properties...
        ODBCINSTDestructProperties( &hFirstProperty );

        // announce that we have revised driver information...
        emit signalChanged();
    }

    // reload to pickup any changes (including any we may not know about)...
    slotLoad();
}

void CDriverList::slotEdit()
{
    QString             stringName      = "";
    CPropertiesDialog * pProperties;
    HODBCINSTPROPERTY   hFirstProperty  = NULL;
    HODBCINSTPROPERTY   hCurProperty    = NULL;
    HODBCINSTPROPERTY   hLastProperty;
    HINI	            hIni;
    char                szINI[FILENAME_MAX+1];
	char b1[ 256 ], b2[ 256 ];

	sprintf( szINI, "%s/%s", odbcinst_system_file_path( b1 ), odbcinst_system_file_name( b1 ) );

    QList<QTableWidgetItem*> listSelectedItems = selectedItems();
    if ( listSelectedItems.count() )
        stringName = item( row( listSelectedItems.at( 0 ) ), 0 )->text();
    else
    {
        QMessageBox::warning( this, tr( "ODBC Administrator" ),  "Please select a Driver from the list first." );
        return;
    }

    // SET UP PROPERTIES LIST STARTING WITH MANDATORY PROPERTIES
    hFirstProperty                  = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    memset( hFirstProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hFirstProperty->nPromptType     = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hFirstProperty->pNext           = NULL;
    hFirstProperty->bRefresh        = 0;
    hFirstProperty->hDLL            = NULL;
    hFirstProperty->pWidget         = NULL;
    hFirstProperty->pszHelp         = strdup( ODBC_HELP_DRIVER_NAME );
    hFirstProperty->aPromptData     = NULL;
    strncpy( hFirstProperty->szName, "Name", INI_MAX_PROPERTY_NAME );
    strcpy( hFirstProperty->szValue, stringName.toAscii().data() );
    hLastProperty = hFirstProperty;

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_DESC );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "Description", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "" );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_FILENAME;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_DRIVER );
    strncpy( hLastProperty->szName, "Driver", INI_MAX_PROPERTY_NAME );
    strncpy( hLastProperty->szValue, "", INI_MAX_PROPERTY_VALUE );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_FILENAME;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_DRIVER64 );
    strncpy( hLastProperty->szName, "Driver64", INI_MAX_PROPERTY_NAME );
    strncpy( hLastProperty->szValue, "", INI_MAX_PROPERTY_VALUE );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_FILENAME;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_SETUP );
    strncpy( hLastProperty->szName, "Setup", INI_MAX_PROPERTY_NAME );
    strncpy( hLastProperty->szValue, "", INI_MAX_PROPERTY_VALUE );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_FILENAME;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_SETUP64 );
    strncpy( hLastProperty->szName, "Setup64", INI_MAX_PROPERTY_NAME );
    strncpy( hLastProperty->szValue, "", INI_MAX_PROPERTY_VALUE );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->aPromptData      = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_USAGECOUNT );
    strncpy( hLastProperty->szName, "UsageCount", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "1" );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_CPTIMEOUT );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "CPTimeout", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "0" );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_CPTIMETOLIVE );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "CPTimeToLive", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "0" );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );
/*
    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_CPREUSE );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "CPReuse", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "0" );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );
*/
    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_DGF );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "DisableGetFunctions", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "0" );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_DLCLOSE );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "DontDLCLose", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "1" );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_FETCHMAP );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "ExFetchMapping", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "1" );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_THREADING );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "Threading", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "3" );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_FAKEUNICODE );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "FakeUnicode", INI_MAX_PROPERTY_NAME );
    strcpy( hLastProperty->szValue, "0" );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_ICONV );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "IconvEncoding", INI_MAX_PROPERTY_NAME );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_TRACE );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "Trace", INI_MAX_PROPERTY_NAME );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_TRACEFILE );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "TraceFile", INI_MAX_PROPERTY_NAME );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    hLastProperty->pNext            = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
    hLastProperty                   = hLastProperty->pNext;
    memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
    hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
    hLastProperty->pNext            = NULL;
    hLastProperty->bRefresh         = 0;
    hLastProperty->hDLL             = NULL;
    hLastProperty->pWidget          = NULL;
    hLastProperty->pszHelp          = strdup( ODBC_HELP_DRIVER_TRACELIBRARY );
    hLastProperty->aPromptData      = NULL;
    strncpy( hLastProperty->szName, "TraceLibrary", INI_MAX_PROPERTY_NAME );
    SQLGetPrivateProfileString( stringName.toAscii().data(), hLastProperty->szName, "", hLastProperty->szValue, sizeof(hLastProperty->szValue)-1, szINI );

    // ACCEPT ANY DRIVER SPECIFIC PROPERTIES THAT HAVE BEEN TYPED IN MANUALLY OR INSTALLED 
    // NOTE: This is a quick enhancement. Much room to optimize this func.
    {
        char  szDriverName[INI_MAX_OBJECT_NAME+1];
        char  szPropertyName[INI_MAX_PROPERTY_NAME+1];
        char  szPropertyNameUpper[INI_MAX_PROPERTY_NAME+1];
        char  szPropertyValue[INI_MAX_PROPERTY_VALUE+1];

        strcpy( szDriverName, stringName.toAscii().data() );
        if ( iniOpen( &hIni, szINI, (char*)"#;", '[', ']', '=', TRUE ) != INI_ERROR )
        {
            if ( iniObjectSeek( hIni, szDriverName ) == INI_SUCCESS )
            {
                while ( iniPropertyEOL( hIni ) == FALSE )
                {
                    iniProperty( hIni, szPropertyName );
                    iniProperty( hIni, szPropertyNameUpper );
                    iniToUpper( szPropertyNameUpper );

                    if ( strncmp( szPropertyNameUpper, "NAME", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "DESCRIPTION", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "DRIVER", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "DRIVER64", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "SETUP", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "SETUP64", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "USAGECOUNT", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "CPTIMEOUT", INI_MAX_PROPERTY_NAME ) != 0 &&
/*                         strncmp( szPropertyNameUpper, "CPREUSE", INI_MAX_PROPERTY_NAME ) != 0 && */
                         strncmp( szPropertyNameUpper, "CPTIMETOLIVE", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "CPPROBE", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "DISABLEGETFUNCTIONS", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "DONTDLCLOSE", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "EXFETCHMAPPING", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "THREADING", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "FAKEUNICODE", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "ICONVENCODING", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "TRACE", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "TRACEFILE", INI_MAX_PROPERTY_NAME ) != 0 &&
                         strncmp( szPropertyNameUpper, "TRACELIBRARY", INI_MAX_PROPERTY_NAME ) != 0 )
                    {
                        iniValue( hIni, szPropertyValue );

                        hLastProperty->pNext        = (HODBCINSTPROPERTY)malloc( sizeof(ODBCINSTPROPERTY) );
                        hLastProperty             = hLastProperty->pNext;
                        memset( hLastProperty, 0, sizeof(ODBCINSTPROPERTY) );
                        hLastProperty->nPromptType      = ODBCINST_PROMPTTYPE_TEXTEDIT;
                        hLastProperty->pNext        = NULL;
                        hLastProperty->bRefresh       = 0;
                        hLastProperty->hDLL         = NULL;
                        hLastProperty->pWidget        = NULL;
                        hLastProperty->pszHelp        = strdup( ODBC_HELP_DRIVER_UNKNOWN );
                        hLastProperty->aPromptData      = NULL;
                        strncpy( hLastProperty->szName, szPropertyName, INI_MAX_PROPERTY_NAME );
                        strncpy( hLastProperty->szValue, szPropertyValue, sizeof(hLastProperty->szValue)-1 );
                    }
                    iniPropertyNext( hIni );
                }
            }
            iniClose( hIni );
        }
    }

    // ALLOW USER TO EDIT
    pProperties = new CPropertiesDialog( this, hFirstProperty );
    pProperties->setWindowTitle( tr( "Driver Properties (edit)" ) );
    if ( pProperties->exec() )
    {
        /* DELETE ENTIRE SECTION IF IT EXISTS (no entry given) */
        if ( SQLWritePrivateProfileString( stringName.toAscii().data(), NULL, NULL, szINI ) == FALSE )
        {
            delete pProperties;
            ODBCINSTDestructProperties( &hFirstProperty );
            CODBCInst::showErrors( this, QString( "Could not write to (%1).\nYou probably need to do this while logged in as root." ).arg( szINI ) );
            return;
        }

        /* ADD ENTRIES; SECTION CREATED ON FIRST CALL */
        for ( hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
        {
            SQLWritePrivateProfileString( hFirstProperty->szValue, hCurProperty->szName, hCurProperty->szValue, szINI );
        }

        emit signalChanged();
    }
    delete pProperties;
    ODBCINSTDestructProperties( &hFirstProperty );

    // RELOAD (slow but safe)
    slotLoad();
}

void CDriverList::slotDelete()
{
    HINI        hIni;
    char        szINI[FILENAME_MAX+1];
    QString     stringName;
	char b1[ 256 ], b2[ 256 ];

	sprintf( szINI, "%s/%s", odbcinst_system_file_path( b1 ), odbcinst_system_file_name( b1 ) );

    // GET SELECT DATA SOURCE NAME
    QList<QTableWidgetItem*> listSelectedItems = selectedItems();
    if ( listSelectedItems.count() )
        stringName = item( row( listSelectedItems.at( 0 ) ), 0 )->text();
    else
    {
        QMessageBox::warning( this, tr( "ODBC Administrator" ),  "Please select a Driver from the list first" );
        return;
    }

    // DELETE ENTIRE SECTION IF IT EXISTS (given NULL entry)
    if ( SQLWritePrivateProfileString( stringName.toAscii().data(), NULL, NULL, szINI ) == FALSE )
        CODBCInst::showErrors( this, QString( "Could not write property list for (%1)" ).arg( stringName ) );

    emit signalChanged();

    // RELOAD (slow but safe)
    slotLoad();
}

void CDriverList::slotLoad()
{
    HINI	hIni;
    char    szINI[FILENAME_MAX+1];
    char    szDriverName[INI_MAX_OBJECT_NAME+1];
    char    szPropertyName[INI_MAX_PROPERTY_NAME+1];
    char    szDescription[INI_MAX_PROPERTY_VALUE+1];
    char    szDriver[INI_MAX_PROPERTY_VALUE+1];
    char    szDriver64[INI_MAX_PROPERTY_VALUE+1];
    char    szSetup[INI_MAX_PROPERTY_VALUE+1];
    char    szSetup64[INI_MAX_PROPERTY_VALUE+1];
    int     nRow = 0;
	char b1[ 256 ], b2[ 256 ];

    setRowCount( 0 );

	sprintf( szINI, "%s/%s", odbcinst_system_file_path( b1 ), odbcinst_system_file_name( b1 ) );

    if ( iniOpen( &hIni, szINI, (char*)"#;", '[', ']', '=', TRUE ) != INI_ERROR )
    {
        iniObjectFirst( hIni );
        while ( iniObjectEOL( hIni ) == FALSE )
        {
            *szDriverName     = '\0';
            *szDescription    = '\0';
            *szDriver         = '\0';
            *szDriver64       = '\0';
            *szSetup64        = '\0';
            *szSetup          = '\0';

            iniObject( hIni, szDriverName );
            iniPropertyFirst( hIni );
            if ( strcmp( szDriverName, "ODBC" ) == 0 )
            {
                iniObjectNext( hIni );
                continue;
            }

            while ( iniPropertyEOL( hIni ) == FALSE )
            {
                iniProperty( hIni, szPropertyName );
                iniToUpper( szPropertyName );

                if ( strncmp( szPropertyName, "DESCRIPTION", INI_MAX_PROPERTY_NAME ) == 0 )
                    iniValue( hIni, szDescription );
#ifdef PLATFORM64
                if ( strncmp( szPropertyName, "DRIVER64", INI_MAX_PROPERTY_NAME ) == 0 )
                    iniValue( hIni, szDriver64 );
                if ( strncmp( szPropertyName, "DRIVER", INI_MAX_PROPERTY_NAME ) == 0 )
                    iniValue( hIni, szDriver );

                if ( strncmp( szPropertyName, "SETUP64", INI_MAX_PROPERTY_NAME ) == 0 )
                    iniValue( hIni, szSetup64 );
                if ( strncmp( szPropertyName, "SETUP", INI_MAX_PROPERTY_NAME ) == 0 )
                    iniValue( hIni, szSetup );
#else
                if ( strncmp( szPropertyName, "DRIVER", INI_MAX_PROPERTY_NAME ) == 0 )
                    iniValue( hIni, szDriver );

                if ( strncmp( szPropertyName, "SETUP", INI_MAX_PROPERTY_NAME ) == 0 )
                    iniValue( hIni, szSetup );
#endif

                iniPropertyNext( hIni );
            }

#ifdef PLATFORM64
            if ( szDriver64[ 0 ] != '\0' )
                strcpy( szDriver, szDriver64 );
            if ( szSetup64[ 0 ] != '\0' )
                strcpy( szSetup, szSetup64 );
#endif
            QTableWidgetItem *ptablewidgetitem;
            setRowCount( nRow + 1 );

            ptablewidgetitem = new QTableWidgetItem( szDriverName );
            ptablewidgetitem->setFlags( Qt::ItemIsSelectable );
            setItem( nRow, 0, ptablewidgetitem );

            ptablewidgetitem = new QTableWidgetItem( szDescription );
            ptablewidgetitem->setFlags( Qt::ItemIsSelectable );
            setItem( nRow, 1, ptablewidgetitem );

            ptablewidgetitem = new QTableWidgetItem( szDriver );
            ptablewidgetitem->setFlags( Qt::ItemIsSelectable );
            setItem( nRow, 2, ptablewidgetitem );

            ptablewidgetitem = new QTableWidgetItem( szSetup );
            ptablewidgetitem->setFlags( Qt::ItemIsSelectable );
            setItem( nRow, 3, ptablewidgetitem );

            nRow++;
            iniObjectNext( hIni );
        }
        iniClose( hIni );
    }
    else
        CODBCInst::showErrors( this, QString( "Could not open system file at %1" ).arg( szINI ) );
}

void CDriverList::slotDoubleClick( QTableWidgetItem * )
{
    slotEdit();
}

