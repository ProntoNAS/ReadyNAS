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

#include "CDataSourceNamesFileModel.h"

#include "CDriverPrompt.h"
#include "CPropertiesDialog.h"

CDataSourceNamesFileModel::CDataSourceNamesFileModel( QObject *pobjectParent )
    : QDirModel( QStringList( "*.dsn" ), QDir::Files, QDir::Name, pobjectParent )
{
}

CDataSourceNamesFileModel::~CDataSourceNamesFileModel()
{
}

/*! 
 * \brief   Adds a file-based Data Source Name to the specified directory.
 * 
 *          Call this method to invoke a process of interacting with the User
 *          to create a new file-based Data Source Name.
 *
 *          This relies upon the selected driver having a viable setup library. By
 *          viable we mean that it implements ODBCINSTGetProperties. 
 *
 * \todo    Need to look at supporting driver setups which provide their own UI for this.
 *          
 * \param   stringDirectory Input. The directory where we want to create the Data Source Name.
 * 
 * \return  bool
 * \retval  true    It was created.
 * \retval  false   Failed to create it (or User simply cancelled).
 *
 */
bool CDataSourceNamesFileModel::addDataSourceName( const QString &stringDirectory )
{
    QString             stringDriverName;
    HODBCINSTPROPERTY   hFirstProperty  = NULL;

    // ask for a driver...
    {
        CDriverPrompt driverprompt;
        if ( !driverprompt.exec() )
            return false;
        stringDriverName = driverprompt.getFriendlyName();
    }

    // get dsn properties from driver setup...
    if ( ODBCINSTConstructProperties( stringDriverName.toAscii().data(), &hFirstProperty ) != ODBCINST_SUCCESS )
    {
        QMessageBox::warning( 0, tr( "ODBC Administrator" ),  QString( "Could not construct a property list for (%1).\nThis is probably because a viable driver setup library could not be found/used.\nYou may want to try configuring the driver to use a generic setup library." ).arg( stringDriverName ) );
        return false;
    }

    // allow properties to be edited...
    {
        CPropertiesDialog propertiesdialog( 0, hFirstProperty );
        propertiesdialog.setWindowTitle( tr( "Data Source Properties (new)" ) );
        if ( !propertiesdialog.exec() )
        {
            ODBCINSTDestructProperties( &hFirstProperty );
            return false;
        }
    }

    // save properties...
    {
        HODBCINSTPROPERTY   hCurProperty            = NULL;
        QString             stringFileNameAndPath   = QString( "%1/%2.dsn" ).arg( stringDirectory ).arg( hFirstProperty->szValue );

        // create the file with a section called "ODBC"...
        if ( !SQLWriteFileDSN( stringFileNameAndPath.toAscii().data(), "ODBC", NULL, NULL ) )
        {
            CODBCInst::showErrors( 0, QString( "Could not write to (%1)" ).arg( stringFileNameAndPath ) );
            ODBCINSTDestructProperties( &hFirstProperty );
            return false;
        }

        // add all of the properties...
        for ( hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
        {
            if ( !SQLWriteFileDSN( stringFileNameAndPath.toAscii().data(), "ODBC", hCurProperty->szName, hCurProperty->szValue ) )
            {
                CODBCInst::showErrors( 0, QString( "Could not write to (%1)" ).arg( stringFileNameAndPath ) );
                ODBCINSTDestructProperties( &hFirstProperty );
                return false;
            }
        }
    }

    // free memory used by properties...
    ODBCINSTDestructProperties( &hFirstProperty );

    // refresh file(s) display...
    refresh( index( stringDirectory ) );

    return true;
}

/*! 
 * \brief   Edits Data Source Name specified by modelindex.
 * 
 *          Call this method to invoke a process of interacting with the User
 *          to edit an existing file-based Data Source Name.
 *
 *          This relies upon the selected driver having a viable setup library. By
 *          viable we mean that it implements ODBCINSTGetProperties. 
 *
 * \todo    Need to look at supporting driver setups which provide their own UI for this.
 *          
 * \param   modelindex Input. Index to a viable Data Source Name file.
 * 
 * \return  bool
 * \retval  true    Any changes were saved.
 * \retval  false   Failed to edit or save changes (or User simply cancelled).
 *
 */
bool CDataSourceNamesFileModel::editDataSourceName( const QModelIndex &modelindex )
{
    HODBCINSTPROPERTY   hFirstProperty  = NULL;
    char                szDriver[FILENAME_MAX];
    QFileInfo           fileinfo        = fileInfo( modelindex );

    // find out which driver the DSN is using...
    szDriver[0] = '\0';
    if ( !SQLReadFileDSN( fileinfo.absoluteFilePath().toAscii().data(), "ODBC", "DRIVER", szDriver, FILENAME_MAX, NULL ) || strlen( szDriver ) < 1 )
    {
        // the driver is not specified in the DSN file so perhaps we are simply a reference to a regular DSN...
        // \note me thinks this is creative use of the DSN property with limited practical use
        char szDsn[FILENAME_MAX];

        szDsn[0] = '\0';
        if ( SQLReadFileDSN( fileinfo.absoluteFilePath().toAscii().data(), "ODBC", "DSN", szDsn, FILENAME_MAX, NULL ) && strlen( szDsn ) >= 1 )
        {
            UWORD n;
            SQLGetConfigMode( &n ); 
            SQLSetConfigMode( ODBC_BOTH_DSN );
            SQLGetPrivateProfileString( szDsn, "Driver", "", szDriver, sizeof( szDriver ), "odbc.ini" );
            SQLSetConfigMode( n );

            if ( strlen( szDriver ) < 1 )
            {
                CODBCInst::showErrors( 0, tr( "Unable to extract driver from FILE DSN" ) );
                return false;
            }
        }
        else
        {
            CODBCInst::showErrors( 0, tr( "Unable to extract driver from FILE DSN" ) );
            return false;
        }
    }

    // get properties from drivers setup library...
    if ( ODBCINSTConstructProperties( szDriver, &hFirstProperty ) != ODBCINST_SUCCESS )
    {
        CODBCInst::showErrors( 0, QString( "Could not construct a property list for (%1)" ).arg( szDriver ) );
        return false;
    }

    // apply property values from file to properties list...
    {
        // set the name property value...
        ODBCINSTSetProperty( hFirstProperty, "Name", fileinfo.baseName().toAscii().data() );

        // set other property values...
        for ( HODBCINSTPROPERTY hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
        {
            char szAttr[ 256 ];

            szAttr[ 0 ] = '\0';
            if ( SQLReadFileDSN( fileinfo.absoluteFilePath().toAscii().data(), "ODBC", hCurProperty->szName, szAttr, sizeof( szAttr ), NULL ))
            {
                ODBCINSTSetProperty( hFirstProperty, hCurProperty->szName, szAttr );
            }
        }
    }

    // present properties for editing...
    {
        CPropertiesDialog propertiesdialog( 0, hFirstProperty );
        propertiesdialog.setWindowTitle( "Data Source Properties (edit)" );
        if ( !propertiesdialog.exec() )
        {
            ODBCINSTDestructProperties( &hFirstProperty );
            return false;
        }
    }

    // save changes...
    {
        if ( !SQLWriteFileDSN( fileinfo.absoluteFilePath().toAscii().data(), "ODBC", NULL, NULL ) )
        {
            CODBCInst::showErrors( 0, QString( "Could not write to (%1)" ).arg( fileinfo.absoluteFilePath() ) );
            ODBCINSTDestructProperties( &hFirstProperty );
            return false;
        }
    
        for ( HODBCINSTPROPERTY hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
        {
            if ( !SQLWriteFileDSN( fileinfo.absoluteFilePath().toAscii().data(), "ODBC", hCurProperty->szName, hCurProperty->szValue ))
            {
                CODBCInst::showErrors( 0, QString( "Could not write to file DSN (%1)" ).arg( fileinfo.absoluteFilePath() ) );
                ODBCINSTDestructProperties( &hFirstProperty );
                return false;
            }
        }
    }

    // free memory used by properties list...
    ODBCINSTDestructProperties( &hFirstProperty );

    // refresh file(s) display in case we have created a new file (if Name was changed)...
    refresh( index( fileinfo.absolutePath() ) );

    return true;
}

/*! 
 * \brief   Deletes Data Source Name specified by modelindex.
 * 
 *          Call this method to permantly remove the Data Source Name file.
 *
 *          This relies upon the selected driver having a viable setup library. By
 *          viable we mean that it implements ODBCINSTGetProperties. 
 *
 * \todo    Need to look at supporting driver setups which provide their own UI for this.
 *          
 * \param   modelindex Input. Index to a viable Data Source Name file.
 * 
 * \return  bool
 * \retval  true    File deleted.
 * \retval  false   Failed delete file - probably a privs issue.
 *
 */
bool CDataSourceNamesFileModel::deleteDataSourceName( const QModelIndex &modelindex )
{
    // \todo we want to refresh with the current root but doing it this way undermines the usefullness of the model
    QString stringRoot = fileInfo( modelindex ).absolutePath();

    // delete the file...
    if ( unlink( filePath( modelindex ).toAscii().data() ) )
    {
        QMessageBox::critical( 0, tr( "ODBC Administrator" ), QString( "Unable to unlink %1" ).arg( filePath( modelindex ) ) );
        return false;
    }

    // refresh file(s) display...
    refresh( index( stringRoot ) );

    return true;
}



