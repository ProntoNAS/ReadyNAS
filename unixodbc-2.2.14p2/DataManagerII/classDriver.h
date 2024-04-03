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
#ifndef classDriver_included
#define classDriver_included

#include "classCanvas.h"
#include "classNode.h"
#ifdef QT_V4LAYOUT
#include <Qt/qstring.h>
#include <Qt/q3listview.h>
#else
#include <qstring.h>
#include <qlistview.h>
#endif
#include <sqlext.h>

class classDriver: public classNode
{
public:
#ifdef QT_V4LAYOUT
    classDriver( Q3ListViewItem  *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, const char *pszDriverName = 0 );
#else
    classDriver( QListViewItem  *pParent, QListViewItem *pAfter, classCanvas *pCanvas, const char *pszDriverName = 0 );
#endif
   ~classDriver() {}

private:
    QString qsDriverName;

};
#endif

