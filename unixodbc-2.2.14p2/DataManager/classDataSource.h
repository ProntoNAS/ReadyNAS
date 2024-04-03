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
#ifndef classDataSource_included
#define classDataSource_included

#ifdef QT_V4LAYOUT
#include <Qt/qstring.h>
#include <Qt/qmessagebox.h>
#include <Qt/qpixmap.h>
#else
#include <qstring.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#endif

#include <ini.h>
#include <odbcinst.h>
#include <sqlext.h>
#include "classNode.h"
#include "classLogin.h"
#include "classTables.h"
#include "classConnectionFrame.h"

class classDataSource: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classDataSource( Q3ListView 		*pParent, classCanvas *pCanvas, int nDataSourceType = User,	char *pszDataSourceName = 0, SQLHENV	hEnv = 0 );
    classDataSource( Q3ListViewItem 	*pParent, classCanvas *pCanvas, int nDataSourceType = User,	char *pszDataSourceName = 0, SQLHENV	hEnv = 0 );
    classDataSource( Q3ListViewItem 	*pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, int nDataSourceType = User,	char *pszDataSourceName = 0, SQLHENV	hEnv = 0 );
#else
    classDataSource( QListView 		*pParent, classCanvas *pCanvas, int nDataSourceType = User,	char *pszDataSourceName = 0, SQLHENV	hEnv = 0 );
    classDataSource( QListViewItem 	*pParent, classCanvas *pCanvas, int nDataSourceType = User,	char *pszDataSourceName = 0, SQLHENV	hEnv = 0 );
    classDataSource( QListViewItem 	*pParent, QListViewItem *pAfter, classCanvas *pCanvas, int nDataSourceType = User,	char *pszDataSourceName = 0, SQLHENV	hEnv = 0 );
#endif
	~classDataSource();

	enum DataSourceTypes
	{
		User,
		System
	};
    void setOpen( bool bOpen );
    void setup();
#ifdef QT_V4LAYOUT
	void selectionChanged ( Q3ListViewItem * );
#else
	void selectionChanged ( QListViewItem * );
#endif

private:
	classTables *           pTables;
	classConnectionFrame *  pConnectionFrame;
	bool	                bLoggedIn;
	QString	                qsDataSourceName;
	int 	                nDataSourceType;
	SQLHENV	                hEnv;
	SQLHDBC	                hDbc;

	void Init( int nDataSourceType, char *pszDataSourceName, SQLHENV hEnv );
};

#endif

