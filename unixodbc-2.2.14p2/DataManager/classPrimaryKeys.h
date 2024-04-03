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
#ifndef classPrimaryKeys_included
#define classPrimaryKeys_included

#ifdef QT_V4LAYOUT
#include <Qt/qstring.h>
#include <Qt/qmessagebox.h>
#include <Qt/qlist.h>
#else
#include <qstring.h>
#include <qmessagebox.h>
#include <qlist.h>
#endif

#include <sqlext.h>
#include "classNode.h"

class classPrimaryKeys: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classPrimaryKeys( Q3ListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
    classPrimaryKeys( Q3ListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
    classPrimaryKeys( Q3ListViewItem 	*pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
#else
    classPrimaryKeys( QListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
    classPrimaryKeys( QListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
    classPrimaryKeys( QListViewItem 	*pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
#endif
	~classPrimaryKeys();

    void setOpen( bool );
    void setup();
#ifdef QT_V4LAYOUT
	void selectionChanged( Q3ListViewItem * );
#else
	void selectionChanged( QListViewItem * );
#endif

private:
	SQLHDBC	hDbc;
	QString	qsTable;

	void Init( SQLHDBC hDbc, char *pszTable );
	void LoadColumns();

};

#endif

