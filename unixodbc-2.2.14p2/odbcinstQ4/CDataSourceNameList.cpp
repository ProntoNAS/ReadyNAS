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

#include "CDataSourceNameList.h"

#include "CDriverPrompt.h"
#include "CPropertiesDialog.h"

#define ODBC_HELP_DSN_NAME "*Unique* DSN name. This is the name you use when using ODBC with applications such as StarOffice. Try to keep unusual characters and spaces out of the name."
#define ODBC_HELP_DSN_DESC "DSN description. A long, perhaps more meaningfull name."
#define ODBC_HELP_DSN_UNKNOWN "No help for this DSN property. Please check with the vendor of the driver... perhaps their web site"

CDataSourceNameList::CDataSourceNameList( QWidget *pwidgetParent, int nSource )
	: QTableWidget( pwidgetParent )
{
    this->nSource = nSource;

    setToolTip( tr( "list of data source names" ) );
    setWhatsThis( tr( "This shows a list of the data source names. Data source names (DSN's) are a convenient way for an application to connect to a data source as the options can be specified in advance, saved, and then used at connect-time by simply referring to the name. Applications which use a DSN to connect also provide a means for the User to easily specify a different data source - simply create the DSN with a same/different driver and options." ) );
    QStringList stringlist;
    setColumnCount( 3 );
    setHorizontalHeaderLabels( stringlist << "Name" << "Description" << "Driver" );
    setSelectionBehavior( QAbstractItemView::SelectRows );
    setSelectionMode( QAbstractItemView::SingleSelection );
    verticalHeader()->setVisible( false );

    connect( this, SIGNAL(itemDoubleClicked( QTableWidgetItem * )), this, SLOT(slotDoubleClick( QTableWidgetItem * )));

    slotLoad();
}

CDataSourceNameList::~CDataSourceNameList()
{
}

void CDataSourceNameList::slotAdd()
{
    // odbc.ini INFO
    QString				stringDataSourceName		= "";
    QString				stringDataSourceDescription	= "";
    QString				stringDataSourceDriver		= "";
    // odbcinst.ini INFO
    QString				stringDriverName			= "";
    QString				stringDriverDescription		= "";
    QString				stringDriverFile			= "";
    QString				stringSetupFile				= "";

    CDriverPrompt	 *  pDriverPrompt;
    CPropertiesDialog * pProperties;
    HODBCINSTPROPERTY	hFirstProperty	= NULL;
    HODBCINSTPROPERTY	hCurProperty	= NULL;
    char				szINI[FILENAME_MAX+1];

    pDriverPrompt = new CDriverPrompt( this );
    if ( pDriverPrompt->exec() )
    {
        stringDriverName		= pDriverPrompt->getFriendlyName();
        stringDriverDescription	= pDriverPrompt->getDescription();
        stringDriverFile		= pDriverPrompt->getDriver();
        stringSetupFile			= pDriverPrompt->getSetup();
        stringDataSourceDriver	= stringDriverName;
        delete pDriverPrompt;

        //
        // can we call SQLConfigDataSource ?
        //

        {
            int mode;

            if( nSource == ODBC_USER_DSN )
            {
                sprintf( szINI, "~/.odbc.ini" );
                mode = ODBC_ADD_DSN;
            }
            else
            {
				char b1[ 256 ];
                sprintf( szINI, "%s/odbc.ini", odbcinst_system_file_path( b1 ));
                mode = ODBC_ADD_SYS_DSN;
            }
            if ( SQLConfigDataSource( (HWND)1, mode, stringDataSourceDriver.toAscii().data(), "" ) )
            {
                slotLoad();
                return;
            }
        }

        // GET PROPERTY LIST FROM DRIVER
        if ( ODBCINSTConstructProperties( stringDataSourceDriver.toAscii().data(), &hFirstProperty ) != ODBCINST_SUCCESS )
        {
            QMessageBox::information( this, tr( "ODBC Administrator" ), QString( "Could not construct a property list for (%1)" ).arg( stringDataSourceDriver ) );
            return;
        }

        // ALLOW USER TO EDIT
        pProperties = new CPropertiesDialog( this, hFirstProperty );
        pProperties->setWindowTitle( tr( "Data Source Properties (new)" ) );
        if ( pProperties->exec() )
        {
            /* DELETE ENTIRE SECTION IF IT EXISTS (no entry given) */
            SQLSetConfigMode( nSource );
            if ( SQLWritePrivateProfileString( hFirstProperty->szValue, NULL, NULL, "odbc.ini" ) == FALSE )
            {
                SQLSetConfigMode( ODBC_BOTH_DSN );
                delete pProperties;
                ODBCINSTDestructProperties( &hFirstProperty );
                CODBCInst::showErrors( this, QString( "Could not write to (%1)" ).arg( szINI ) );
                return;
            }
            stringDataSourceName = hFirstProperty->szValue;
            /* ADD ENTRIES; SECTION CREATED ON FIRST CALL */
            QString stringName;
            for ( hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
            {
                stringName = hCurProperty->szName;
                if ( stringName.toUpper() == "DESCRIPTION" )
                    stringDataSourceDescription = hCurProperty->szValue;

                SQLWritePrivateProfileString( hFirstProperty->szValue, hCurProperty->szName, hCurProperty->szValue, "odbc.ini" );
            }
            SQLSetConfigMode( ODBC_BOTH_DSN );
        }
        delete pProperties;
        ODBCINSTDestructProperties( &hFirstProperty );
    }
    else
        delete pDriverPrompt;

    // RELOAD (slow but safe)
    slotLoad();
}

void CDataSourceNameList::slotEdit()
{
	// odbc.ini INFO
	QString				stringDataSourceName		= "";
	QString				stringDataSourceDescription	= "";
	QString				stringDataSourceDriver		= "";
	// odbcinst.ini INFO
	QString				stringDriverFile			= "";
	QString				stringSetupFile				= "";
	QString				stringError					= "";

	CPropertiesDialog *  pProperties;
	HODBCINSTPROPERTY	hFirstProperty	= NULL;
	HODBCINSTPROPERTY	hCurProperty	= NULL;

	char				szEntryNames[4096];
	char				szProperty[INI_MAX_PROPERTY_NAME+1];
	char				szValue[INI_MAX_PROPERTY_VALUE+1];
	
	DWORD				nErrorCode;
	char				szErrorMsg[101];
	char				szINI[FILENAME_MAX+1];
	int					nElement;	

	// HAS THE USER SELECTED SOMETHING
    QList<QTableWidgetItem*> listSelectedItems = selectedItems();
    if ( listSelectedItems.count() )
    {
        int nRow = row( listSelectedItems.at( 0 )  );
        stringDataSourceName		= item( nRow, 0 )->text();
        stringDataSourceDescription	= item( nRow, 1 )->text();
        stringDataSourceDriver		= item( nRow, 2 )->text();
    }
    else
    {
        QMessageBox::information( this, tr( "ODBC Administrator" ),  "Please select a Data Source from the list first" );
        return;
    }

    //
    // can we call SQLConfigDataSource ?
    //

    {
        char attr[ 128 ];
        int mode;

        sprintf( attr, "DSN=%s", stringDataSourceName.toAscii().data() );

		/*
		 * add extra null
		 */

		attr[ strlen( attr ) ] = '\0';

        if( nSource == ODBC_USER_DSN )
        {
            sprintf( szINI, "~/.odbc.ini" );
            mode = ODBC_CONFIG_DSN;
        }
        else
        {
			char b1[ 256 ];
            sprintf( szINI, "%s/odbc.ini", odbcinst_system_file_path( b1 ));
            mode = ODBC_CONFIG_SYS_DSN;
        }

        if ( SQLConfigDataSource(( HWND ) 1, mode,
            stringDataSourceDriver.toAscii().data(), attr ))
        {
			SQLSetConfigMode( ODBC_BOTH_DSN );
            slotLoad();
            return;
        }
        SQLSetConfigMode( ODBC_BOTH_DSN );
    }

	// GET PROPERTY LIST FROM DRIVER
	if ( ODBCINSTConstructProperties( (char*) stringDataSourceDriver.toAscii().data(), &hFirstProperty ) != ODBCINST_SUCCESS )
	{
        CODBCInst::showErrors( this, QString( "Could not construct a property list for (%1)" ).arg( stringDataSourceDriver ) );
		return;
	}

	// COPY EXISTING VALUES INTO PROPERTIES LIST
	SQLSetConfigMode( nSource );
	ODBCINSTSetProperty( hFirstProperty, "Name", stringDataSourceName.toAscii().data() );
    memset( szEntryNames, 0, sizeof( szEntryNames ));
	SQLGetPrivateProfileString( stringDataSourceName.toAscii().data(), NULL, NULL, szEntryNames, 4090, "odbc.ini" ); // GET ALL ENTRY NAMES FOR THE SELCTED DATA SOURCE
	for ( nElement = 0; iniElement( szEntryNames, '\0', '\0', nElement, szProperty, 1000 ) == INI_SUCCESS ; nElement++ )
	{
		SQLGetPrivateProfileString( stringDataSourceName.toAscii().data(), szProperty, "", szValue, INI_MAX_PROPERTY_VALUE, szINI ); // GET VALUE FOR EACH ENTRY

		if ( ODBCINSTSetProperty( hFirstProperty, szProperty, szValue ) == ODBCINST_ERROR )
        {
		    ODBCINSTAddProperty( hFirstProperty, szProperty, szValue );
        }
	}
	SQLSetConfigMode( ODBC_BOTH_DSN );

	// ALLOW USER TO EDIT
	pProperties = new CPropertiesDialog( this, hFirstProperty );
	pProperties->setWindowTitle( tr( "Data Source Properties (edit)" ) );
	if ( pProperties->exec() )
	{
		SQLSetConfigMode( nSource );
		/* DELETE ENTIRE SECTION IF IT EXISTS (given NULL entry) */
		if ( SQLWritePrivateProfileString( stringDataSourceName.toAscii().data(), NULL, NULL, "odbc.ini" ) == FALSE )
		{
			SQLSetConfigMode( ODBC_BOTH_DSN );
			delete pProperties;
			ODBCINSTDestructProperties( &hFirstProperty );
			CODBCInst::showErrors( this, QString( "Could not write to (%1)" ).arg( szINI ) );
			return;
		}
		stringDataSourceName = hFirstProperty->szValue;
		/* ADD ENTRIES; SECTION CREATED ON FIRST CALL */
        QString stringName;
		for ( hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
		{
            stringName = hCurProperty->szName;
			if ( stringName.toUpper() == "DESCRIPTION" )
				stringDataSourceDescription = hCurProperty->szValue;

			SQLWritePrivateProfileString( hFirstProperty->szValue, hCurProperty->szName, hCurProperty->szValue, szINI );
		}
		SQLSetConfigMode( ODBC_BOTH_DSN );
	}
	delete pProperties;
	ODBCINSTDestructProperties( &hFirstProperty );

	// RELOAD (slow but safe)
	slotLoad();
}

void CDataSourceNameList::slotDelete()
{
	char 	szINI[FILENAME_MAX+1];
	char *  pDataSourceName;
	QString	stringError;
	DWORD	nErrorCode;
	char	szErrorMsg[FILENAME_MAX+1];

	// GET SELECT DATA SOURCE NAME
    QList<QTableWidgetItem*> listSelectedItems = selectedItems();
    if ( listSelectedItems.count() )
	{
        int nRow = row( listSelectedItems.at( 0 ) );
		pDataSourceName = (char *)item( nRow, 0 )->text().toAscii().data();
	}
	else
	{
		QMessageBox::information( this, tr( "ODBC Administrator" ),  "Please select a Data Source from the list first" );
		return;
	}

	// DELETE ENTIRE SECTION IF IT EXISTS (given NULL entry)
	SQLSetConfigMode( nSource );
	if ( SQLWritePrivateProfileString( pDataSourceName, NULL, NULL, szINI ) == FALSE )
		CODBCInst::showErrors( this, QString( "Could not write property list for (%1)" ).arg( pDataSourceName ) );

	SQLSetConfigMode( ODBC_BOTH_DSN );
	
	// RELOAD (slow but safe)
	slotLoad();
}

void CDataSourceNameList::slotLoad()
{
	QString         stringError;
	DWORD	        nErrorCode;
	char   	        szErrorMsg[101];
	char	        szINI[FILENAME_MAX+1];
	char	        szSectionNames[4096];
	char	        szSectionName[INI_MAX_OBJECT_NAME+1];
	char	        szDriver[INI_MAX_PROPERTY_VALUE+1];
	char	        szDescription[INI_MAX_PROPERTY_VALUE+1];
	int		        nElement;

    setRowCount( 0 );

    // GET SECTION NAMES (Data Sources)
	strcpy( szINI, "odbc.ini" );
	memset( szSectionNames, 0, sizeof(szSectionNames) );
	SQLSetConfigMode( nSource );
	if ( SQLGetPrivateProfileString( NULL, NULL, NULL, szSectionNames, 4090, "odbc.ini" ) >= 0 )
	{
        for ( nElement = 0; iniElement( szSectionNames, '\0', '\0', nElement, szSectionName, INI_MAX_OBJECT_NAME ) == INI_SUCCESS ; nElement++ )
		{
			// GET DRIVER AND DESCRIPTION
			szDriver[0]		= '\0';
			szDescription[0]	= '\0';
#ifdef PLATFORM64
			SQLGetPrivateProfileString( szSectionName, "Driver64", "", szDriver, INI_MAX_PROPERTY_VALUE, "odbc.ini" );
			if ( szDriver[ 0 ] == '\0' )
			{
				SQLGetPrivateProfileString( szSectionName, "Driver", "", szDriver, INI_MAX_PROPERTY_VALUE, "odbc.ini" );
			}
#else
			SQLGetPrivateProfileString( szSectionName, "Driver", "", szDriver, INI_MAX_PROPERTY_VALUE, "odbc.ini" );
#endif
			SQLGetPrivateProfileString( szSectionName, "Description", "", szDescription, INI_MAX_PROPERTY_VALUE, "odbc.ini" );

            QTableWidgetItem *ptablewidgetitem;
            setRowCount( nElement + 1 );

            ptablewidgetitem = new QTableWidgetItem( szSectionName );
            ptablewidgetitem->setFlags( Qt::ItemIsSelectable );
            setItem( nElement, 0, ptablewidgetitem );

            ptablewidgetitem = new QTableWidgetItem( szDescription );
            ptablewidgetitem->setFlags( Qt::ItemIsSelectable );
            setItem( nElement, 1, ptablewidgetitem );

            ptablewidgetitem = new QTableWidgetItem( szDriver );
            ptablewidgetitem->setFlags( Qt::ItemIsSelectable );
            setItem( nElement, 2, ptablewidgetitem );
		}
		SQLSetConfigMode( ODBC_BOTH_DSN );
	}
	else
	{
        SQLSetConfigMode( ODBC_BOTH_DSN );
		CODBCInst::showErrors( this, QString( "Could not load %1" ).arg( szINI ) );
	}
}

void CDataSourceNameList::slotDoubleClick( QTableWidgetItem * )
{
    slotEdit();
}

