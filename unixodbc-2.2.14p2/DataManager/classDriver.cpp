/**************************************************
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 18.FEB.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 **************************************************/
#include "classDriver.h"
#include "driver.xpm"

#ifdef QT_V4LAYOUT
classDriver::classDriver( Q3ListView *pParent, classCanvas *pCanvas, char *pszDriverName, SQLHENV	hEnv  )
#else
classDriver::classDriver( QListView *pParent, classCanvas *pCanvas, char *pszDriverName, SQLHENV	hEnv  )
#endif
    : classNode( pParent, pCanvas )
{
	Init( pszDriverName, hEnv );
}

#ifdef QT_V4LAYOUT
classDriver::classDriver( Q3ListViewItem *pParent, classCanvas *pCanvas, char *pszDriverName, SQLHENV	hEnv )
#else
classDriver::classDriver( QListViewItem *pParent, classCanvas *pCanvas, char *pszDriverName, SQLHENV	hEnv )
#endif
    : classNode( pParent, pCanvas )
{
	Init( pszDriverName, hEnv );
}

#ifdef QT_V4LAYOUT
classDriver::classDriver( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, char *pszDriverName, SQLHENV	hEnv )
#else
classDriver::classDriver( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, char *pszDriverName, SQLHENV	hEnv )
#endif
    : classNode( pParent, pAfter, pCanvas )
{
	Init( pszDriverName, hEnv );
}

classDriver::~classDriver()
{
}

/*******************************************************
 * Init
 *******************************************************
 * Call the odbcinst to get Driver description.
 *
 * NOTE: 	This should be done via the Driver Manager by
 *			calling SQLDrivers() but the iODBC Driver Manager
 *			does not, at this time, adequately support this call.
 *******************************************************/
void classDriver::Init( char *pszDriverName, SQLHENV hEnv )
{
	char 				szResults[1024];
	char				szPropertyValue[INI_MAX_PROPERTY_VALUE+1];

	qsDriverName = "";
	if ( pszDriverName != 0 )
		qsDriverName = pszDriverName;

	this->pCanvas 	= pCanvas;
	this->hEnv 		= hEnv;
	setPixmap( 0, QPixmap( driver_xpm ) );

	szResults[0] 		= '\0';
    szPropertyValue[0]	= '\0';
	if ( SQLGetPrivateProfileString((char*) qsDriverName.ascii(), "Description", "", szResults, sizeof(szResults), "odbcinst" ) > 0 )
		iniElement( szResults, '\0', '\0', 0, szPropertyValue, INI_MAX_PROPERTY_VALUE );
	else
	{
		//SQLInstallerError( 1, &nError, szError, FILENAME_MAX, 0 );
		//if ( cVerbose == 0 ) printf( "[odbcinst] SQLGetPrivateProfileString failed with %s.\n", szError );
		//return;
	}

	setText( 0, qsDriverName );
	setText( 1, "" );
	setText( 2, szPropertyValue );
}

void classDriver::setOpen( bool o )
{
    if ( o && !childCount() )
	{
		setExpandable( FALSE );
    }
#ifdef QT_V4LAYOUT
    Q3ListViewItem::setOpen( o );
#else
    QListViewItem::setOpen( o );
#endif
}

void classDriver::setup()
{
    setExpandable( TRUE );
#ifdef QT_V4LAYOUT
    Q3ListViewItem::setup();
#else
    QListViewItem::setup();
#endif
}


#ifdef QT_V4LAYOUT
void classDriver::selectionChanged( Q3ListViewItem *p )
#else
void classDriver::selectionChanged( QListViewItem *p )
#endif
{
	if ( p == this )
	{
	}
}

