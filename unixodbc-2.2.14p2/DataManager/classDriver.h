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
#ifndef classDriver_included
#define classDriver_included

#ifdef QT_V4LAYOUT
#include <Qt/qstring.h>
#else
#include <qstring.h>
#endif

#include <ini.h>
#include <odbcinst.h>
#include <sqlext.h>
#include "classNode.h"

class classDriver: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classDriver( Q3ListView 		*pParent, classCanvas *pCanvas,	char *pszDriverName = 0, SQLHENV	hEnv = 0 );
    classDriver( Q3ListViewItem 	*pParent, classCanvas *pCanvas,	char *pszDriverName = 0, SQLHENV	hEnv = 0 );
    classDriver( Q3ListViewItem 	*pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas,	char *pszDriverName = 0, SQLHENV	hEnv = 0 );
#else
    classDriver( QListView 		*pParent, classCanvas *pCanvas,	char *pszDriverName = 0, SQLHENV	hEnv = 0 );
    classDriver( QListViewItem 	*pParent, classCanvas *pCanvas,	char *pszDriverName = 0, SQLHENV	hEnv = 0 );
    classDriver( QListViewItem 	*pParent, QListViewItem *pAfter, classCanvas *pCanvas,	char *pszDriverName = 0, SQLHENV	hEnv = 0 );
#endif
	~classDriver();

    void setOpen( bool );
    void setup();
#ifdef QT_V4LAYOUT
	void selectionChanged ( Q3ListViewItem * );
#else
	void selectionChanged ( QListViewItem * );
#endif

private:
	QString	qsDriverName;
	SQLHENV	hEnv;

	void Init( char *pszDriverName, SQLHENV hEnv );

};

#endif

