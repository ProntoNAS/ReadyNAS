/**************************************************
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 18.FEB.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey         - pharvey@codebydesign.com
 **************************************************/
#ifndef classDataSource_included
#define classDataSource_included

#include "classCanvas.h"
#include "classTables.h"
#include "classConnectionFrame.h"
#include "classODBC.h"
#ifdef QT_V4LAYOUT
#include <Qt/qstring.h>
#include <Qt/q3listview.h>
#include <Qt/q3ptrlist.h>
#else
#include <qstring.h>
#include <qlistview.h>
#include <qlist.h>
#endif
#include <sqlext.h>

class classDataSource: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classDataSource( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, classODBC::DSType dataSourceType, const char *pszDataSourceName = 0 );
#else
    classDataSource( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, classODBC::DSType dataSourceType, const char *pszDataSourceName = 0 );
#endif
   ~classDataSource();

    void setOpen( bool bOpen );
#ifdef QT_V4LAYOUT
    void selectionChanged ( Q3ListViewItem * );
#else
    void selectionChanged ( QListViewItem * );
#endif
    void LoadLibraries();

private:
#ifdef QT_V4LAYOUT
    Q3PtrList<classTables>      listLibraries;
#else
    QList<classTables>      listLibraries;
#endif
    classConnectionFrame *  pConnectionFrame;
    QString                 qsDataSourceName;
    EnvironmentScoper       env;
    ConnectionScoper        dbc;
};
#endif

