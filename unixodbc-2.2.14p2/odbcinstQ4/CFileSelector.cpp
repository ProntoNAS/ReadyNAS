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

#include "CFileSelector.h"

#include "Set.xpm"

CFileSelector::CFileSelector( CFileSelectorMode nMode, const QString &stringLabel, bool bCompact, bool bButton, QWidget* pwidgetParent )
	: QWidget( pwidgetParent )
{
    this->nMode = nMode;

	QHBoxLayout *playoutTop = new QHBoxLayout;

    if ( bCompact )
    {
        playoutTop->setMargin( 0 );
        playoutTop->setSpacing( 0 );
    }

    if ( !stringLabel.isEmpty() )
        playoutTop->addWidget( new QLabel( stringLabel ) );

    {
        pLineEdit = new QLineEdit;
        playoutTop->addWidget( pLineEdit, 10 );
        connect( pLineEdit, SIGNAL(editingFinished()), this, SIGNAL(signalChanged()) );
    }

    if ( bButton )
    {
        QToolButton *pToolButton = new QToolButton;
        pToolButton->setToolTip( tr( "click this to invoke a dialog to help select a file/directory" ) );
        pToolButton->setWhatsThis( tr( "Click this to invoke a file dialog to assist in selecting a file/directory." ) );
        pToolButton->setIcon( QIcon( xpmSet ) );
        playoutTop->addWidget( pToolButton );
        connect( pToolButton, SIGNAL(clicked()), this, SLOT(slotInvokeDialog()) );
    }

    setLayout( playoutTop );

}

CFileSelector::~CFileSelector()
{
}

void CFileSelector::setText( const QString &stringText )
{
    pLineEdit->setText( stringText );
    emit signalChanged();
}

QString CFileSelector::getText()
{
    return pLineEdit->text();
}

void CFileSelector::slotInvokeDialog()
{
    QString stringText;
    QString stringSelectedFilter;

    switch ( nMode )
    {
        case TraceFile:
            stringText = QFileDialog::getSaveFileName( this, tr( "Select log file..." ), getText(), tr( "Trace File (*.log *.txt);;All Files (*.*)" ), &stringSelectedFilter, QFileDialog::DontConfirmOverwrite );
            break;

        case TraceLibrary:
            {
                QString stringFilter = tr( "Share Library (*" );
                if ( strlen( SHLIBEXT ) > 0 )
                    stringFilter += SHLIBEXT;
                else
                    stringFilter += ".so";
                stringFilter += tr( "*);;All Files (*.*)" );
                stringText = QFileDialog::getOpenFileName( this, tr( "Select ODBC Trace Libary..." ), getText(), stringFilter, &stringSelectedFilter );
            }
            break;

        case FileDSNDirectory:
            stringText = QFileDialog::getExistingDirectory( this, tr( "Select directory..." ), getText() );
            break;

        case Driver:
            {
                QString stringFilter = tr( "Share Library (*" );
                if ( strlen( SHLIBEXT ) > 0 )
                    stringFilter += SHLIBEXT;
                else
                    stringFilter += ".so";
                stringFilter += tr( "*);;All Files (*.*)" );
                stringText = QFileDialog::getOpenFileName( this, tr( "Select Driver..." ), getText(), stringFilter, &stringSelectedFilter );
            }
            break;

        case Setup:
            {
                QString stringFilter = tr( "Share Library (*" );
                if ( strlen( SHLIBEXT ) > 0 )
                    stringFilter += SHLIBEXT;
                else
                    stringFilter += ".so";
                stringFilter += tr( "*);;All Files (*.*)" );
                stringText = QFileDialog::getOpenFileName( this, tr( "Select Driver Setup..." ), getText(), stringFilter, &stringSelectedFilter );
            }
            break;
    }

    if ( stringText.isNull() )
	    return;

    setText( stringText );
}


