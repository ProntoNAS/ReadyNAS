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

#include "CPropertiesDialog.h"

#include "CPropertiesModel.h"
#include "CPropertiesDelegate.h"

#include "ODBC.xpm"

CPropertiesDialog::CPropertiesDialog( QWidget *pwidgetParent, HODBCINSTPROPERTY hFirstProperty )
	: QDialog( pwidgetParent )
{
    QVBoxLayout *           playout                 = new QVBoxLayout;
    QTableView *            ptableview              = new QTableView;
    QFrame *                pframe                  = new QFrame;
    QDialogButtonBox *      pdialogbuttonbox        = new QDialogButtonBox( QDialogButtonBox::Cancel | QDialogButtonBox::Ok | QDialogButtonBox::Help );

    pframe->setFrameStyle( QFrame::HLine );

    ppropertiesmodel    = new CPropertiesModel( 0, hFirstProperty );
    ppropertiesdelegate = new CPropertiesDelegate;
    ptableview->setModel( ppropertiesmodel );
    ptableview->setItemDelegateForColumn ( 1, ppropertiesdelegate );
    ptableview->verticalHeader()->hide();

    connect( pdialogbuttonbox, SIGNAL(accepted()), this, SLOT(accept()) );
    connect( pdialogbuttonbox, SIGNAL(rejected()), this, SLOT(reject()) );

    playout->addWidget( ptableview, 10 );
    playout->addWidget( pframe );
    playout->addWidget( pdialogbuttonbox );

    setWindowIcon( QPixmap( xpmODBC ) );
    setLayout( playout );

    doLoadState();
}

CPropertiesDialog::~CPropertiesDialog()
{
    doSaveState();
    delete ppropertiesdelegate;
    delete ppropertiesmodel;
}

void CPropertiesDialog::doLoadState()
{
    QSettings settings;

    int nW = settings.value( "CPropertiesDialog/w", geometry().width() ).toInt();
    int nH = settings.value( "CPropertiesDialog/h", geometry().height() ).toInt();

    resize( nW, nH );
}

void CPropertiesDialog::doSaveState()
{
    QSettings settings;

    settings.setValue( "CPropertiesDialog/w", width() );
    settings.setValue( "CPropertiesDialog/h", height() );
}


