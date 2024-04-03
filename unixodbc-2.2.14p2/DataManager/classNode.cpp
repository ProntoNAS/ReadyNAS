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
#include "classNode.h"

#ifdef QT_V4LAYOUT
classNode::classNode( Q3ListView *pParent, classCanvas *pCanvas )
    : Q3ListViewItem( pParent )
#else
classNode::classNode( QListView *pParent, classCanvas *pCanvas )
    : QListViewItem( pParent )
#endif
{
	Init( pCanvas);
}

#ifdef QT_V4LAYOUT
classNode::classNode( Q3ListViewItem *pParent, classCanvas *pCanvas )
    : Q3ListViewItem( pParent )
#else
classNode::classNode( QListViewItem *pParent, classCanvas *pCanvas )
    : QListViewItem( pParent )
#endif
{
	Init( pCanvas );
}

#ifdef QT_V4LAYOUT
classNode::classNode( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas )
    : Q3ListViewItem( pParent, pAfter )
#else
classNode::classNode( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas )
    : QListViewItem( pParent, pAfter )
#endif
{
	Init( pCanvas );
}

classNode::~classNode()
{
}

void classNode::Init( classCanvas *pCanvas )
{
	this->pCanvas = pCanvas;
	setText( 0, "Object" );
	setText( 1, "Type" );
	setText( 2, "Description" );
}

void classNode::setOpen( bool o )
{
    if ( o && !childCount() )
	{
    }
#ifdef QT_V4LAYOUT
    Q3ListViewItem::setOpen( o );
#else
    QListViewItem::setOpen( o );
#endif
}

void classNode::setup()
{
    setExpandable( TRUE );
#ifdef QT_V4LAYOUT
    Q3ListViewItem::setup();
#else
    QListViewItem::setup();
#endif
}

#ifdef QT_V4LAYOUT
void classNode::selectionChanged ( Q3ListViewItem * )
#else
void classNode::selectionChanged ( QListViewItem * )
#endif
{
}





