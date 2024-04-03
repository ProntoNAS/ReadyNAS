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
#ifndef classIndexs_included
#define classIndexs_included

#ifdef QT_V4LAYOUT
#include <Qt/qstring.h>
#include <Qt/qmessagebox.h>
#include <Qt/q3ptrlist.h>
#else
#include <qstring.h>
#include <qmessagebox.h>
#include <qlist.h>
#endif

#include <sqlext.h>
#include "classNode.h"
#include "classIndex.h"


class classIndexs: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classIndexs( Q3ListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
    classIndexs( Q3ListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
    classIndexs( Q3ListViewItem 	*pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
#else
    classIndexs( QListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
    classIndexs( QListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
    classIndexs( QListViewItem 	*pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
#endif
	~classIndexs();

    void setOpen( bool );
    void setup();
#ifdef QT_V4LAYOUT
	void selectionChanged( Q3ListViewItem * );
#else
	void selectionChanged( QListViewItem * );
#endif

private:
#ifdef QT_V4LAYOUT
	Q3PtrList<classIndex>	listIndexs;
#else
	QList<classIndex>	listIndexs;
#endif
	SQLHDBC				hDbc;
	QString				qsTable;

	void Init( SQLHDBC hDbc, char *pszTable );
	void LoadIndexs();

};

#endif

