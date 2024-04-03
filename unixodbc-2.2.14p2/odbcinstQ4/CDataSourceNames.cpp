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

#include "CDataSourceNames.h"

#include "CDataSourceNameList.h"

#include "DataSourceNameUser48.xpm"
#include "DataSourceNameSystem48.xpm"

CDataSourceNames::CDataSourceNames( QWidget* pwidgetParent, int nSource )
    : QWidget( pwidgetParent )
{
    Q_ASSERT( (nSource == ODBC_USER_DSN || nSource == ODBC_SYSTEM_DSN) );

    this->nSource = nSource;

    pDataSourceNameList = new CDataSourceNameList( this, nSource );

    QPushButton *ppushbuttonAdd = new QPushButton( tr( "A&dd..." ) );
    ppushbuttonAdd->setToolTip( tr( "click to add a data source name" ) );
    ppushbuttonAdd->setWhatsThis( tr( "Click this to add a data source name. A registered driver is selected then an attempt is made to load the drivers setup code. The drivers setup should support the unixODBC setup feature." ) );

    QPushButton *ppushbuttonConfigure = new QPushButton( tr( "&Configure..." ) );
    ppushbuttonConfigure->setToolTip( tr( "click to configure the selected data source name" ) );
    ppushbuttonConfigure->setWhatsThis( tr( "Click to edit the data source name properties." ) );

    QPushButton *ppushbuttonRemove = new QPushButton( tr( "&Remove" ) );
    ppushbuttonRemove->setToolTip( tr( "click to remove the selected data source name" ) );
    ppushbuttonRemove->setWhatsThis( tr( "Click to remove the select data source name." ) );

    QHBoxLayout *playout                = new QHBoxLayout;
    QVBoxLayout *playoutButtons         = new QVBoxLayout;

    playoutButtons->addWidget( ppushbuttonAdd );
    playoutButtons->addWidget( ppushbuttonConfigure );
    playoutButtons->addWidget( ppushbuttonRemove );
    playoutButtons->addStretch( 10 );

    playout->addWidget( pDataSourceNameList );
    playout->addLayout( playoutButtons );

    setLayout( playout );

    connect( ppushbuttonAdd, SIGNAL(clicked()), pDataSourceNameList, SLOT(slotAdd()) );
    connect( ppushbuttonConfigure, SIGNAL(clicked()), pDataSourceNameList, SLOT(slotEdit()) );
    connect( ppushbuttonRemove, SIGNAL(clicked()), pDataSourceNameList, SLOT(slotDelete()) );

    if ( nSource == ODBC_USER_DSN )
    {
        setWindowIcon( QPixmap( xpmDataSourceNameUser48 ) );
        setWindowTitle( tr( "User Data Source Names" ) );
    }
    else
    {
        setWindowIcon( QPixmap( xpmDataSourceNameSystem48 ) );
        setWindowTitle( tr( "System Data Source Names" ) );
    }
}

CDataSourceNames::~CDataSourceNames()
{
}

void CDataSourceNames::slotLoad()
{
    pDataSourceNameList->slotLoad();
}


