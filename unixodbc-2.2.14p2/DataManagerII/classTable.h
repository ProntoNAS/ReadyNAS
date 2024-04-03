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
#ifndef classTable_included
#define classTable_included

#include "classCanvas.h"
#include "classColumn.h"
#include "classPrimaryKeys.h"
#include "classSpecialColumns.h"
#include "classIndexs.h"
#include "classBrowseFrame.h"
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

class classTable: public classNode
{
public:
#ifdef QT_V4LAYOUT
   classTable( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc, const char *pszName, const char *pszType = 0, const char *pszDescription = 0, const char *pszLibraryName = 0 );
#else
   classTable( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc, const char *pszName, const char *pszType = 0, const char *pszDescription = 0, const char *pszLibraryName = 0 );
#endif
  ~classTable();

   void setOpen( bool bOpen );
#ifdef QT_V4LAYOUT
   void selectionChanged( Q3ListViewItem * );
#else
   void selectionChanged( QListViewItem * );
#endif

private:
#ifdef QT_V4LAYOUT
  Q3PtrList<classColumn>  listColumns;
#else
  QList<classColumn>  listColumns;
#endif
  classPrimaryKeys    *pPrimaryKeys;
  classSpecialColumns *pSpecialColumns;
  classIndexs         *pIndexs;
  classBrowseFrame    *pBrowse;
  SQLHDBC             hDbc;
  QString             qsLibrary;
  QString             qsTable;

  void Fini();
  void LoadColumns();
};
#endif

