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

#include "CMonitor.h"

#include "CMonitorHandleCounts.h"
#include "CMonitorProcesses.h"

#include "Stats48.xpm"

CMonitor::CMonitor( QWidget *pwidgetParent )
    : QTabWidget( pwidgetParent )
{
    // handles
    {
        pMonitorHandleCounts = new CMonitorHandleCounts( this );
        addTab( pMonitorHandleCounts, tr( "Handle Counts" ) );
    }

    // tracing
    {
        pMonitorProcesses = new CMonitorProcesses( this );
        addTab( pMonitorProcesses, tr( "Processes" ) );
    }

    setWindowIcon( QPixmap( xpmStats48 ) );
    setWindowTitle( tr( "Monitor" ) );
}

CMonitor::~CMonitor()
{
}



