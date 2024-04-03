/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#include "CODBCInst.h"

#include <QObject>

CDSNWizardData::CDSNWizardData( const QString &stringDataSourceName ) 
{ 
    this->stringDataSourceName = stringDataSourceName; 
    nType           = TypeUser; 
    hFirstProperty  = 0; 
}

CDSNWizardData::~CDSNWizardData() 
{ 
    if ( hFirstProperty ) 
        ODBCINSTDestructProperties( &hFirstProperty ); 
}

/*! 
 * \brief   Save Data Source Name.
 *
 *          Call this to save User, System and File DSN's based upon a property list.
 * 
 * \param   pwidgetParent   Input. Widget to use as parent for messages. Null is ok.
 * \param   hFirstProperty  Input. First property in NULL terminated property list.
 * \param   nType           Input. Type of DSN.
 * \param   stringIni       Input. Default is QString::null.
 *
 *                          \li User and System DSN. This can be a QString::null to use 
 *                          default location or it can be a fully qualified path and name.
 *                          \li File DSN. This can be a QString::null to use default 
 *                          location or it can be an abs directory. In either case the file
 *                          name itself will be derived from the Name property.
 * 
 * \return  bool
 * \retval  true    Hey - it worked :)
 * \retval  false   Call failed for some reason. Error message(s) is shown in message dialog.
 */
bool CODBCInst::saveDataSourceName( QWidget *pwidgetParent, HODBCINSTPROPERTY hFirstProperty, CDSNWizardData::Type nType, const QString &stringIni )
{
    // sanity checks...
    if ( !hFirstProperty )
    {
        QMessageBox::critical( pwidgetParent, QObject::tr( "ODBC Administrator" ),  QObject::tr( "Programmer error: Uninitialized property list." ) );
        return false;
    }
    if ( (QString( hFirstProperty->szValue )).simplified().isEmpty() )
    {
        QMessageBox::critical( pwidgetParent, QObject::tr( "ODBC Administrator" ),  QObject::tr( "Missing 'Name' property value." ) );
        return false;
    }

    // init
    UWORD   nConfigMode     = ODBC_BOTH_DSN;
    bool    bReturn         = false;
    QString stringFileName  = stringIni;

    switch ( nType )
    {
        case CDSNWizardData::TypeUser:
            SQLGetConfigMode( &nConfigMode );
            SQLSetConfigMode( ODBC_USER_DSN );
            if ( stringFileName.isEmpty() ) stringFileName = "odbc.ini";
            break;
        case CDSNWizardData::TypeSystem:
            SQLGetConfigMode( &nConfigMode );
            SQLSetConfigMode( ODBC_SYSTEM_DSN );
            if ( stringFileName.isEmpty() ) stringFileName = "odbc.ini";
            break;
        case CDSNWizardData::TypeFile:
            // should we calc the dir and file name (do not allow the special purpose names)...
            if ( stringFileName.isEmpty() || stringFileName.at( 0 ) != QChar( '/' ) )
            {
                char szDirectory[FILENAME_MAX];

                szDirectory[0] = '\0';
                _odbcinst_FileINI( szDirectory );
                stringFileName = QString( "%1/%2.dsn" ).arg( szDirectory ).arg( hFirstProperty->szValue );
            }
            else if ( stringFileName.at( 0 ) == QChar( '/' ) )
                stringFileName = QString( "%1/%2.dsn" ).arg( stringFileName ).arg( hFirstProperty->szValue );
            else
            {
                QMessageBox::critical( pwidgetParent, QObject::tr( "ODBC Administrator" ),  QObject::tr( "File DSN directory expected but seems to be a name or file name." ) );
                return false;
            }
            break;
        default:
            QMessageBox::critical( pwidgetParent, QObject::tr( "ODBC Administrator" ),  QObject::tr( "Unhandled Data Source Name Type" ) );
            return false;
    }

    // save...
    if ( nType == CDSNWizardData::TypeFile )
    {
        // create the file with a section called "ODBC"...
        if ( !SQLWriteFileDSN( stringFileName.toAscii().data(), "ODBC", NULL, NULL ) )
        {
            showErrors( 0, QString( "Could not write to (%1)" ).arg( stringFileName ) );
            goto CODBCInstExit1;
        }
        // add all of the properties...
        for ( HODBCINSTPROPERTY hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
        {
            if ( !SQLWriteFileDSN( stringFileName.toAscii().data(), "ODBC", hCurProperty->szName, hCurProperty->szValue ) )
            {
                showErrors( 0, QString( "Could not write to (%1)" ).arg( stringFileName ) );
                goto CODBCInstExit1;
            }
        }
    }
    else
    {
        // create a section called using Name property (always 1st property)...
        if ( SQLWritePrivateProfileString( hFirstProperty->szValue, NULL, NULL, stringFileName.toAscii().data() ) == FALSE )
        {
            showErrors( pwidgetParent, QString( QObject::tr( "Could not write to (%1). You may need additional system privileges." ) ).arg( stringFileName ) );
            goto CODBCInstExit1;
        }
        // add all of the properties...
        for ( HODBCINSTPROPERTY hCurProperty = hFirstProperty->pNext; hCurProperty != NULL; hCurProperty = hCurProperty->pNext )
        {
            SQLWritePrivateProfileString( hFirstProperty->szValue, hCurProperty->szName, hCurProperty->szValue, stringFileName.toAscii().data() );
        }
    }

    bReturn = true;

    // fini
CODBCInstExit1:
    if ( nType != CDSNWizardData::TypeFile )
        SQLSetConfigMode( nConfigMode );

    return bReturn;
}

int CODBCInst::showErrors( QWidget *pwidgetParent, const QString &stringConsequence )
{
    DWORD   nCode;
    char    szMessage[FILENAME_MAX+1];
    int     nMessage = 1;

    for ( ; nMessage <= 10; nMessage++ )
    {    
        if ( SQLInstallerError( nMessage, &nCode, szMessage, FILENAME_MAX, NULL ) != SQL_SUCCESS )
            break;
        QMessageBox::critical( pwidgetParent, QObject::tr( "ODBC Administrator" ),  szMessage );
    }

    if ( !stringConsequence.isEmpty() )
        QMessageBox::warning( pwidgetParent, QObject::tr( "ODBC Administrator" ),  stringConsequence );

    return (nMessage - 1);
}

