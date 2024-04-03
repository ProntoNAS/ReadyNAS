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
#include "classSpecialColumns.h"

#include "id.xpm"

#ifdef QT_V4LAYOUT
classSpecialColumns::classSpecialColumns( Q3ListView *pParent, classCanvas *pCanvas, SQLHDBC hDbc, char *pszTable )
#else
classSpecialColumns::classSpecialColumns( QListView *pParent, classCanvas *pCanvas, SQLHDBC hDbc, char *pszTable )
#endif
    : classNode( pParent, pCanvas )
{
	Init( hDbc, pszTable );
}

#ifdef QT_V4LAYOUT
classSpecialColumns::classSpecialColumns( Q3ListViewItem *pParent, classCanvas *pCanvas, SQLHENV hDbc, char *pszTable )
#else
classSpecialColumns::classSpecialColumns( QListViewItem *pParent, classCanvas *pCanvas, SQLHENV hDbc, char *pszTable )
#endif
    : classNode( pParent, pCanvas )
{
	Init( hDbc, pszTable );
}

#ifdef QT_V4LAYOUT
classSpecialColumns::classSpecialColumns( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHENV hDbc, char *pszTable )
#else
classSpecialColumns::classSpecialColumns( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHENV hDbc, char *pszTable )
#endif
    : classNode( pParent, pAfter, pCanvas )
{
	Init( hDbc, pszTable );
}

classSpecialColumns::~classSpecialColumns()
{
	listColumns.clear();
}

void classSpecialColumns::Init( SQLHDBC hDbc, char *pszTable )
{
	this->hDbc		= hDbc;
	this->qsTable 	= pszTable;
	setText( 0, "SpecialColumns" );
	setText( 1, "" );
	setText( 2, "" );
	sortChildItems( 1, FALSE );
	listColumns.setAutoDelete( TRUE );
	this->setPixmap( 0, QPixmap( id_xpm ) );
}

void classSpecialColumns::setOpen( bool o )
{
    if ( o && !childCount() )
	{
		LoadColumns();
    }
#ifdef QT_V4LAYOUT
    Q3ListViewItem::setOpen( o );
#else
    QListViewItem::setOpen( o );
#endif
}

void classSpecialColumns::setup()
{
    setExpandable( TRUE );
#ifdef QT_V4LAYOUT
    Q3ListViewItem::setup();
#else
    QListViewItem::setup();
#endif
}

void classSpecialColumns::LoadColumns()
{
	SQLHSTMT        hstmt;
	SQLRETURN		nReturn             = -1;
	SQLCHAR         szTableName[101]	= "";
	SQLCHAR         szColumnName[101]	= "";
	QString         qsError;

	// CREATE A STATEMENT
	nReturn = SQLAllocStmt( hDbc, &hstmt );
	if ( nReturn != SQL_SUCCESS )
	{
		QMessageBox::warning( pCanvas, "Data Manager",  "Failed to SQLAllocStmt" );
		return;
	}

	// EXECUTE OUR SQL/CALL
#ifdef QT_V4LAYOUT
	strcpy( (char *)szTableName, qsTable.ascii() );
#else
	strcpy( (char *)szTableName, qsTable.data() );
#endif

	if ( SQL_SUCCESS != (nReturn=SQLSpecialColumns( hstmt, SQL_BEST_ROWID, 0, 0, 0, 0, szTableName, SQL_NTS, SQL_SCOPE_SESSION, SQL_NULLABLE )) )
	{
		QMessageBox::warning( pCanvas, "Data Manager",  "Failed to SQLSpecialColumns" );
		return;
	}

	// GET RESULTS
	nReturn = SQLFetch( hstmt );
	while ( nReturn == SQL_SUCCESS || nReturn == SQL_SUCCESS_WITH_INFO )
	{
		nReturn = SQLGetData( hstmt, 2, SQL_C_CHAR, &szColumnName[0], sizeof(szColumnName), 0 );
		if ( nReturn != SQL_SUCCESS )
			strcpy( (char *)szColumnName, "Unknown" );

		listColumns.append( new classColumn( this, pCanvas, hDbc, (char *)szColumnName ) );

		nReturn = SQLFetch( hstmt );
	}

	// FREE STATEMENT
	nReturn = SQLFreeStmt( hstmt, SQL_DROP );
	if ( nReturn != SQL_SUCCESS )
		QMessageBox::warning( pCanvas, "Data Manager",  "Failed to SQLFreeStmt" );

}

#ifdef QT_V4LAYOUT
void classSpecialColumns::selectionChanged( Q3ListViewItem *p )
#else
void classSpecialColumns::selectionChanged( QListViewItem *p )
#endif
{
	classColumn	*pColumn;

	for ( pColumn = listColumns.first(); pColumn != 0; pColumn = listColumns.next() )
        pColumn->selectionChanged( p );

	if ( p == this )
	{
	}
}

