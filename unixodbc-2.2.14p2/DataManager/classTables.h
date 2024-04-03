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
#ifndef classTables_included
#define classTables_included

#include <stdio.h>
#include <sqlext.h>
#ifdef QT_V4LAYOUT
#include <Qt/qstring.h>
#include <Qt/qmessagebox.h>
#include <Qt/q3ptrlist.h>
#else
#include <qstring.h>
#include <qmessagebox.h>
#include <qlist.h>
#endif
#include "classNode.h"
#include "classTable.h"

class classTables: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classTables( Q3ListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0 );
    classTables( Q3ListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0 );
    classTables( Q3ListViewItem 	*pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0 );
#else
    classTables( QListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0 );
    classTables( QListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0 );
    classTables( QListViewItem 	*pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0 );
#endif
	~classTables();

    void setOpen( bool bOpen );
    void setup();
#ifdef QT_V4LAYOUT
	void selectionChanged( Q3ListViewItem * );
#else
	void selectionChanged( QListViewItem * );
#endif

private:
#ifdef QT_V4LAYOUT
	Q3PtrList<classTable> listTables;
#else
	QList<classTable> listTables;
#endif
	SQLHDBC	hDbc;

	void Init( SQLHDBC hDbc );
	void LoadTables();

};

#endif

