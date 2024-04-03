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

#include "CPooling.h"

#include "Pooling48.xpm"

CPooling::CPooling( QWidget *pwidgetParent )
    : QWidget( pwidgetParent )
{
    QGridLayout *playout = new QGridLayout;

    pcheckboxEnable = new QCheckBox;
    pcheckboxEnable->setToolTip( tr( "check to enable pooling" ) );
    pcheckboxEnable->setWhatsThis( tr( "Check this to enable connection pooling. Connections with similar or same connection options can be reduced to one, common, connection and this can often improve performance - particularly for server processes." ) );

    playout->addWidget( new QLabel( tr( "Enable" ) ), 0, 0 );
    playout->addWidget( pcheckboxEnable, 0, 1 );

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

    setWindowIcon( QPixmap( xpmPooling48 ) );
    setWindowTitle( tr( "Pooling" ) );

    loadData();
}

CPooling::~CPooling()
{
}

bool CPooling::loadData()
{
    slotDefault();

    char szPooling[10];

    SQLGetPrivateProfileString( "ODBC", "Pooling", "No", szPooling, sizeof(szPooling), "odbcinst.ini" );
    if ( szPooling[0] == '1' || toupper( szPooling[0] ) == 'Y' || toupper( szPooling[0] ) == 'O' )
        pcheckboxEnable->setChecked( true );

    return true;
}

bool CPooling::saveData()
{
    if ( slotApply() )
        return true;

    int n = QMessageBox::question( this, tr( "Save..." ), tr( "Some data could not be saved. Click Discard if its ok to lose changes." ), QMessageBox::Cancel | QMessageBox::Discard, QMessageBox::Discard );
    if ( n == QMessageBox::Discard )
        return true;

    return false;
}

bool CPooling::slotApply()
{
    if ( !SQLWritePrivateProfileString( "ODBC", "Pooling", ( pcheckboxEnable->isChecked() ? "1" : "0" ), "odbcinst.ini" ) )
    {
        CODBCInst::showErrors( this, tr( "Could not save options. You may not have the privileges to write odbcinst.ini." ) );
        return false;
    }

    return true;
}

bool CPooling::slotDefault()
{
    pcheckboxEnable->setChecked( false );
}

