/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#include "CDSNWizard.h"

#include "CDSNWizardEntre.h"
#include "CDSNWizardType.h"
#include "CDSNWizardDriver.h"
#include "CDSNWizardProperties.h"
#include "CDSNWizardFini.h"

#include "ODBCManageDataSources64.xpm"
#include "WizardCreateDataSourceName.xpm"

CDSNWizard::CDSNWizard( CDSNWizardData *pWizardData, QWidget* pwidgetParent )
    : QWizard( pwidgetParent )
{
    CDSNWizardEntre *       pWizardEntre        = new CDSNWizardEntre( pWizardData );
    CDSNWizardType *        pWizardType         = new CDSNWizardType( pWizardData );
    CDSNWizardDriver *      pWizardDriver       = new CDSNWizardDriver( pWizardData );
    CDSNWizardProperties *  pWizardProperties   = new CDSNWizardProperties( pWizardData );
    CDSNWizardFini *        pWizardFini         = new CDSNWizardFini( pWizardData );

    setPage( PageEntre,     pWizardEntre );
    setPage( PageType,      pWizardType );
    setPage( PageDriver,    pWizardDriver );
    setPage( PageProperties,pWizardProperties );
    setPage( PageFini,      pWizardFini );

    setStartId( PageEntre );

    setOption( HaveHelpButton, true );
    connect( this, SIGNAL(helpRequested()), this, SLOT(slotHelp()) );

    setWindowTitle( tr( "Create Data Source Name Wizard" ) );
    setWindowIcon( QPixmap( xpmODBCManageDataSources64 ) );
    setPixmap( QWizard::WatermarkPixmap, QPixmap( xpmWizardCreateDataSourceName ) );
}

CDSNWizard::~CDSNWizard()
{
}

void CDSNWizard::slotHelp()
{
    QString stringMessage;

    switch ( currentId() )
    {
        case PageEntre:
            stringMessage = tr( "This page introduces the wizard - stating its purpose." );
            break;
        case PageType:
            stringMessage = tr( "Choose from one of the 3 types of Data Source Names (DSN's).\n\nUser and System DSN's are stored in hidden files.\n\nEach File DSN is stored in its own file - which makes it easy to share/deploy on a network.\n\nElevated privileges are usually required to create a System or File DSN.\n\nThose unsure which to choose should choose User." );
            break;
        case PageDriver:
            stringMessage = tr( "Choose a driver. Each ODBC data source has a driver. ODBC Drivers must be registered when/after they are installed on your system. Contact your SysAdmin or data source vendor if your driver is not shown here." );
            break;
        case PageProperties:
            stringMessage = tr( "Connect properties. Defaults are usually fine but please provide a Name. Contact driver vendor for details about these properties." );
            break;
        case PageFini:
            stringMessage = tr( "This page simply states that we have all the required information." );
            break;
        default:
            stringMessage = tr( "Sorry; no help implemented for this page." );
    }

    QMessageBox::information( this, tr("Create Data Source Name Wizard Help"), stringMessage );
}

