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

#include "CHelp.h"

#include "Info16.xpm"
#include "InfoArrow16.xpm"

CHelp::CHelp( const QString &stringKey, const QIcon &icon, const QString &stringText, QWidget *pwidgetParent )
	: QWidget( pwidgetParent )
{
    this->stringKey = stringKey;
    bVisibleView    = true;

    QHBoxLayout *pLayout = new QHBoxLayout( this );

    // toggle button...
    {
        QVBoxLayout *playoutV = new QVBoxLayout;

        pToolButton = new QToolButton;
        pToolButton->setIcon( QIcon( xpmInfoArrow16 ) );
        pToolButton->setWhatsThis( tr( "Use this to toggle the help text in and out of view. The state is saved at exit and used the next time the program is run." ) );
        pToolButton->setToolTip( tr( "toggle help text" ) );

        playoutV->addWidget( pToolButton );
        playoutV->addStretch( 10 );

        connect( pToolButton, SIGNAL(clicked()), this, SLOT(slotToggleView()) );

        pLayout->addLayout( playoutV );
    }

    // setup frame...
    {
        pFrame = new QFrame;
        pFrame->setFrameShape( QFrame::StyledPanel );

        // setup icon and text...
        {
            QHBoxLayout *playoutH   = new QHBoxLayout( pFrame );

            plabelIcon = new QLabel;
            plabelIcon->setPixmap( icon.pixmap( 64, 64 ) );
            plabelText = new QLabel( stringText );
            plabelText->setWordWrap( true );
        
            playoutH->addWidget( plabelIcon );
            playoutH->addWidget( plabelText, 10 );
        }
        pLayout->addWidget( pFrame, 10 );
    }

    loadState();
}

CHelp::~CHelp()
{
    saveState();
}

void CHelp::loadState()
{
    QSettings settings;

    bool bVisible = settings.value( QString( "CHelp/%1/visible" ).arg( stringKey ), true ).toBool();

    if ( bVisible != bVisibleView )
        slotToggleView();
}

void CHelp::saveState()
{
    QSettings settings;

    settings.setValue( QString( "CHelp/%1/visible" ).arg( stringKey ), bVisibleView );
}

void CHelp::slotToggleView()
{
    if ( bVisibleView )
    {
        pFrame->setFrameShape( QFrame::NoFrame );
        plabelIcon->hide();
        plabelText->hide();
        pToolButton->setIcon( QIcon( xpmInfo16 ) );
        bVisibleView = false;
    }
    else
    {
        pFrame->setFrameShape( QFrame::StyledPanel );
        plabelIcon->show();
        plabelText->show();
        pToolButton->setIcon( QIcon( xpmInfoArrow16 ) );
        bVisibleView = true;
    }
}



