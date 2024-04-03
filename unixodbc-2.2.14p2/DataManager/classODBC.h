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
#ifndef classODBC_included
#define classODBC_included

#ifdef QT_V4LAYOUT
#include <Qt/qmessagebox.h>
#else
#include <qmessagebox.h>
#endif

#include <sqlext.h>
#include "classNode.h"
#include "classDrivers.h"
#include "classDataSources.h"

class classODBC: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classODBC( Q3ListView 		*pParent, classCanvas *pCanvas );
    classODBC( Q3ListViewItem 	*pParent, classCanvas *pCanvas );
#else
    classODBC( QListView 		*pParent, classCanvas *pCanvas );
    classODBC( QListViewItem 	*pParent, classCanvas *pCanvas );
#endif
    ~classODBC();

    void setOpen( bool );
    void setup();
#ifdef QT_V4LAYOUT
	void selectionChanged ( Q3ListViewItem * );
#else
	void selectionChanged ( QListViewItem * );
#endif

private:
	SQLHENV				hEnv;
	classDrivers		*pDrivers;
	classDataSources	*pDataSourcesUser;
	classDataSources	*pDataSourcesSystem;

	void Init();
};

#endif

