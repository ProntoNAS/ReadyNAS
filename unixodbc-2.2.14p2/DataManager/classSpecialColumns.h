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
#ifndef classSpecialColumns_included
#define classSpecialColumns_included

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
#include "classColumn.h"

class classSpecialColumns: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classSpecialColumns( Q3ListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
    classSpecialColumns( Q3ListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
    classSpecialColumns( Q3ListViewItem 	*pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
#else
    classSpecialColumns( QListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
    classSpecialColumns( QListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
    classSpecialColumns( QListViewItem 	*pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszTable = 0 );
#endif
	~classSpecialColumns();

    void setOpen( bool );
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
	SQLHDBC				hDbc;
	QString				qsTable;

	void Init( SQLHDBC hDbc, char *pszTable );
	void LoadColumns();

};

#endif

