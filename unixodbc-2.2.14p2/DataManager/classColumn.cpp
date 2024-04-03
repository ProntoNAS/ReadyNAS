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
#include "classColumn.h"
#include "column.xpm"


#ifdef QT_V4LAYOUT
classColumn::classColumn( Q3ListView *pParent, classCanvas *pCanvas, SQLHDBC hDbc, char *pszName, char *pszType, char *pszDesc )
#else
classColumn::classColumn( QListView *pParent, classCanvas *pCanvas, SQLHDBC hDbc, char *pszName, char *pszType, char *pszDesc )
#endif
    : classNode( pParent, pCanvas )
{
	Init( hDbc, pszName, pszType, pszDesc );
}

#ifdef QT_V4LAYOUT
classColumn::classColumn( Q3ListViewItem *pParent, classCanvas *pCanvas, SQLHENV hDbc, char *pszName, char *pszType, char *pszDesc )
#else
classColumn::classColumn( QListViewItem *pParent, classCanvas *pCanvas, SQLHENV hDbc, char *pszName, char *pszType, char *pszDesc )
#endif
    : classNode( pParent, pCanvas )
{
	Init( hDbc, pszName, pszType, pszDesc );
}

#ifdef QT_V4LAYOUT
classColumn::classColumn( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHENV hDbc, char *pszName, char *pszType, char *pszDesc )
#else
classColumn::classColumn( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHENV hDbc, char *pszName, char *pszType, char *pszDesc )
#endif
    : classNode( pParent, pAfter, pCanvas )
{
	Init( hDbc, pszName, pszType, pszDesc );
}

classColumn::~classColumn()
{
}

void classColumn::Init( SQLHDBC hDbc, char *pszName, char *pszType, char *pszDesc )
{
	this->hDbc		= hDbc;
	this->qsName	= pszName;
	setText( 0, qsName );
	setText( 1, pszType );
	setText( 2, pszDesc );
	setPixmap( 0, QPixmap( column_xpm ) );
}

void classColumn::setOpen( bool o )
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

void classColumn::setup()
{
    setExpandable( FALSE );
#ifdef QT_V4LAYOUT
    Q3ListViewItem::setup();
#else
    QListViewItem::setup();
#endif
}


#ifdef QT_V4LAYOUT
void classColumn::selectionChanged( Q3ListViewItem *p )
#else
void classColumn::selectionChanged( QListViewItem *p )
#endif
{
	if ( p == this )
	{
	}
}


