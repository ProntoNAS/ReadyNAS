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

#include "classColumn.h"
#ifdef QT_V4LAYOUT
#include <Qt/qpixmap.h>
#else
#include <qpixmap.h>
#endif
#include "column.xpm"

#ifdef QT_V4LAYOUT
classColumn::classColumn( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc, const char *pszName, const char *pszType, const char *pszDesc )
    : classNode( pParent, pAfter, pCanvas )
#else
classColumn::classColumn( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, SQLHDBC hDbc, const char *pszName, const char *pszType, const char *pszDesc )
    : classNode( pParent, pAfter, pCanvas )
#endif
{
  setText( 0, pszName );
  setText( 1, pszType );
  setText( 2, pszDesc );
  setPixmap( 0, QPixmap( column_xpm ) );
  setExpandable( FALSE );
}

