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

#include "CThreading.h"

#include "Threading48.xpm"

const char *ODBC_HELP_THREADING = "Threading options are the means with which the DM handles concurrency (different bits of code trying to access the same internal data at the same time). " \
                            "If compiled with thread support the DM allows four different thread strategies.\n" \
                            "Level 0 - Only the DM internal structures are protected the driver is assumed to take care of it's self.\n" \
                            "Level 1 - The driver is protected down to the statement level each statement will be protected, and the same for the connect level for connect functions, note that descriptors are considered equal to statements when it comes to thread protection.\n" \
                            "Level 2 - The driver is protected at the connection level. only one thread can be in a particular driver at one time.\n" \
                            "Level 3 - The driver is protected at the env level, only one thing at a time.";

CThreading::CThreading( QWidget *pwidgetParent )
    : QWidget( pwidgetParent )
{
    QGridLayout *playout = new QGridLayout;

    pspinboxLevel = new QSpinBox;
    pspinboxLevel->setMinimum( 0 );
    pspinboxLevel->setMaximum( 3 );
    pspinboxLevel->setToolTip( tr( "set threading level" ) );
    pspinboxLevel->setWhatsThis( tr( "Select the threading level. Higher numbers are safer while lower numbers allow more concurrency." ) );

    playout->addWidget( new QLabel( tr( "Level" ) ), 0, 0 );
    playout->addWidget( pspinboxLevel, 0, 1 );

    QPushButton *ppushbuttonSetDefault = new QPushButton( "De&fault", this );
    ppushbuttonSetDefault->setToolTip( tr( "click to set values to default" ) );
    ppushbuttonSetDefault->setWhatsThis( tr( "Click to restore the default values." ) );

    QPushButton *ppushbuttonApply = new QPushButton( "A&pply", this );
    ppushbuttonApply->setToolTip( tr( "click to apply the current settings" ) );
    ppushbuttonApply->setWhatsThis( tr( "Click this to save the current values." ) );

    playout->addWidget( ppushbuttonSetDefault, 0, 2 );
    playout->addWidget( ppushbuttonApply, 1, 2 );

    connect( ppushbuttonSetDefault, SIGNAL(clicked()), SLOT(slotDefault()) );
    connect( ppushbuttonApply, SIGNAL(clicked()), SLOT(slotApply()) );

    playout->setRowStretch( 3, 10 );
    playout->setColumnStretch( 1, 10 );
    setLayout( playout );

    setWindowIcon( QPixmap( xpmThreading48 ) );
    setWindowTitle( tr( "Threading" ) );

    loadData();
}

CThreading::~CThreading()
{
}

QString CThreading::windowHelp() 
{ 
    return QString( tr( ODBC_HELP_THREADING ) ); 
}

bool CThreading::loadData()
{
    slotDefault();

    char szThreading[100];

    SQLGetPrivateProfileString( "ODBC", "Threading", "3", szThreading, sizeof(szThreading), "odbcinst.ini" );

    pspinboxLevel->setValue( atoi( szThreading ) );

    return true;
}

bool CThreading::saveData()
{
    if ( slotApply() )
        return true;

    int n = QMessageBox::question( this, tr( "Save..." ), tr( "Some data could not be saved. Click Discard if its ok to lose changes." ), QMessageBox::Cancel | QMessageBox::Discard, QMessageBox::Discard );
    if ( n == QMessageBox::Discard )
        return true;

    return false;
}

bool CThreading::slotApply()
{
    char szThreading[2];

    sprintf( szThreading, "%d", pspinboxLevel->value() );

    if ( !SQLWritePrivateProfileString( "ODBC", "Threading", szThreading, "odbcinst.ini" ) )
    {
        CODBCInst::showErrors( this, tr( "Could not save options. You may not have the privileges to write odbcinst.ini." ) );
        return false;
    }

    return true;
}

bool CThreading::slotDefault()
{
    pspinboxLevel->setValue( 3 );
}

