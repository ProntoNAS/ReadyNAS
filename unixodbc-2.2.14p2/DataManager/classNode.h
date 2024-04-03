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
#ifndef classNode_included
#define classNode_included

#ifdef QT_V4LAYOUT
#include <Qt/q3listview.h>
#include <Qt/qmessagebox.h>
#else
#include <qlistview.h>
#include <qmessagebox.h>
#endif

#include "classCanvas.h"

#ifdef QT_V4LAYOUT
class classNode: public Q3ListViewItem
#else
class classNode: public QListViewItem
#endif
{
public:
#ifdef QT_V4LAYOUT
    classNode( Q3ListView 		*pParent, classCanvas *pCanvas );
    classNode( Q3ListViewItem 	*pParent, classCanvas *pCanvas );
    classNode( Q3ListViewItem 	*pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas );
#else
    classNode( QListView 		*pParent, classCanvas *pCanvas );
    classNode( QListViewItem 	*pParent, classCanvas *pCanvas );
    classNode( QListViewItem 	*pParent, QListViewItem *pAfter, classCanvas *pCanvas );
#endif
    ~classNode();

	virtual void Init( classCanvas *pCanvas );
    virtual void setOpen( bool );
    virtual void setup();
#ifdef QT_V4LAYOUT
	virtual void selectionChanged ( Q3ListViewItem * );
#else
	virtual void selectionChanged ( QListViewItem * );
#endif

protected:
	classCanvas	*pCanvas;
	
};

#endif

