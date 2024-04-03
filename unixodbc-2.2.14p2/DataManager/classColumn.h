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
#ifndef classColumn_included
#define classColumn_included

#ifdef QT_V4LAYOUT
#include <Qt/qstring.h>
#include <Qt/qmessagebox.h>
#include <Qt/qpixmap.h>
#else
#include <qstring.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#endif

#include <sqlext.h>
#include "classNode.h"

class classColumn: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classColumn( Q3ListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszName = 0, char *pszType = 0, char *pszDesc = 0 );
    classColumn( Q3ListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszName = 0, char *pszType = 0, char *pszDesc = 0 );
    classColumn( Q3ListViewItem 	*pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszName = 0, char *pszType = 0, char *pszDesc = 0 );
#else
    classColumn( QListView 		*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszName = 0, char *pszType = 0, char *pszDesc = 0 );
    classColumn( QListViewItem 	*pParent, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszName = 0, char *pszType = 0, char *pszDesc = 0 );
    classColumn( QListViewItem 	*pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc = 0, char *pszName = 0, char *pszType = 0, char *pszDesc = 0 );
#endif
	~classColumn();

    void setOpen( bool );
    void setup();
#ifdef QT_V4LAYOUT
	void selectionChanged ( Q3ListViewItem * );
#else
	void selectionChanged ( QListViewItem * );
#endif

private:
	SQLHDBC	hDbc;
	QString	qsName;

	void Init( SQLHDBC hDbc, char *pszName, char *pszType, char *pszDesc );
};

#endif

