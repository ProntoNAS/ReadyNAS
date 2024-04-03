/**************************************************
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 18.FEB.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey   - pharvey@codebydesign.com
 **************************************************/
#ifndef classIndexs_included
#define classIndexs_included

#include "classCanvas.h"
#include "classIndex.h"
#ifdef QT_V4LAYOUT
#include <Qt/qstring.h>
#include <Qt/q3ptrlist.h>
#include <Qt/q3listview.h>
#else
#include <qstring.h>
#include <qlist.h>
#include <qlistview.h>
#endif
#include <sqlext.h>

class classIndexs: public classNode
{
public:
#ifdef QT_V4LAYOUT
   classIndexs( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc, const char *pszTable = 0, const char *pszLibrary = 0 );
#else
   classIndexs( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc, const char *pszTable = 0, const char *pszLibrary = 0 );
#endif
  ~classIndexs() {}

   void setOpen( bool );

private:
#ifdef QT_V4LAYOUT
  Q3PtrList<classColumn> listIndexs;
#else
  QList<classColumn> listIndexs;
#endif
  SQLHDBC            hDbc;
  QString            qsLibrary;
  QString            qsTable;

  void LoadIndexs();

};
#endif

