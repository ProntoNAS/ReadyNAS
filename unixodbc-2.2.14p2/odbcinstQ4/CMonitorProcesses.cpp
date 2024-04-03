/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#include "CMonitorProcesses.h"

#define MAXPROCESSES 100
#define MAXHANDLES 4

CMonitorProcesses::CMonitorProcesses( QWidget* pwidgetParent )
	: QTableWidget( pwidgetParent )
{
    setToolTip( tr( "list of processes currently owning one or more ODBC handles" ) );
    setWhatsThis( tr( "This shows a list of processes currently owning one or more ODBC handles. The number of handles are shown. This is revised frequently while in view. This is useful for debugging and monitoring performance." ) );

    // lets assume, for the moment, that we can get monitoring info from DM...
    bEnabled = true;

    // setup table...
    {
        // we remember rows from last load so we can optimize next load...
        nRowsWithValues = 0;

        // we use a fixed number of row/cols so init here...
        setRowCount( MAXPROCESSES );
        setColumnCount( 5 );

        // init headers and decorations...
        {
            QStringList stringlist;
            setHorizontalHeaderLabels( stringlist << "PID" << "Environments" << "Connections" << "Statements" << "Descriptors" );
            setSelectionBehavior( QAbstractItemView::SelectRows );
            setSelectionMode( QAbstractItemView::SingleSelection );
            verticalHeader()->setVisible( false );
        }
        
        // preload table with item objects...
        for ( int nRow = 0; nRow < rowCount(); nRow++ )
        {
            for ( int nCol = 0; nCol < columnCount(); nCol++ )
            {
                setItem( nRow, nCol, new QTableWidgetItem( "" ) );
            }
        }
    }

    // init timer...
    {
        hStats = 0;
        pTimer = new QTimer( this );
        connect( pTimer, SIGNAL(timeout()), SLOT(slotLoad()) );
        pTimer->start( 2000 ); // 1000 = 1second
    }
}

CMonitorProcesses::~CMonitorProcesses()
{
    if ( hStats )
        uodbc_close_stats( hStats );
}

void CMonitorProcesses::slotLoad()
{
    // only bother with this if we are visible...
    if ( !isVisible() )
        return;

    if ( !bEnabled )
        return;

    // init coms with DM but don't even bother the DM until (if) we become visible... 
    if ( !hStats )
    {
        if ( uodbc_open_stats( &hStats, UODBC_STATS_READ ) != 0 )
        {
//            char szError[512];
//            QMessageBox::critical( this, tr( "Monitor Processes" ),  QString( "uodbc_open_stats failed\n%1" ).arg( uodbc_stats_error( szError, 512 ) ) );
//            bEnabled = false;
            return;
        }
    }

    // get current processes info...
    {
        uodbc_stats_retentry aPIDs[MAXPROCESSES];
        int nPIDs = uodbc_get_stats( hStats, 0, aPIDs, MAXPROCESSES );

        for ( int nPID = 0; nPID < MAXPROCESSES; nPID++ )
        {
            if ( nPID < nPIDs )
            {
                // get handle count for current process...
                uodbc_stats_retentry aHandles[MAXHANDLES];
                int nHandles = uodbc_get_stats( hStats, aPIDs[nPID].value.l_value, aHandles, MAXHANDLES );

                if ( nHandles > 0 )
                {
                    item( nPID, 0 )->setText( QString( "%1" ).arg( aPIDs[nPID].value.l_value ) );
                    for ( int nHandle = 0; nHandle < MAXHANDLES; nHandle++ )
                    {
                        item( nPID, nHandle + 1 )->setText( QString( "%1" ).arg( aHandles[nHandle].value.l_value ) );
                    }
                }
                else
                    clearRow( nPID );
            }
            else
            {
                // no need to clear remaining rows if no data in them...
                if ( nPID >= nRowsWithValues )
                {
                    nRowsWithValues = nPID;
                    return;
                }
                clearRow( nPID );
            }
        }
    }
}

void CMonitorProcesses::clearRow( int nRow )
{
    int nCol;

    for ( nCol = 0; nCol < columnCount(); nCol++ )
    {
        item( nRow, nCol )->setText( "" );
    }
}

