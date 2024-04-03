/**************************************************
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 31.JAN.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 * Nick Gorham      - nick@easysoft.com
 **************************************************/

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qfiledialog.h>
#include <Qt/qfileinfo.h>
#else
#include <qfiledialog.h>
#endif
#include "CFileList.h"

#ifdef HAVE_STRNCASECMP
#ifdef HAVE_STRINGS
#include <strings.h>
#endif
#ifdef HAVE_STRING
#include <string.h>
#endif
#else
int strncasecmp( char *, char *, int );
#endif

extern int has_started;

#ifdef QT_V4LAYOUT
CFileList::CFileList( QWidget* parent, const char* name )
	: Q3ListView( parent, name )
#else
CFileList::CFileList( QWidget* parent, const char* name )
	: QListView( parent, name )
#endif
{
	resize( 310,230 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
#ifdef QT_V4LAYOUT
	setFocusPolicy( Qt::TabFocus );
	setBackgroundMode( Qt::PaletteBackground );
#else
	setFocusPolicy( QWidget::TabFocus );
	setBackgroundMode( QWidget::PaletteBackground );
#endif
	setFrameStyle( QFrame::Box | QFrame::Raised );
#ifdef QT_V4LAYOUT
	setResizePolicy( Q3ScrollView::Manual );
	setVScrollBarMode( Q3ScrollView::Auto );
	setHScrollBarMode( Q3ScrollView::Auto );
#else
	setResizePolicy( QScrollView::Manual );
	setVScrollBarMode( QScrollView::Auto );
	setHScrollBarMode( QScrollView::Auto );
#endif
	setTreeStepSize( 20 );
	setMultiSelection( FALSE );
	setAllColumnsShowFocus( TRUE );
	setItemMargin( 1 );
	setRootIsDecorated( FALSE );
	addColumn( "File Name", -1 );
#ifdef QT_V4LAYOUT
	setColumnWidthMode( 0, Q3ListView::Maximum );
#else
	setColumnWidthMode( 0, QListView::Maximum );
#endif
	setColumnAlignment( 0, 1 );
	addColumn( "Permissions", -1 );
#ifdef QT_V4LAYOUT
	setColumnWidthMode( 1, Q3ListView::Maximum );
#else
	setColumnWidthMode( 1, QListView::Maximum );
#endif
	setColumnAlignment( 1, 1 );
	addColumn( "Owner", -1 );
#ifdef QT_V4LAYOUT
	setColumnWidthMode( 2, Q3ListView::Maximum );
#else
	setColumnWidthMode( 2, QListView::Maximum );
#endif
	setColumnAlignment( 2, 1 );
	addColumn( "Group", -1 );
#ifdef QT_V4LAYOUT
	setColumnWidthMode( 3, Q3ListView::Maximum );
#else
	setColumnWidthMode( 3, QListView::Maximum );
#endif
	setColumnAlignment( 3, 1 );
	addColumn( "Size", -1 );
#ifdef QT_V4LAYOUT
	setColumnWidthMode( 4, Q3ListView::Maximum );
#else
	setColumnWidthMode( 4, QListView::Maximum );
#endif
	setColumnAlignment( 4, 1 );
}

CFileList::~CFileList()
{
}

void CFileList::Load( QString *in_cwd )
{
#ifdef QT_V4LAYOUT
	Q3ListViewItem	*pListViewItem;
#else
	QListViewItem	*pListViewItem;
#endif
    QDir d;
	
	clear();
    if ( in_cwd )
    {
        cwd = *in_cwd;
    }

    d.setFilter( QDir::Files | QDir::Hidden );
    d.cd( cwd );
    d.setNameFilter("*.dsn");

#ifdef QT_V4LAYOUT
    const QFileInfoList list = d.entryInfoList();
    QFileInfo fi;                          // pointer for traversing

	for ( int i = 0; i < list.size(); i ++ ) {
        QString perm, size;
        char driver[ 128 ];

		fi = list.at(i);

        perm = "-";
        perm += fi.permission( QFileInfo::ReadUser ) ? "r" : "-";
        perm += fi.permission( QFileInfo::WriteUser ) ? "w" : "-";
        perm += fi.permission( QFileInfo::ExeUser ) ? "x" : "-";
        perm += fi.permission( QFileInfo::ReadGroup ) ? "r" : "-";
        perm += fi.permission( QFileInfo::WriteGroup ) ? "w" : "-";
        perm += fi.permission( QFileInfo::ExeGroup ) ? "x" : "-";
        perm += fi.permission( QFileInfo::ReadOther ) ? "r" : "-";
        perm += fi.permission( QFileInfo::WriteOther ) ? "w" : "-";
        perm += fi.permission( QFileInfo::ExeOther ) ? "x" : "-";

        size.sprintf( "%d bytes", fi.size());

        pListViewItem = new Q3ListViewItem( this, fi.fileName(), perm, fi.owner(), fi.group(), size );
    }
#else
    const QFileInfoList *list = d.entryInfoList();
    QFileInfoListIterator it( *list );      // create list iterator
    QFileInfo *fi;                          // pointer for traversing

    while ( (fi=it.current()) ) 
    {           // for each file...

        QString perm, size;
        char driver[ 128 ];

        perm = "-";
        perm += fi->permission( QFileInfo::ReadUser ) ? "r" : "-";
        perm += fi->permission( QFileInfo::WriteUser ) ? "w" : "-";
        perm += fi->permission( QFileInfo::ExeUser ) ? "x" : "-";
        perm += fi->permission( QFileInfo::ReadGroup ) ? "r" : "-";
        perm += fi->permission( QFileInfo::WriteGroup ) ? "w" : "-";
        perm += fi->permission( QFileInfo::ExeGroup ) ? "x" : "-";
        perm += fi->permission( QFileInfo::ReadOther ) ? "r" : "-";
        perm += fi->permission( QFileInfo::WriteOther ) ? "w" : "-";
        perm += fi->permission( QFileInfo::ExeOther ) ? "x" : "-";

        size.sprintf( "%d bytes", fi->size());

        pListViewItem = new QListViewItem( this, fi->fileName(), perm, fi->owner(), fi->group(), size );

        ++it;                               // goto next list element
    }
#endif
}

QString CFileList::GetDir()
{
    return cwd;
}

void CFileList::NewDir()
{
    QString dir = cwd;
        
    cwd = QFileDialog::getExistingDirectory ( dir );
    Load();
}

void CFileList::Add()
{
	// odbc.ini INFO
    /*
	QString				qsDataSourceName		= "";
	QString				qsDataSourceDescription	= "";
    */

	QString				qsDataSourceDriver		= "";
	QString				qsDriverName			= "";
	CDriverPrompt	 *pDriverPrompt;
	CPropertiesFrame *pProperties;
	HODBCINSTPROPERTY	 hFirstProperty	= NULL;
	HODBCINSTPROPERTY	 hCurProperty	= NULL;
	DWORD				nErrorCode;
	char				szErrorMsg[101];
	QString				qsError					= "";
    int                 ret;

	pDriverPrompt = new CDriverPrompt( this );
	if ( pDriverPrompt->exec() )
	{
		qsDriverName		= pDriverPrompt->qsDriverName;
		qsDataSourceDriver	= qsDriverName;

		delete pDriverPrompt;

        //
        // can we call SQLDriverConnect
        //

		// GET PROPERTY LIST FROM DRIVER
		if ( ODBCINSTConstructProperties( (char*)qsDataSourceDriver.ascii(), &hFirstProperty ) != ODBCINST_SUCCESS )
		{
			qsError.sprintf( "Could not construct a property list for (%s)", qsDataSourceDriver.ascii() );
			QMessageBox::information( this, "ODBC Config",  qsError );
			return;
		}

		// ALLOW USER TO EDIT
		pProperties = new CPropertiesFrame( this, "Properties", hFirstProperty );
        pProperties->setCaption( "Data Source Properties (new)" );
		if ( pProperties->exec() )
		{
            char dir[ 256 ];
            sprintf( dir, "%s/%s", cwd.ascii(), hFirstProperty->szValue );

            ret = SQLWriteFileDSN( dir, "ODBC", NULL, NULL );
            if ( !ret )
            {
				qsError.sprintf( "Could not write to (%s)", dir );
				QMessageBox::information( this, "ODBC Config",  qsError );

                int i = 1;
				while ( SQLInstallerError( i++, &nErrorCode, szErrorMsg, 100, NULL ) == SQL_SUCCESS )
					QMessageBox::information( this, "ODBC Config",  szErrorMsg );

				return;
            }

			/* ADD ENTRIES; SECTION CREATED ON FIRST CALL */
			for ( hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
			{
                ret = SQLWriteFileDSN( dir, "ODBC", hCurProperty->szName, hCurProperty->szValue );
                if ( !ret )
                {
                    qsError.sprintf( "Could not write to (%s)", dir );
                    QMessageBox::information( this, "ODBC Config",  qsError );

                    int i = 1;
                    while ( SQLInstallerError( i++, &nErrorCode, szErrorMsg, 100, NULL ) == SQL_SUCCESS )
                        QMessageBox::information( this, "ODBC Config",  szErrorMsg );

                    return;
                }
			}
		}
		delete pProperties;
		ODBCINSTDestructProperties( &hFirstProperty );
	}
	else
		delete pDriverPrompt;

	// RELOAD (slow but safe)
	Load();
}

void CFileList::Edit()
{
	// odbc.ini INFO
	QString				qsDataSourceName		= "";
	QString				qsDataSourceDescription		= "";
	QString				qsDataSourceDriver		= "";
	// odbcinst.ini INFO
	QString				qsDriverFile			= "";
	QString				qsSetupFile				= "";
	QString				qsError					= "";

	CPropertiesFrame		*pProperties;
	HODBCINSTPROPERTY	hFirstProperty	= NULL;
	HODBCINSTPROPERTY	hCurProperty	= NULL;
#ifdef QT_V4LAYOUT
	Q3ListViewItem		*pListViewItem;
#else
	QListViewItem		*pListViewItem;
#endif

	char				szEntryNames[4096];
	char				szProperty[INI_MAX_PROPERTY_NAME+1];
	char				szValue[INI_MAX_PROPERTY_VALUE+1];
	
	DWORD				nErrorCode;
	char				szErrorMsg[101];
	char				szINI[FILENAME_MAX+1];
	int					nElement;	

    char dir[ 256 ];
    char szDriver[ 256 ];
    QString qsFileName;

	// HAS THE USER SELECTED SOMETHING
    pListViewItem = currentItem();
	if ( pListViewItem )
	{
		qsFileName		= pListViewItem->text( 0 );

        /*
		qsDataSourceDescription	= pListViewItem->text( 1 );
		qsDataSourceDriver		= pListViewItem->text( 2 );
        */
	}
	else
	{
		QMessageBox::information( this, "ODBC Config",  "Please select a Data Source from the list first" );
		return;
	}

    sprintf( dir, "%s/%s", cwd.ascii(), qsFileName.ascii());
    szDriver[ 0 ] = '\0';
    if ( !SQLReadFileDSN( dir, "ODBC", "DRIVER", szDriver, sizeof( szDriver ), NULL ) ||
            strlen( szDriver ) < 1 )
    {
        char szDsn[ 256 ];

        szDsn[ 0 ] = '\0';
        if ( SQLReadFileDSN( dir, "ODBC", "DSN", szDsn, sizeof( szDsn ), NULL ) &&
                strlen( szDsn ) >= 1 )
        {
            SQLSetConfigMode( ODBC_BOTH_DSN );
			SQLGetPrivateProfileString( szDsn, "Driver", "", szDriver, sizeof( szDriver ), "odbc.ini" );

            if ( strlen( szDriver ) < 1 )
            {
                QMessageBox::information( this, "ODBC Config",  "Unable to extract driver from FILE DSN" );
                return;
            }
        }
        else
        {
            QMessageBox::information( this, "ODBC Config",  "Unable to extract driver from FILE DSN" );
            return;
        }
    }

    //
    // can we call SQLDriverConnect
    //

	// GET PROPERTY LIST FROM DRIVER
	if ( ODBCINSTConstructProperties( szDriver, &hFirstProperty ) != ODBCINST_SUCCESS )
	{
		qsError.sprintf( "Could not construct a property list for (%s)", szDriver );
		QMessageBox::information( this, "ODBC Config",  qsError );

        int i = 1;
		while ( SQLInstallerError( i ++, &nErrorCode, szErrorMsg, 100, NULL ) == SQL_SUCCESS )
			QMessageBox::information( this, "ODBC Config",  szErrorMsg );

		return;
	}

	ODBCINSTSetProperty( hFirstProperty, "Name", (char*)qsFileName.ascii());

    for ( hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
    {
        char szAttr[ 256 ];

        szAttr[ 0 ] = '\0';
        if ( SQLReadFileDSN( dir, "ODBC", hCurProperty->szName, szAttr, sizeof( szAttr ), NULL ))
        {
		    ODBCINSTSetProperty( hFirstProperty, hCurProperty->szName, szAttr );
        }
    }

	// ALLOW USER TO EDIT
	pProperties = new CPropertiesFrame( this, "Properties", hFirstProperty );
	pProperties->setCaption( "Data Source Properties (edit)" );
	if ( pProperties->exec() )
	{
        int ret;

        ret = SQLWriteFileDSN( dir, "ODBC", NULL, NULL );
        if ( !ret )
        {
            qsError.sprintf( "Could not write to (%s)", dir );
            QMessageBox::information( this, "ODBC Config",  qsError );

            int i = 1;
            while ( SQLInstallerError( i++, &nErrorCode, szErrorMsg, 100, NULL ) == SQL_SUCCESS )
                QMessageBox::information( this, "ODBC Config",  szErrorMsg );

            return;
        }

        for ( hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
        {
            if ( !SQLWriteFileDSN( dir, "ODBC", hCurProperty->szName, hCurProperty->szValue ))
            {
                qsError.sprintf( "Could not write to file dsn (%s)", dir );
                QMessageBox::information( this, "ODBC Config",  qsError );

                int i = 1;
                while ( SQLInstallerError( i ++, &nErrorCode, szErrorMsg, 100, NULL ) == SQL_SUCCESS )
                    QMessageBox::information( this, "ODBC Config",  szErrorMsg );

                return;
            }
        }
	}
	delete pProperties;
	ODBCINSTDestructProperties( &hFirstProperty );

	// RELOAD (slow but safe)
	Load();
}

void CFileList::Delete()
{
#ifdef QT_V4LAYOUT
	Q3ListViewItem		*pListViewItem;
#else
	QListViewItem		*pListViewItem;
#endif
	char 				szINI[FILENAME_MAX+1];
	char 				*pDataSourceName;
	QString				qsError;
	DWORD				nErrorCode;
	char				szErrorMsg[FILENAME_MAX+1];

	// GET SELECT DATA SOURCE NAME
    pListViewItem = currentItem();
	if ( pListViewItem )
	{
		pDataSourceName = (char *)pListViewItem->text( 0 ).ascii();
	}
	else
	{
		QMessageBox::information( this, "ODBC Config",  "Please select a Data Source from the list first" );
		return;
	}

    char dir[ 256 ];
    sprintf( dir, "%s/%s", cwd.ascii(), pDataSourceName );

	// DELETE ENTIRE SECTION IF IT EXISTS (given NULL entry)
    if ( unlink( dir ))
    {
        QString msg;

        msg.sprintf( "Unable to unlink %s", dir );
        QMessageBox::information( this, "ODBC Config", msg );
    }
    else
    {
        QMessageBox::information( this, "ODBC Config", "Done!" );
    }
	
	// RELOAD (slow but safe)
	Load();
}


