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

#include "CManageDrivers.h"

#include "CDriverList.h"

#include "Driver48.xpm"

CManageDrivers::CManageDrivers( QWidget* pwidgetParent )
    : QWidget( pwidgetParent )
{
    QPushButton *ppushbuttonAdd = new QPushButton( tr( "A&dd..." ) );
    ppushbuttonAdd->setToolTip( tr( "click to add/register a driver" ) );
    ppushbuttonAdd->setWhatsThis( tr( "Click this to register a driver. Registering a driver informs the Driver Manager about its location and allows some driver-level options to be set. A well behaved driver installer will automatically register a driver but there are times when this needs to be done manually here. You will need to have elevated (root) privileges to do this." ) );

    QPushButton *ppushbuttonConfigure = new QPushButton( tr( "&Configure..." ) );
    ppushbuttonConfigure->setToolTip( tr( "click to configure the selected driver" ) );
    ppushbuttonConfigure->setWhatsThis( tr( "Click this to edit driver options. You will need to have elevated (root) privileges to do this." ) );

    QPushButton *ppushbuttonRemove = new QPushButton( tr( "&Remove" ) );
    ppushbuttonRemove->setToolTip( tr( "click to remove the selected driver" ) );
    ppushbuttonRemove->setWhatsThis( tr( "Click this to deregister the selected driver. This does not remove any files from the system. You will need to have elevated (root) privileges to do this." ) );

    QHBoxLayout *playout                = new QHBoxLayout;
    QVBoxLayout *playoutButtons         = new QVBoxLayout;
    CDriverList *pDriverList            = new CDriverList;

    playoutButtons->addWidget( ppushbuttonAdd );
    playoutButtons->addWidget( ppushbuttonConfigure );
    playoutButtons->addWidget( ppushbuttonRemove );
    playoutButtons->addStretch( 10 );

    playout->addWidget( pDriverList );
    playout->addLayout( playoutButtons );

    setLayout( playout );

    connect( ppushbuttonAdd, SIGNAL(clicked()), pDriverList, SLOT(slotAdd()) );
    connect( ppushbuttonConfigure, SIGNAL(clicked()), pDriverList, SLOT(slotEdit()) );
    connect( ppushbuttonRemove, SIGNAL(clicked()), pDriverList, SLOT(slotDelete()) );
    connect( pDriverList, SIGNAL(signalChanged()), this, SIGNAL(signalChanged()) );

    setWindowIcon( QPixmap( xpmDriver48 ) );
    setWindowTitle( tr( "Drivers" ) );
}

CManageDrivers::~CManageDrivers()
{
}


