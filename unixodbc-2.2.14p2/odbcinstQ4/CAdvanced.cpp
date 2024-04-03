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

#include "CAdvanced.h"

#include "CManageDrivers.h"
#include "CPooling.h"
#include "CTracing.h"
#include "CThreading.h"
#include "CPage.h"

#include "Advanced48.xpm"

CAdvanced::CAdvanced( QWidget *pwidgetParent )
    : QTabWidget( pwidgetParent )
{
    pManageDrivers  = new CManageDrivers( this );
    pPooling        = new CPooling( this );
    pTracing        = new CTracing( this );
    pThreading      = new CThreading( this );

    CPage *ppageDrivers     = new CPage( this, QString::null, pManageDrivers, pManageDrivers->windowIcon(), pManageDrivers->windowHelp() ); 
    CPage *ppagePooling     = new CPage( this, QString::null, pPooling, pPooling->windowIcon(), pPooling->windowHelp() ); 
    CPage *ppageTracing     = new CPage( this, QString::null, pTracing, pTracing->windowIcon(), pTracing->windowHelp() ); 
    CPage *ppageThreading   = new CPage( this, QString::null, pThreading, pThreading->windowIcon(), pThreading->windowHelp() ); 

    addTab( ppageDrivers, tr( "Drivers" ) );
    addTab( ppagePooling, tr( "Pooling" ) );
    addTab( ppageTracing, tr( "Tracing" ) );
    addTab( ppageThreading, tr( "Threading" ) );

    connect( pManageDrivers, SIGNAL(signalChanged()), this, SIGNAL(signalChanged()) );

    setWindowIcon( QPixmap( xpmAdvanced48 ) );
    setWindowTitle( tr( "Advanced" ) );
}

CAdvanced::~CAdvanced()
{
}

bool CAdvanced::saveData()
{
    if ( !pManageDrivers->saveData() )
        return false;
    if ( !pPooling->saveData() )
        return false;
    if ( !pTracing->saveData() )
        return false;
    if ( !pThreading->saveData() )
        return false;
}


