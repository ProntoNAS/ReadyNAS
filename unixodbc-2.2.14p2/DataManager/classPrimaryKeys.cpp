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
#include "classPrimaryKeys.h"

#include "keyred.xpm"

#ifdef QT_V4LAYOUT
classPrimaryKeys::classPrimaryKeys( Q3ListView *pParent, classCanvas *pCanvas, SQLHDBC hDbc, char *pszTable )
#else
classPrimaryKeys::classPrimaryKeys( QListView *pParent, classCanvas *pCanvas, SQLHDBC hDbc, char *pszTable )
#endif
    : classNode( pParent, pCanvas )
{
	Init( hDbc, pszTable );
}

#ifdef QT_V4LAYOUT
classPrimaryKeys::classPrimaryKeys( Q3ListViewItem *pParent, classCanvas *pCanvas, SQLHENV hDbc, char *pszTable )
#else
classPrimaryKeys::classPrimaryKeys( QListViewItem *pParent, classCanvas *pCanvas, SQLHENV hDbc, char *pszTable )
#endif
    : classNode( pParent, pCanvas )
{
	Init( hDbc, pszTable );
}

#ifdef QT_V4LAYOUT
classPrimaryKeys::classPrimaryKeys( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHENV hDbc, char *pszTable )
#else
classPrimaryKeys::classPrimaryKeys( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHENV hDbc, char *pszTable )
#endif
    : classNode( pParent, pAfter, pCanvas )
{
	Init( hDbc, pszTable );
}

classPrimaryKeys::~classPrimaryKeys()
{
}

void classPrimaryKeys::Init( SQLHDBC hDbc, char *pszTable )
{
	this->hDbc		= hDbc;
	this->qsTable	= pszTable;
	setText( 0, "PrimaryKeys" );
	setText( 1, "" );
	setText( 2, "" );
	this->setPixmap( 0, QPixmap( keyred_xpm ) );
}

void classPrimaryKeys::setOpen( bool o )
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

void classPrimaryKeys::setup()
{
    setExpandable( TRUE );
#ifdef QT_V4LAYOUT
    Q3ListViewItem::setup();
#else
    QListViewItem::setup();
#endif
}

void classPrimaryKeys::LoadColumns()
{
}

#ifdef QT_V4LAYOUT
void classPrimaryKeys::selectionChanged( Q3ListViewItem * )
#else
void classPrimaryKeys::selectionChanged( QListViewItem * )
#endif
{
}

