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
#ifndef classTables_included
#define classTables_included

#ifdef QT_V4LAYOUT
#include <Qt/q3ptrlist.h>
#include <Qt/q3listview.h>
#include <Qt/qstring.h>
#else
#include <qlist.h>
#include <qlistview.h>
#include <qstring.h>
#endif
#include "classNode.h"
#include "classCanvas.h"
#include "classTable.h"
#include <sqlext.h>

class classTables: public classNode
{
public:
#ifdef QT_V4LAYOUT
   classTables( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc, const char * szLibrary = 0 );
#else
   classTables( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc, const char * szLibrary = 0 );
#endif
  ~classTables() {}

   void setOpen( bool bOpen );
#ifdef QT_V4LAYOUT
   void selectionChanged( Q3ListViewItem * );
#else
   void selectionChanged( QListViewItem * );
#endif

private:
#ifdef QT_V4LAYOUT
  Q3PtrList<classTable> listTables;
#else
  QList<classTable> listTables;
#endif
  SQLHDBC           hDbc;
  QString           qsLibrary;

  void LoadTables();

};
#endif

