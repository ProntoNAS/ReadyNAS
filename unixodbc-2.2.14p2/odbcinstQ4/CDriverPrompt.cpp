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

#include "CDriverPrompt.h"

#include "CDriverList.h"

#include "ODBC.xpm"

CDriverPrompt::CDriverPrompt( QWidget *pwidgetParent )
	: QDialog( pwidgetParent )
{
    QVBoxLayout *       playout             = new QVBoxLayout;
    QLabel *            pLabel              = new QLabel( tr( "Select a driver for which you want to set up a data source..." ) );
    QDialogButtonBox *  pdialogbuttonbox    = new QDialogButtonBox( QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help );

    pDriverList = new CDriverList;

    playout->addWidget( pLabel );
    playout->addWidget( pDriverList );
    playout->addWidget( pdialogbuttonbox );

    connect( pdialogbuttonbox, SIGNAL(accepted()), this, SLOT(slotOk()) );
    connect( pdialogbuttonbox, SIGNAL(rejected()), this, SLOT(reject()) );

    setLayout( playout );

    setWindowTitle( tr( "Create New Data Source..." ) );
    setWindowIcon( QPixmap( xpmODBC ) );

    doLoadState();
}

CDriverPrompt::~CDriverPrompt()
{
    doSaveState();
}

QString CDriverPrompt::getFriendlyName()
{
    return pDriverList->getFriendlyName();
}

QString CDriverPrompt::getDescription()
{
    return pDriverList->getDescription();
}

QString CDriverPrompt::getDriver()
{
    return pDriverList->getDriver();
}

QString CDriverPrompt::getSetup()
{
    return pDriverList->getSetup();
}

void CDriverPrompt::slotOk()
{
	if ( getFriendlyName().isEmpty() )
        QMessageBox::information( this, tr( "ODBC Administrator" ),  tr( "please select a Driver" ) );
    else
        accept();
}

void CDriverPrompt::doLoadState()
{
    QSettings settings;

    int nW = settings.value( "CDriverPrompt/w", geometry().width() ).toInt();
    int nH = settings.value( "CDriverPrompt/h", geometry().height() ).toInt();

    resize( nW, nH );
}

void CDriverPrompt::doSaveState()
{
    QSettings settings;

    settings.setValue( "CDriverPrompt/w", width() );
    settings.setValue( "CDriverPrompt/h", height() );
}



