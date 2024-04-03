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

#include "CTracing.h"

#include "CFileSelector.h"

#include "Tracing48.xpm"

CTracing::CTracing( QWidget *pwidgetParent )
	: QWidget( pwidgetParent )
{
    QGridLayout *playout = new QGridLayout;

    pcheckboxEnable = new QCheckBox;
    pcheckboxEnable->setToolTip( tr( "check to enable/disable tracing" ) );
    pcheckboxEnable->setWhatsThis( tr( "Check this to tell the Driver Manager to log all ODBC calls to a trace file. Used for debugging. This will reduce performance and use up disk space - turn it off after each debugging session." ) );

    pcheckboxForce = new QCheckBox;
    pcheckboxForce->setToolTip( tr( "check this to force tracing" ) );
    pcheckboxForce->setWhatsThis( tr( "Check this to tell the Driver Manager that it should log all ODBC calls even if otherwise told not to (by app). This is only relevant if tracing is generally enabled." ) );

    pfileselectorTraceFile      = new CFileSelector( CFileSelector::TraceFile );
    pfileselectorTraceFile->setToolTip( tr( "select a Trace File" ) );
    pfileselectorTraceFile->setWhatsThis( tr( "Select a file to hold the trace output." ) );

    pfileselectorTraceLibrary   = new CFileSelector( CFileSelector::TraceLibrary );
    pfileselectorTraceLibrary->setToolTip( tr( "select a Trace Library" ) );
    pfileselectorTraceLibrary->setWhatsThis( tr( "The Trace Library is used, by the Driver Manager and/or Driver, to create the trace output. unixODBC has a standard one but a custom one can be used." ) );

    playout->addWidget( new QLabel( tr( "Enable" ) ), 0, 0 );
    playout->addWidget( new QLabel( tr( "Force" ) ), 1, 0 );
    playout->addWidget( new QLabel( tr( "Output File" ) ), 2, 0 );
    playout->addWidget( new QLabel( tr( "Library" ) ), 3, 0 );
    playout->addWidget( pcheckboxEnable, 0, 1 );
    playout->addWidget( pcheckboxForce, 1, 1 );
    playout->addWidget( pfileselectorTraceFile, 2, 1 );
    playout->addWidget( pfileselectorTraceLibrary, 3, 1 );

    QPushButton *ppushbuttonSetDefault = new QPushButton( "De&fault", this );
    ppushbuttonSetDefault->setToolTip( tr( "click this to restore the default values" ) );
    ppushbuttonSetDefault->setWhatsThis( tr( "Click this to restore the default values - the values used when unixODBC was first installed." ) );

    QPushButton *ppushbuttonApply = new QPushButton( "A&pply", this );
    ppushbuttonApply->setToolTip( tr( "click this to apply the current values" ) );
    ppushbuttonApply->setWhatsThis( tr( "Click this to save the current values. The values will be immediately effective for newly allocated environments and will remain in effect until different values are applied." ) );

    playout->addWidget( ppushbuttonSetDefault, 0, 2 );
    playout->addWidget( ppushbuttonApply, 1, 2 );

    connect( ppushbuttonSetDefault, SIGNAL(clicked()), SLOT(slotDefault()) );
    connect( ppushbuttonApply, SIGNAL(clicked()), SLOT(slotApply()) );

    playout->setRowStretch( 3, 10 );
    setLayout( playout );

    setWindowIcon( QPixmap( xpmTracing48 ) );
    setWindowTitle( tr( "Tracing" ) );

    loadData();
}

CTracing::~CTracing()
{
}

bool CTracing::saveData()
{
    if ( slotApply() )
        return true;

    int n = QMessageBox::question( this, tr( "Save..." ), tr( "Some data could not be saved. Click Discard if its ok to lose changes." ), QMessageBox::Cancel | QMessageBox::Discard, QMessageBox::Discard );
    if ( n == QMessageBox::Discard )
        return true;

    return false;
}

bool CTracing::loadData()
{
    slotDefault();

    char szTracing[10];
    char szForce[10];
    char szTracingFile[FILENAME_MAX];
    char szTracingLibrary[FILENAME_MAX];
    char szTracingLibraryDefault[FILENAME_MAX];

    SQLGetPrivateProfileString( "ODBC", "Trace", "No", szTracing, sizeof(szTracing), "odbcinst.ini" );
    if ( szTracing[0] == '1' || toupper( szTracing[0] ) == 'Y' || toupper( szTracing[0] ) == 'O' )
        pcheckboxEnable->setChecked( true );

    SQLGetPrivateProfileString( "ODBC", "ForceTrace", "No", szForce, sizeof(szForce), "odbcinst.ini" );
    if ( szForce[0] == '1' || toupper( szForce[0] ) == 'Y' || toupper( szForce[0] ) == 'O' )
        pcheckboxForce->setChecked( true );

    SQLGetPrivateProfileString( "ODBC", "TraceFile", "/tmp/Trace.txt", szTracingFile, sizeof(szTracingFile)-1, "odbcinst.ini" );
    pfileselectorTraceFile->setText( szTracingFile );

    if ( strlen( SHLIBEXT ) > 0 )
        sprintf( szTracingLibraryDefault, "libodbctrac%s", SHLIBEXT );
    else
        sprintf( szTracingLibraryDefault, "libodbctrac.so" );

    SQLGetPrivateProfileString( "ODBC", "TraceLibrary", szTracingLibraryDefault, szTracingLibrary, sizeof(szTracingLibrary)-1, "odbcinst.ini" );
    pfileselectorTraceLibrary->setText( szTracingLibrary );

    return true;
}

bool CTracing::slotApply()
{
    if ( !SQLWritePrivateProfileString( "ODBC", "Trace", ( pcheckboxEnable->isChecked() ? "1" : "0" ), "odbcinst.ini" ) )
    {
        CODBCInst::showErrors( this, tr( "Could not save options. You may not have the privileges to write odbcinst.ini." ) );
        return false;
    }

    SQLWritePrivateProfileString( "ODBC", "ForceTrace", ( pcheckboxForce->isChecked() ? "1" : "0" ), "odbcinst.ini" );
    SQLWritePrivateProfileString( "ODBC", "TraceFile", pfileselectorTraceFile->getText().toAscii().data(), "odbcinst.ini" );
    SQLWritePrivateProfileString( "ODBC", "TraceLibrary", pfileselectorTraceLibrary->getText().toAscii().data(), "odbcinst.ini" );

    return true;
}

bool CTracing::slotDefault()
{
    char szTracingLibraryDefault[FILENAME_MAX];

    if ( strlen( SHLIBEXT ) > 0 )
        sprintf( szTracingLibraryDefault, "libodbctrac%s", SHLIBEXT );
    else
        sprintf( szTracingLibraryDefault, "libodbctrac.so" );

    pcheckboxEnable->setChecked( false );
    pcheckboxForce->setChecked( false );
    pfileselectorTraceFile->setText( "/tmp/Trace.txt" );
    pfileselectorTraceLibrary->setText( szTracingLibraryDefault );
}

