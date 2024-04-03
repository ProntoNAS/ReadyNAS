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
#include "classIndex.h"
#include "keysilver2.xpm"

#ifdef QT_V4LAYOUT
classIndex::classIndex( Q3ListView *pParent, classCanvas *pCanvas, SQLHDBC hDbc, char *pszTable, char *pszIndex, char *pszDesc )
#else
classIndex::classIndex( QListView *pParent, classCanvas *pCanvas, SQLHDBC hDbc, char *pszTable, char *pszIndex, char *pszDesc )
#endif
    : classNode( pParent, pCanvas )
{
	Init( hDbc, pszTable, pszIndex, pszDesc );
}

#ifdef QT_V4LAYOUT
classIndex::classIndex( Q3ListViewItem *pParent, classCanvas *pCanvas, SQLHENV hDbc, char *pszTable, char *pszIndex, char *pszDesc )
#else
classIndex::classIndex( QListViewItem *pParent, classCanvas *pCanvas, SQLHENV hDbc, char *pszTable, char *pszIndex, char *pszDesc )
#endif
    : classNode( pParent, pCanvas )
{
	Init( hDbc, pszTable, pszIndex, pszDesc );
}

#ifdef QT_V4LAYOUT
classIndex::classIndex( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHENV hDbc, char *pszTable, char *pszIndex, char *pszDesc )
#else
classIndex::classIndex( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHENV hDbc, char *pszTable, char *pszIndex, char *pszDesc )
#endif
    : classNode( pParent, pAfter, pCanvas )
{
	Init( hDbc, pszTable, pszIndex, pszDesc );
}

classIndex::~classIndex()
{
	listColumns.clear();
}

void classIndex::Init( SQLHDBC hDbc, char *pszTable, char *pszIndex, char *pszDesc )
{
	this->hDbc		= hDbc;
	this->qsTable	= pszTable;
	this->qsIndex	= pszIndex;
	setText( 0, qsIndex );
	setText( 1, "" );
	setText( 2, pszDesc );
	sortChildItems( 1, FALSE );
	listColumns.setAutoDelete( TRUE );
	setPixmap( 0, QPixmap( keysilver2_xpm ) );
}

void classIndex::setOpen( bool o )
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

void classIndex::setup()
{
    setExpandable( TRUE );
#ifdef QT_V4LAYOUT
    Q3ListViewItem::setup();
#else
    QListViewItem::setup();
#endif
}

#ifdef QT_V4LAYOUT
void classIndex::selectionChanged( Q3ListViewItem *p )
#else
void classIndex::selectionChanged( QListViewItem *p )
#endif
{
	if ( p == this )
	{
	}
}

void classIndex::LoadColumns()
{
	SQLHSTMT        hstmt;
	SQLRETURN		nReturn             	= -1;
	SQLCHAR         szTableName[101]		= "";
	SQLCHAR         szIndexName[101]		= "";
	SQLCHAR         szColumnName[101]		= "";
	QString         qsError;
	SQLLEN  		nIndicator				= 0;

	if ( qsTable == "" || qsIndex == "" )
		return;

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
	if ( SQL_SUCCESS != (nReturn=SQLStatistics( hstmt, 0, 0, 0, 0, szTableName, SQL_NTS, 0, 0 )) )
	{
		QMessageBox::warning( pCanvas, "Data Manager",  "Failed to SQLStatistics" );
		return;
	}

	// GET RESULTS
	nReturn = SQLFetch( hstmt );
	while ( nReturn == SQL_SUCCESS || nReturn == SQL_SUCCESS_WITH_INFO )
	{
		nReturn = SQLGetData( hstmt, 6, SQL_C_CHAR, &szIndexName[0], sizeof(szIndexName), &nIndicator );
		if ( nReturn != SQL_SUCCESS || nIndicator == SQL_NULL_DATA )
			strcpy( (char *)szIndexName, "Unknown" );

		nReturn = SQLGetData( hstmt, 9, SQL_C_CHAR, &szColumnName[0], sizeof(szColumnName), &nIndicator );
		if ( nReturn != SQL_SUCCESS || nIndicator == SQL_NULL_DATA )
			strcpy( (char *)szColumnName, "Unknown" );

#ifdef QT_V4LAYOUT
		if ( strcmp( (const char*)szIndexName,  qsIndex.ascii() ) == 0 )
#else
		if ( strcmp( (const char*)szIndexName,  qsIndex.data() ) == 0 )
#endif
		{
			listColumns.append( new classColumn( this, pCanvas, hDbc, (char *)szColumnName ) );
		}

		nReturn = SQLFetch( hstmt );
	}

	// FREE STATEMENT
	nReturn = SQLFreeStmt( hstmt, SQL_DROP );
	if ( nReturn != SQL_SUCCESS )
		QMessageBox::warning( pCanvas, "Data Manager",  "Failed to SQLFreeStmt" );

}


