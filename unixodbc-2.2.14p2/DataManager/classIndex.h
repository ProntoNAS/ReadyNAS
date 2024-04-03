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
#ifndef classIndex_included
#define classIndex_included

#ifdef QT_V4LAYOUT
#include <Qt/q3ptrlist.h>
#include <Qt/qstring.h>
#include <Qt/qmessagebox.h>
#include <Qt/qpixmap.h>
#else
#include <qlist.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#endif
#include <sqlext.h>
#include "classNode.h"
#include "classColumn.h"

class classIndex: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classIndex( Q3ListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0, char *pszIndex = 0, char *pszDesc = 0 );
    classIndex( Q3ListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0, char *pszIndex = 0, char *pszDesc = 0 );
    classIndex( Q3ListViewItem 	*pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0, char *pszIndex = 0, char *pszDesc = 0 );
#else
    classIndex( QListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0, char *pszIndex = 0, char *pszDesc = 0 );
    classIndex( QListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0, char *pszIndex = 0, char *pszDesc = 0 );
    classIndex( QListViewItem 	*pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0, char *pszIndex = 0, char *pszDesc = 0 );
#endif
	~classIndex();

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
	QString	qsIndex;
#ifdef QT_V4LAYOUT
	Q3PtrList<classColumn>	listColumns;
#else
	QList<classColumn>	listColumns;
#endif

	void Init( SQLHDBC hDbc, char *pszTable, char *pszIndex, char *pszDesc );
	void LoadColumns();
};

#endif

