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

#include "classDriver.h"
#ifdef QT_V4LAYOUT
#include <Qt/qpixmap.h>
#else
#include <qpixmap.h>
#endif
#include <ini.h>
#include <odbcinst.h>
#include "driver.xpm"

#ifdef QT_V4LAYOUT
classDriver::classDriver( Q3ListViewItem *pParent, Q3ListViewItem *pAfter, classCanvas *pCanvas, const char *pszDriverName )
#else
classDriver::classDriver( QListViewItem *pParent, QListViewItem *pAfter, classCanvas *pCanvas, const char *pszDriverName )
#endif
    : classNode( pParent, pAfter, pCanvas )
{
  char szResults[9600] ;
  char szPropertyValue[INI_MAX_PROPERTY_VALUE+1] ;
  szResults[0]        = 0 ;
  szPropertyValue[0]  = 0 ;

  if ( pszDriverName != 0 )
    qsDriverName = pszDriverName;

  setPixmap( 0, QPixmap( driver_xpm ) );
  setExpandable( FALSE );

  if ( SQLGetPrivateProfileString((char*) qsDriverName.ascii(), "Description", "", szResults, sizeof(szResults), "odbcinst" ) > 0 )
    iniElement( szResults, 0, 0, 0, szPropertyValue, INI_MAX_PROPERTY_VALUE );

  setText( 0, qsDriverName );
  setText( 1, "DRIVER  " );
  setText( 2, szPropertyValue );
}

