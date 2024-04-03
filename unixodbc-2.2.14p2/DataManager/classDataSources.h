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
#ifndef classDataSources_included
#define classDataSources_included

#ifdef QT_V4LAYOUT
#include <Qt/qpixmap.h>
#include <Qt/q3ptrlist.h>
#else
#include <qpixmap.h>
#include <qlist.h>
#endif

#include <ini.h>
#include <odbcinst.h>
#include <sqlext.h>
#include "classNode.h"
#include "classDataSource.h"

class classDataSources: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classDataSources( Q3ListView 		*pParent, classCanvas *pCanvas, int nDataSourceType = User, SQLHENV hEnv = 0 );
    classDataSources( Q3ListViewItem 	*pParent, classCanvas *pCanvas, int nDataSourceType = User, SQLHENV hEnv = 0 );
    classDataSources( Q3ListViewItem 	*pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, int nDataSourceType = User, SQLHENV hEnv = 0 );
#else
    classDataSources( QListView 		*pParent, classCanvas *pCanvas, int nDataSourceType = User, SQLHENV hEnv = 0 );
    classDataSources( QListViewItem 	*pParent, classCanvas *pCanvas, int nDataSourceType = User, SQLHENV hEnv = 0 );
    classDataSources( QListViewItem 	*pParent, QListViewItem *pAfter, classCanvas *pCanvas, int nDataSourceType = User, SQLHENV hEnv = 0 );
#endif
	~classDataSources();

	enum DataSourceTypes
	{
		User,
		System
	};
    void setOpen( bool bOpen );
    void setup();
#ifdef QT_V4LAYOUT
	void selectionChanged( Q3ListViewItem * );
#else
	void selectionChanged( QListViewItem * );
#endif

private:
#ifdef QT_V4LAYOUT
	Q3PtrList<classDataSource> listDataSources;
#else
	QList<classDataSource> listDataSources;
#endif
	int 	nDataSourceType;
	SQLHENV hEnv;

	void Init( int nDataSourceType, SQLHENV hEnv );
};

#endif

