/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#include <odbcinstext.h>

#include <QtGui>

#include "CDataSourceNamesFile.h"

#include "CDataSourceNamesFileModel.h"
#include "CFileSelector.h"

#include "DataSourceNameFile48.xpm"
#include "Set.xpm"

CDataSourceNamesFile::CDataSourceNamesFile( QWidget* pwidgetParent )
	: QWidget( pwidgetParent )
{
    QGridLayout *playout = new QGridLayout;

    // create the 'Default' and the 'Current' directory widgets...
    {
        QGridLayout *   playoutDirectories  = new QGridLayout;
        QToolButton *   ptoolbuttonDefault  = new QToolButton;
        QToolButton *   ptoolbuttonCurrent  = new QToolButton;
    
        plabelDefault = new QLabel;
        pFileSelector = new CFileSelector( CFileSelector::FileDSNDirectory, QString::null, false, false );

        plabelDefault->setWhatsThis( tr( "used system-wide to determine where to find file based data source names" ) );
        pFileSelector->setWhatsThis( tr( "current directory being shown" ) );

        ptoolbuttonDefault->setIcon( QIcon( xpmSet ) );
        ptoolbuttonCurrent->setIcon( QIcon( xpmSet ) );

        ptoolbuttonDefault->setToolTip( tr( "use the Current directory as the Default" ) );
        ptoolbuttonCurrent->setToolTip( tr( "change the Current directory" ) );

        playoutDirectories->addWidget( new QLabel( tr( "Default:" ) ), 0, 0 );
        playoutDirectories->addWidget( plabelDefault, 0, 1 );
        playoutDirectories->addWidget( ptoolbuttonDefault, 0, 2 );

        playoutDirectories->addWidget( new QLabel( tr( "Current:" ) ), 1, 0 );
        playoutDirectories->addWidget( pFileSelector, 1, 1 );
        playoutDirectories->addWidget( ptoolbuttonCurrent, 1, 2 );

        playout->addLayout( playoutDirectories, 0, 0 );

        // start with the default directory...
        {
            QString stringDefault = getDefault();
            pFileSelector->setText( stringDefault );
            plabelDefault->setText( stringDefault );
        }

        connect( ptoolbuttonDefault, SIGNAL(clicked()), this, SLOT(slotSetDefault()) );
        connect( ptoolbuttonCurrent, SIGNAL(clicked()), pFileSelector, SLOT(slotInvokeDialog()) );
        connect( pFileSelector, SIGNAL(signalChanged()), this, SLOT(slotLoad()) );
    }

    // create the model & view for the file data source names...
    {
        pDataSourceNamesFileModel   = new CDataSourceNamesFileModel;
        pListView                   = new QListView;
        pListView->setToolTip( tr( "list of file-based data source names" ) );
        pListView->setWhatsThis( tr( "This is a list of file-based data source names. File-based data source names are *.dsn files which exist on the file system. These files are read from a default directory at connect time." ) );
    
        pListView->setViewMode( QListView::IconMode );
        pListView->setModel( pDataSourceNamesFileModel );

        slotLoad();

        playout->addWidget( pListView, 1, 0 );
    }

    // create the push buttons to invoke add, edit, and delete of a selected file data source name...
    {
        QVBoxLayout *playoutButtons             = new QVBoxLayout;
        QPushButton *ppushbuttonAdd             = new QPushButton( tr( "A&dd..." ) );
        QPushButton *ppushbuttonConfigure       = new QPushButton( tr( "&Configure..." ) );
        QPushButton *ppushbuttonRemove          = new QPushButton( tr( "&Remove" ) );

        playoutButtons->addWidget( ppushbuttonAdd );
        playoutButtons->addWidget( ppushbuttonConfigure );
        playoutButtons->addWidget( ppushbuttonRemove );
        playoutButtons->addStretch( 10 );

        playout->addLayout( playoutButtons, 1, 1 );

        connect( ppushbuttonAdd, SIGNAL(clicked()), this, SLOT(slotAdd()) );
        connect( ppushbuttonConfigure, SIGNAL(clicked()), this, SLOT(slotEdit()) );
        connect( ppushbuttonRemove, SIGNAL(clicked()), this, SLOT(slotDelete()) );
    }

    setLayout( playout );

    setWindowIcon( QPixmap( xpmDataSourceNameFile48 ) );
    setWindowTitle( tr( "File Data Source Names" ) );
}

CDataSourceNamesFile::~CDataSourceNamesFile()
{
    delete pDataSourceNamesFileModel;
}

void CDataSourceNamesFile::slotLoad()
{
    pListView->setRootIndex( pDataSourceNamesFileModel->index( pFileSelector->getText() ) );
}

void CDataSourceNamesFile::slotAdd()
{
    pDataSourceNamesFileModel->addDataSourceName( pFileSelector->getText() );
}

void CDataSourceNamesFile::slotEdit()
{
    QModelIndexList listSelectedIndexes = pListView->selectionModel()->selectedIndexes();
    if ( !listSelectedIndexes.count() )
    {
        QMessageBox::warning( this, tr( "ODBC Administrator" ),  tr( "Please select a Data Source Name from the list" ) );
        return;
    }
    pDataSourceNamesFileModel->editDataSourceName( listSelectedIndexes.at( 0 ) );
}

void CDataSourceNamesFile::slotDelete()
{
    QModelIndexList listSelectedIndexes = pListView->selectionModel()->selectedIndexes();
    if ( !listSelectedIndexes.count() )
    {
        QMessageBox::warning( this, tr( "ODBC Administrator" ),  tr( "Please select a Data Source Name from the list" ) );
        return;
    }
    pDataSourceNamesFileModel->deleteDataSourceName( listSelectedIndexes.at( 0 ) );
}

void CDataSourceNamesFile::slotSetDefault()
{
    if ( SQLWritePrivateProfileString( "ODBC", "FileDSNPath", pFileSelector->getText().toAscii().constData(), "odbcinst.ini" ) == SQL_FALSE )
    {
        CODBCInst::showErrors( this, tr( "Failed to set default. You may lack the elevated privileges usually required to do this." ) );
        return;
    }

    plabelDefault->setText( getDefault() );
}

QString CDataSourceNamesFile::getDefault()
{
    char szDirectory[FILENAME_MAX];

    szDirectory[0] = '\0';
    _odbcinst_FileINI( szDirectory );

    return QString( szDirectory );
}

