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
#ifndef classDataSources_included
#define classDataSources_included

#include "classNode.h"
#include "classCanvas.h"
#include "classDataSource.h"
#include "classODBC.h"
#ifdef QT_V4LAYOUT
#include <Qt/q3ptrlist.h>
#include <Qt/q3listview.h>
#else
#include <qlist.h>
#include <qlistview.h>
#endif
#include <sqlext.h>

class classDataSources: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classDataSources( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, classODBC::DSType dataSourceType );
#else
    classDataSources( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, classODBC::DSType dataSourceType );
#endif
   ~classDataSources() {}

    void setOpen( bool bOpen );
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
  classODBC::DSType      dataSourceType;

  void Init();
};
#endif

