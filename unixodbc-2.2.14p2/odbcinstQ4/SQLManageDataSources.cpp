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

#include <QApplication>

#include "CODBCConfig.h"
#include "CDSNWizard.h"

/*! 
 * \brief   Manage Data Sources
 * 
 *          SQLManageDataSources done using Qt4. Here is where we enter into Qt4 'world'.
 * 
 * \param   hWnd
 * 
 * \return  BOOL
 */
static BOOL QT4ManageDataSources( HWND hWnd )
{
    /*
     * Here, we handle case where a non QT app has called us. Our approach here is a bit different 
     * than what we used for Qt3 because we were getting segfault upon app exit. Here; we instantiate
     * with a 'new' and NOT 'delete' it. Its a work around.
     */ 
    if ( !qApp )
    {
        int argc = 1;
        char *argv[] = { "odbcinstQ4", NULL };

        QApplication *pApplication = new QApplication( argc, argv );
        QCoreApplication::setOrganizationName("unixODBC");
        QCoreApplication::setOrganizationDomain("unixodbc.org");
        QCoreApplication::setApplicationName("ODBC Administrator");
    }

    // missing parent widget? Use desktop...
    QWidget *pWidget = (QWidget*)hWnd;
    if ( !pWidget )
        pWidget = qApp->desktop();

    // show dialog...
	CODBCConfig odbcconfig( pWidget );
	if ( odbcconfig.exec() == QDialog::Accepted )
        return true;

    return false;
}

/*! 
 * \brief   Create Data Source
 * 
 *          SQLCreateDataSource done using Qt4. Here is where we enter into Qt4 'world'.
 * 
 * \param   hWnd
 * 
 * \return  BOOL
 */
static BOOL QT4CreateDataSource( HWND hWnd, LPCSTR pszDataSourceName )
{
    /*
     * Here, we handle case where a non QT app has called us. Our approach here is a bit different 
     * than what we used for Qt3 because we were getting segfault upon app exit. Here; we instantiate
     * with a 'new' and NOT 'delete' it. Its a work around.
     */ 
    if ( !qApp )
    {
        int argc = 1;
        char *argv[] = { "odbcinstQ4", NULL };

        QApplication *pApplication = new QApplication( argc, argv );
        QCoreApplication::setOrganizationName("unixODBC");
        QCoreApplication::setOrganizationDomain("unixodbc.org");
        QCoreApplication::setApplicationName("ODBC Administrator");
    }

    // missing parent widget? Use desktop...
    QWidget *pWidget = (QWidget*)hWnd;
    if ( !pWidget )
        pWidget = qApp->desktop();

    // show dialog...
    CDSNWizardData  WizardData( pszDataSourceName );
    CDSNWizard      odbccreate( &WizardData, pWidget );
	int nReturn = odbccreate.exec();

    //  sucks to have to do this after wizard is on its way out - no fail and Back :(
    if ( nReturn == QDialog::Accepted )
        return CODBCInst::saveDataSourceName( pWidget, WizardData.hFirstProperty, WizardData.nType );

    return false;
}

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief   Plugin in entry point.
 *
 *          This entry point is called to implement the SQLManageDataSources call. This
 *          plugin does it using Qt4.
 */
BOOL ODBCManageDataSources( HWND hWnd )
{
    return QT4ManageDataSources( hWnd );
}

/*!
 * \brief   Plugin in entry point.
 *
 *          This entry point is called to implement the SQLCreateDataSource call. This
 *          plugin does it using Qt4.
 */
BOOL ODBCCreateDataSource( HWND hWnd, LPCSTR lpszDS )
{
    return QT4CreateDataSource( hWnd, lpszDS );
}

#ifdef __cplusplus
};
#endif

