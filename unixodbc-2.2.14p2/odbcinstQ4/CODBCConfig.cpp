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

#include "CODBCConfig.h"
#include "CManageDataSourceNames.h"
#include "CMonitor.h"
#include "CAdvanced.h"
#include "CAbout.h"
#include "CPage.h"

#include "ODBCManageDataSources64.xpm"

CODBCConfig::CODBCConfig( QWidget* pwidgetParent, Qt::WindowFlags nFlags )
	: QDialog( pwidgetParent, nFlags )
{
    QVBoxLayout *playout00 = new QVBoxLayout;

    // init the help system...
    {
        // \note slotHelp fails to invoke assistant when in shell as root - problem here perhaps?
        pAssistantClient = new QAssistantClient( QLibraryInfo::location( QLibraryInfo::BinariesPath ), this );

        /*! 
         * \todo
         *
         *  1. create unixODBC.adp
         *  2. get unixODBC.adp and associated doc to install in various install scenarios
         *  3. tweek the way we get the path to the doc so its more portable (currently just Linux friendly)
         *
         * \sa
         *
         *  slotHelp
         */
        QStringList stringlistArgs;
        stringlistArgs << "-profile" << QString( "%1/share/doc/packages/unixODBC/unixODBC.adp" ).arg( THEPREFIX );
        pAssistantClient->setArguments( stringlistArgs );
        connect( pAssistantClient, SIGNAL(error(const QString&)), this, SLOT(slotHelpError(const QString&)) );
    }

    // init icon menu and stacked widget...
    {
        QHBoxLayout *playout01 = new QHBoxLayout;

        createConfigWidgets();
        createIconMenu();

        playout01->addWidget( plistwidgetIcons );
        playout01->addWidget( pstackedwidgetConfigWidgets, 10 );    
        playout00->addLayout( playout01 );
    }

    // init dialog buttons...
    {
        QDialogButtonBox *  pdialogbuttonbox    = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Help );
        QFrame *            pframe              = new QFrame;

        pframe->setFrameStyle( QFrame::HLine );

//        connect( pdialogbuttonbox, SIGNAL(accepted()), this, SLOT(slotAccept()) );
        connect( pdialogbuttonbox, SIGNAL(accepted()), this, SLOT(accept()) );
        connect( pdialogbuttonbox, SIGNAL(rejected()), this, SLOT(reject()) );
        connect( pdialogbuttonbox, SIGNAL(helpRequested()), this, SLOT(slotHelp()) );

        playout00->addWidget( pframe );
        playout00->addWidget( pdialogbuttonbox );
    }

    // 
    setLayout( playout00 );
    setWindowTitle( tr( "ODBC Administrator" ) );
    setWindowIcon( QPixmap( xpmODBCManageDataSources64 ) );

    // restore last state...
    loadState();
}

CODBCConfig::~CODBCConfig()
{
    pAssistantClient->closeAssistant();
    delete pAssistantClient;
    saveState();
}

bool CODBCConfig::saveData()
{
    if ( !pManageDataSourceNames->saveData() );
        return false;
    if ( !pMonitor->saveData() )
        return false;
    if ( !pAdvanced->saveData() )
        return false;
    if ( !pAbout->saveData() )
        return false;

    return true;
}

void CODBCConfig::slotChangePage( QListWidgetItem *plistwidgetitemCurrent, QListWidgetItem *plistwidgetitemPrevious )
{
    if ( !plistwidgetitemCurrent )
        plistwidgetitemCurrent = plistwidgetitemPrevious;
    
    pstackedwidgetConfigWidgets->setCurrentIndex( plistwidgetIcons->row( plistwidgetitemCurrent ) );
}

void CODBCConfig::slotHelp()
{
    pAssistantClient->showPage( QString( "%1/share/doc/packages/unixODBC/index.html" ).arg( THEPREFIX ) );
}

void CODBCConfig::slotHelpError( const QString &stringMessage )
{
    QMessageBox::critical( this, tr("ODBC Administrator"), stringMessage );
}

void CODBCConfig::slotAccept()
{
    /* \todo This can only be implemented after any widget which does something in saveData() tracks changes
             in its data such that an actual save is only done if data changes. */
printf( "[PAH][%s][%d]\n", __FILE__, __LINE__ );
    if ( saveData() )
//        done( QDialog::Accepted );
        accept();
}

void CODBCConfig::createIconMenu()
{
    plistwidgetIcons = new QListWidget;
    plistwidgetIcons->setViewMode( QListView::IconMode );
    plistwidgetIcons->setIconSize( QSize( 96, 84 ) );
    plistwidgetIcons->setMovement( QListView::Static );
    plistwidgetIcons->setMaximumWidth( 128 );
    plistwidgetIcons->setSpacing( 12 );

    QListWidgetItem *plistwidgetitemDataSourceNames = new QListWidgetItem ( plistwidgetIcons );
    plistwidgetitemDataSourceNames->setTextAlignment( Qt::AlignHCenter );
    plistwidgetitemDataSourceNames->setIcon( pManageDataSourceNames->windowIcon() );
    plistwidgetitemDataSourceNames->setText( tr( "Data\nSource\nNames" ) );

    QListWidgetItem *plistwidgetitemMonitor = new QListWidgetItem ( plistwidgetIcons ); 
    plistwidgetitemMonitor->setTextAlignment( Qt::AlignHCenter );
    plistwidgetitemMonitor->setIcon( pMonitor->windowIcon() );
    plistwidgetitemMonitor->setText( tr( "Monitor" ) );

    QListWidgetItem *plistwidgetitemAdvanced = new QListWidgetItem ( plistwidgetIcons ); 
    plistwidgetitemAdvanced->setTextAlignment( Qt::AlignHCenter );
    plistwidgetitemAdvanced->setIcon( pAdvanced->windowIcon() );
    plistwidgetitemAdvanced->setText( tr( "Advanced" ) );

    QListWidgetItem *plistwidgetitemAbout = new QListWidgetItem ( plistwidgetIcons ); 
    plistwidgetitemAbout->setTextAlignment( Qt::AlignHCenter );
    plistwidgetitemAbout->setIcon( pAbout->windowIcon() );
    plistwidgetitemAbout->setText( tr( "About" ) );

    plistwidgetIcons->setCurrentItem( plistwidgetitemDataSourceNames );

    connect( plistwidgetIcons, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)), this, SLOT(slotChangePage(QListWidgetItem *, QListWidgetItem*)) );
}

void CODBCConfig::createConfigWidgets()
{
    pManageDataSourceNames  = new CManageDataSourceNames;
    pMonitor                = new CMonitor;
    pAdvanced               = new CAdvanced;
    pAbout                  = new CAbout;

    pstackedwidgetConfigWidgets = new QStackedWidget;
    pstackedwidgetConfigWidgets->addWidget( new CPage( this, pManageDataSourceNames->windowTitle(), pManageDataSourceNames, pManageDataSourceNames->windowIcon(), pManageDataSourceNames->windowHelp() ) );
    pstackedwidgetConfigWidgets->addWidget( new CPage( this, pMonitor->windowTitle(), pMonitor, pMonitor->windowIcon(), pMonitor->windowHelp() ) );
    pstackedwidgetConfigWidgets->addWidget( new CPage( this, pAdvanced->windowTitle(), pAdvanced, pAdvanced->windowIcon(), pAdvanced->windowHelp() ) );
    pstackedwidgetConfigWidgets->addWidget( new CPage( this, pAbout->windowTitle(), pAbout, pAbout->windowIcon(), pAbout->windowHelp() ) );

    /* this will handle case where we delete a driver and it results in DSN's being deleted (those that use the driver) */
    connect( pAdvanced, SIGNAL(signalChanged()), pManageDataSourceNames, SLOT(slotLoad()) ); 
}

void CODBCConfig::loadState()
{
    QSettings settings;

    int nW = settings.value( "CODBCConfig/w", geometry().width() ).toInt();
    int nH = settings.value( "CODBCConfig/h", geometry().height() ).toInt();

    resize( nW, nH );
}

void CODBCConfig::saveState()
{
    QSettings settings;

    settings.setValue( "CODBCConfig/w", width() );
    settings.setValue( "CODBCConfig/h", height() );
}


