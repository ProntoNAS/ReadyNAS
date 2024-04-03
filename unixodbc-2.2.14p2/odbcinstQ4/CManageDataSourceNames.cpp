/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#include <QtGui>

#include "CManageDataSourceNames.h"

#include "CDataSourceNames.h"
#include "CDataSourceNamesFile.h"
#include "CPage.h"

#include "DataSourceName48.xpm"

CManageDataSourceNames::CManageDataSourceNames( QWidget *pwidgetParent )
    : QTabWidget( pwidgetParent )
{
    pDataSourceNamesUser    = new CDataSourceNames( this, ODBC_USER_DSN );
    pDataSourceNamesSystem  = new CDataSourceNames( this, ODBC_SYSTEM_DSN );
    pDataSourceNamesFile    = new CDataSourceNamesFile( this );

    CPage *ppageUser    = new CPage( this, QString::null, pDataSourceNamesUser, pDataSourceNamesUser->windowIcon(), pDataSourceNamesUser->windowHelp() ); 
    CPage *ppageSystem  = new CPage( this, QString::null, pDataSourceNamesSystem, pDataSourceNamesSystem->windowIcon(), pDataSourceNamesSystem->windowHelp() ); 
    CPage *ppageFile    = new CPage( this, QString::null, pDataSourceNamesFile, pDataSourceNamesFile->windowIcon(), pDataSourceNamesFile->windowHelp() ); 

    addTab( ppageUser, tr( "User" ) );
    addTab( ppageSystem, tr( "System" ) );
    addTab( ppageFile, tr( "File" ) );

    setWindowIcon( QPixmap( xpmDataSourceName48 ) );
    setWindowTitle( tr( "Data Source Names" ) );
}

CManageDataSourceNames::~CManageDataSourceNames()
{
}

void CManageDataSourceNames::slotLoad()
{
    pDataSourceNamesUser->slotLoad();
    pDataSourceNamesSystem->slotLoad();
    pDataSourceNamesFile->slotLoad();
}


