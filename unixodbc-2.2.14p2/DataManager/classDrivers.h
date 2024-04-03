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
#ifndef classDrivers_included
#define classDrivers_included

#include <ini.h>
#include <odbcinst.h>
#include <sqlext.h>
#ifdef QT_V4LAYOUT
#include <Qt/q3ptrlist.h>
#else
#include <qlist.h>
#endif
#include "classNode.h"
#include "classDriver.h"

class classDrivers: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classDrivers( Q3ListView 		*pParent, classCanvas *pCanvas, SQLHENV hEnv = 0 );
    classDrivers( Q3ListViewItem 	*pParent, classCanvas *pCanvas, SQLHENV hEnv = 0 );
    classDrivers( Q3ListViewItem 	*pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHENV hEnv = 0 );
#else
    classDrivers( QListView 		*pParent, classCanvas *pCanvas, SQLHENV hEnv = 0 );
    classDrivers( QListViewItem 	*pParent, classCanvas *pCanvas, SQLHENV hEnv = 0 );
    classDrivers( QListViewItem 	*pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHENV hEnv = 0 );
#endif
	~classDrivers();

    void setOpen( bool );
    void setup();
#ifdef QT_V4LAYOUT
	void selectionChanged ( Q3ListViewItem * );
#else
	void selectionChanged ( QListViewItem * );
#endif

private:
#ifdef QT_V4LAYOUT
	Q3PtrList<classDriver> listDrivers;
#else
	QList<classDriver> listDrivers;
#endif
	SQLHENV	hEnv;

	void Init( SQLHENV	hEnv );
};

#endif

