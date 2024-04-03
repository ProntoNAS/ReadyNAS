/**************************************************
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 18.FEB.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey         - pharvey@codebydesign.com
 **************************************************/

#include "classDataSource.h"
#include "classLogin.h"
#ifdef QT_V4LAYOUT
#include "Qt/qpixmap.h"
#else
#include "qpixmap.h"
#endif
#include <ini.h>
#include <odbcinst.h>
#include "computergreen.xpm"
#include "computerred.xpm"

#ifdef QT_V4LAYOUT
classDataSource::classDataSource( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, classODBC::DSType dataSourceType, const char *pszDataSourceName )
#else
classDataSource::classDataSource( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, classODBC::DSType dataSourceType, const char *pszDataSourceName )
#endif
    : classNode( pParent, pAfter, pCanvas ), env(), dbc( env(), "classDataSource" ), pConnectionFrame( NULL )
{
  listLibraries.setAutoDelete( TRUE );
  char szResults[9600];
  char szDescription[INI_MAX_PROPERTY_VALUE+1];
  char szDriver[INI_MAX_PROPERTY_VALUE+1];
  QString qsDescription;

  if ( pszDataSourceName != 0 )
    qsDataSourceName = pszDataSourceName;

  setPixmap( 0, QPixmap( computerred_xpm ) );
  setExpandable( TRUE );

  szResults[0]     = '\0';
  szDescription[0] = '\0';
  szDriver[0]      = '\0';
  if ( dataSourceType == classODBC::User )
    SQLSetConfigMode( ODBC_USER_DSN );
  else
    SQLSetConfigMode( ODBC_SYSTEM_DSN );

  if ( SQLGetPrivateProfileString((char*) qsDataSourceName.ascii(), "Description", "", szResults, sizeof(szResults), 0 ) > 0 )
    iniElement( szResults, '\0', '\0', 0, szDescription, INI_MAX_PROPERTY_VALUE );
  if ( SQLGetPrivateProfileString((char*) qsDataSourceName.ascii(), "Driver", "", szResults, sizeof(szResults), 0 ) > 0 )
    iniElement( szResults, '\0', '\0', 0, szDriver, INI_MAX_PROPERTY_VALUE );

  SQLSetConfigMode( ODBC_BOTH_DSN );

  qsDescription.sprintf( "%s [%s]", szDescription, szDriver );
  setText( 0, qsDataSourceName );
  setText( 1, "DATASOURCE  " );
  setText( 2, qsDescription );
}

classDataSource::~classDataSource()
{
  delete pConnectionFrame ;
  pConnectionFrame = NULL ;
}

void classDataSource::setOpen( bool bOpen )
{
  if ( bOpen && !dbc.active() ) // Always refresh
  {
    classLogin login( pCanvas, dbc, qsDataSourceName );
    if ( login.exec() )
    {
      setPixmap( 0, QPixmap( computergreen_xpm ) );
      LoadLibraries();
      if ( !pConnectionFrame )
        pConnectionFrame = new classConnectionFrame( dbc(), qsDataSourceName, pCanvas );
      pConnectionFrame->show();
    }
  }
  else if ( !bOpen && dbc.active() )
  {
    listLibraries.clear();
    delete pConnectionFrame;
    pConnectionFrame = NULL;
    // disconnect
    dbc.disconnect() ;
    setPixmap( 0, QPixmap( computerred_xpm ) );
  }
#ifdef QT_V4LAYOUT
  Q3ListViewItem::setOpen( bOpen );
#else
  QListViewItem::setOpen( bOpen );
#endif
}

void classDataSource::LoadLibraries()
{
  CursorScoper s(listView()) ;
  SQLRETURN    nReturn ;
  SQLCHAR      szLibraryName[MAX_COLUMN_WIDTH];
  SQLLEN       nIndicatorName;
  classTables  *pLibrary = NULL;

  // CREATE A STATEMENT
  StatementScoper stmt( dbc() ) ; if ( !stmt() ) return ;

  // EXECUTE OUR SQL/CALL
  if (!SQL_SUCCEEDED(nReturn=SQLTables( stmt(), (SQLCHAR*)"", SQL_NTS, (SQLCHAR*)"%", SQL_NTS, (SQLCHAR*)"", SQL_NTS, 0, 0 ) ) )
    return my_msgBox( "classDataSource", "SQLTables", nReturn, NULL, NULL, stmt() ) ;

  SQLBindCol( stmt(), SQLTables_TABLE_SCHEM, SQL_C_CHAR, szLibraryName, sizeof(szLibraryName), &nIndicatorName );

  // GET RESULTS
  
  int first = 1;
  SQLCHAR last_name[ MAX_COLUMN_WIDTH ];
  
  while ( SQL_SUCCEEDED(SQLFetch( stmt() ) ) )
  {
    if ( nIndicatorName != SQL_NULL_DATA )
    {
        // remove duplicate rows with driver such as Postgres
        if ( first )
        {
            first = 0;
            strcpy((char *) last_name,(char *) szLibraryName );
        }
        else
        {
            if ( strcmp((char *) last_name,(char *) szLibraryName ) == 0 )
            {
                continue;
            }
            else
            {
                strcpy((char *) last_name,(char *) szLibraryName );
            }
        }

        listLibraries.append( pLibrary = new classTables( this, pLibrary, pCanvas, dbc(), QString((char*)szLibraryName).stripWhiteSpace() ) );
    }
  }
}

#ifdef QT_V4LAYOUT
void classDataSource::selectionChanged( Q3ListViewItem *p )
#else
void classDataSource::selectionChanged( QListViewItem *p )
#endif
{

  for ( classTables *pLibrary = listLibraries.first(); pLibrary != 0; pLibrary = listLibraries.next() )
    pLibrary->selectionChanged( p );

  if ( dbc.active() )
  {
    if ( p == this || p->parent() == this ) // Allow any library also
      pConnectionFrame->show();
    else
      pConnectionFrame->hide();
  }
}

