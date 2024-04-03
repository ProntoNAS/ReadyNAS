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
#ifndef classNode_included
#define classNode_included

#include "classCanvas.h"
#ifdef QT_V4LAYOUT
#include <Qt/q3listview.h>
#else
#include <qlistview.h>
#endif

#ifdef QT_V4LAYOUT
class classNode: public Q3ListViewItem
{
public:
    classNode( Q3ListView     *pParent,                        classCanvas *pCanvas );
    classNode( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas );
   ~classNode() {}

protected:
    classCanvas *pCanvas;

};
#else
class classNode: public QListViewItem
{
public:
    classNode( QListView     *pParent,                        classCanvas *pCanvas );
    classNode( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas );
   ~classNode() {}

protected:
    classCanvas *pCanvas;

};
#endif
#endif

