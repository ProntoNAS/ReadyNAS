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
#ifndef classDrivers_included
#define classDrivers_included

#include "classCanvas.h"
#include "classDriver.h"
#ifdef QT_V4LAYOUT
#include <Qt/q3ptrlist.h>
#include <Qt/q3listview.h>
#else
#include <qlist.h>
#include <qlistview.h>
#endif
#include <sqlext.h>

class classDrivers: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classDrivers( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas );
#else
    classDrivers( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas );
#endif
   ~classDrivers() {}

    void setOpen( bool );

private:
#ifdef QT_V4LAYOUT
    Q3PtrList<classDriver> listDrivers;
#else
    QList<classDriver> listDrivers;
#endif

};
#endif

