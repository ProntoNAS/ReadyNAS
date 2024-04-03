/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#include "CMonitorHandleCounts.h"

CMonitorHandleCounts::CMonitorHandleCounts( QWidget *pwidgetParent )
	: QWidget( pwidgetParent )
{
    setToolTip( tr( "number of active ODBC handles" ) );
    setWhatsThis( tr( "This shows the number of active ODBC handles at any given moment. The values are automatically updated frequently. This is useful for debugging and performance monitoring." ) );

    // lets assume, for the moment, that we can get monitoring info from DM...
    bEnabled = true;

    QGridLayout *pLayout = new QGridLayout;

    // setup labels to show current values as simple numbers...
    {
        plabelEnvironment   = new QLabel( "0" );
        plabelConnection    = new QLabel( "0" );
        plabelStatement     = new QLabel( "0" );
        plabelDescriptor    = new QLabel( "0" );

        pLayout->addWidget( plabelEnvironment,  0, 0 );
        pLayout->addWidget( plabelConnection,   0, 1 );
        pLayout->addWidget( plabelStatement,    0, 2 );
        pLayout->addWidget( plabelDescriptor,   0, 3 );
    }

    // setup sliders to show current values in an interesting manner...
    {
        // set default scale for sliders...
        nSliderMax = 10;

        psliderEnvironment  = new QSlider;
        psliderConnection   = new QSlider;
        psliderStatement    = new QSlider;
        psliderDescriptor   = new QSlider;

        psliderEnvironment->setInvertedAppearance( true );
        psliderConnection->setInvertedAppearance( true );
        psliderStatement->setInvertedAppearance( true );
        psliderDescriptor->setInvertedAppearance( true );
//        psliderEnvironment->setTickmarks( QSlider::Both );
//        psliderConnection->setTickmarks( QSlider::Both );
//        psliderStatement->setTickmarks( QSlider::Both );
//        psliderDescriptor->setTickmarks( QSlider::Both );

        psliderEnvironment->setMinimum( 0-nSliderMax );
        psliderConnection->setMinimum( 0-nSliderMax );
        psliderStatement->setMinimum( 0-nSliderMax );
        psliderDescriptor->setMinimum( 0-nSliderMax );

        psliderEnvironment->setMaximum( 0 );
        psliderConnection->setMaximum( 0 );
        psliderStatement->setMaximum( 0 );
        psliderDescriptor->setMaximum( 0 );

        pLayout->addWidget( psliderEnvironment, 1, 0 );
        pLayout->addWidget( psliderConnection,  1, 1 );
        pLayout->addWidget( psliderStatement,   1, 2 );
        pLayout->addWidget( psliderDescriptor,  1, 3 );
    }

    // setup some labels to show what each slider is for...
    {
        pLayout->addWidget( new QLabel( tr( "Env" ) ), 2, 0 );
        pLayout->addWidget( new QLabel( tr( "Con" ) ), 2, 1 );
        pLayout->addWidget( new QLabel( tr( "Sta" ) ), 2, 2 );
        pLayout->addWidget( new QLabel( tr( "Des" ) ), 2, 3 );
    }

    // get timer going...
    {
        hStats  = 0;
        pTimer  = new QTimer( this );
        connect( pTimer, SIGNAL(timeout()), SLOT(slotLoad()) );
        pTimer->start( 2000 ); // 1000 = 1 second
    }

    // adopt our layout...
    setLayout( pLayout );
}

CMonitorHandleCounts::~CMonitorHandleCounts()
{
    if ( hStats )
        uodbc_close_stats( hStats );
}

void CMonitorHandleCounts::slotLoad()
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
//            QMessageBox::critical( this, tr( "Monitor Handle Counts" ), QString( tr( "uodbc_open_stats failed\n%1" ) ).arg( uodbc_stats_error( szError, 512 ) ) );
//            bEnabled = false;
            return;
        }
    }

    // get current handle counts...
    {
        uodbc_stats_retentry aStats[4];

        uodbc_get_stats( hStats, -1, aStats, 4 );

//        if ( uodbc_get_stats( hStats, -1, aStats, 4 ) == 4 )
//        {
//            char szError[512];
//            QMessageBox::critical( this, tr( "Monitor Handle Counts" ), QString( tr( "uodbc_get_stats failed\n%1" ) ).arg( uodbc_stats_error( szError, 512 ) ) );
//            bEnabled = false;
//            return;
//        }

        // bump up our scale for the sliders as needed...
        if ( aStats[0].value.l_value > nSliderMax )
            nSliderMax = aStats[0].value.l_value;
        if ( aStats[1].value.l_value > nSliderMax )
            nSliderMax = aStats[1].value.l_value;
        if ( aStats[2].value.l_value > nSliderMax )
            nSliderMax = aStats[2].value.l_value;
        if ( aStats[3].value.l_value > nSliderMax )
            nSliderMax = aStats[3].value.l_value;

        // update UI bits...
        plabelEnvironment->setText( QString( "%1" ).arg( aStats[0].value.l_value ) );
        psliderEnvironment->setMinimum( 0-nSliderMax );
        psliderEnvironment->setValue( 0-aStats[0].value.l_value );

        plabelConnection->setText( QString( "%1" ).arg( aStats[1].value.l_value ) );
        psliderConnection->setMinimum( 0-nSliderMax );
        psliderConnection->setValue( 0-aStats[1].value.l_value );

        plabelStatement->setText( QString( "%1" ).arg( aStats[2].value.l_value ) );
        psliderStatement->setMinimum( 0-nSliderMax );
        psliderStatement->setValue( 0-aStats[2].value.l_value );

        plabelDescriptor->setText( QString( "%1" ).arg( aStats[3].value.l_value ) );
        psliderDescriptor->setMinimum( 0-nSliderMax );
        psliderDescriptor->setValue( 0-aStats[3].value.l_value );
    }
}

