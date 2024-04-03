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
#ifndef classTable_included
#define classTable_included

#include <stdio.h>

#ifdef QT_V4LAYOUT
#include <Qt/qstring.h>
#include <Qt/qmessagebox.h>
#include <Qt/q3ptrlist.h>
#include <Qt/qpixmap.h>
#else
#include <qstring.h>
#include <qmessagebox.h>
#include <qlist.h>
#include <qpixmap.h>
#endif
#include <sqlext.h>
#include "classNode.h"
#include "classColumn.h"
#include "classPrimaryKeys.h"
#include "classSpecialColumns.h"
#include "classIndexs.h"
#include "classBrowseFrame.h"

class classTable: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classTable( Q3ListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszName = 0, char *pszType = 0, char *pszDescription = 0 );
    classTable( Q3ListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszName = 0, char *pszType = 0, char *pszDescription = 0 );
    classTable( Q3ListViewItem 	*pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszName = 0, char *pszType = 0, char *pszDescription = 0 );
#else
    classTable( QListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszName = 0, char *pszType = 0, char *pszDescription = 0 );
    classTable( QListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszName = 0, char *pszType = 0, char *pszDescription = 0 );
    classTable( QListViewItem 	*pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszName = 0, char *pszType = 0, char *pszDescription = 0 );
#endif
	~classTable();

    void setOpen( bool bOpen );
    void setup();
#ifdef QT_V4LAYOUT
	void selectionChanged( Q3ListViewItem * );
#else
	void selectionChanged( QListViewItem * );
#endif

private:
#ifdef QT_V4LAYOUT
	Q3PtrList<classColumn>	listColumns;
#else
	QList<classColumn>	listColumns;
#endif
	classPrimaryKeys	*pPrimaryKeys;
    classSpecialColumns	*pSpecialColumns;
	classIndexs			*pIndexs;
	SQLHDBC	            hDbc;
	QString	            qsName;
	classBrowseFrame    *pBrowse;

	void Init( SQLHDBC hDbc, char *pszName, char *pszType, char *pszDescription );
    void Fini();
	void LoadColumns();
};

#endif

