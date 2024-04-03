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

#include "classNode.h"
#include "classCanvas.h"
#ifdef QT_V4LAYOUT
#include <Qt/q3listview.h>
#else
#include <qlistview.h>
#endif

#ifdef QT_V4LAYOUT
classNode::classNode( Q3ListView *pParent, classCanvas *pCanvas )
    : pCanvas( pCanvas) , Q3ListViewItem( pParent )
#else
classNode::classNode( QListView *pParent, classCanvas *pCanvas )
    : pCanvas( pCanvas) , QListViewItem( pParent )
#endif
{
}

#ifdef QT_V4LAYOUT
classNode::classNode( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas )
    : pCanvas( pCanvas) , Q3ListViewItem( pParent, pAfter )
#else
classNode::classNode( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas )
    : pCanvas( pCanvas) , QListViewItem( pParent, pAfter )
#endif
{
}

