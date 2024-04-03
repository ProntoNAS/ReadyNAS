/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#include "CDSNWizardProperties.h"

#include "CDSNWizard.h"

#include "CPropertiesModel.h"
#include "CPropertiesDelegate.h"

CDSNWizardProperties::CDSNWizardProperties( CDSNWizardData *pWizardData, QWidget *pwidgetParent )
     : QWizardPage( pwidgetParent )
{
    this->pWizardData = pWizardData;

    QVBoxLayout *pLayout = new QVBoxLayout( this );

    pPropertiesModel    = 0;
    pPropertiesDelegate = new CPropertiesDelegate;

    pTableView = new QTableView;
    pTableView->setItemDelegateForColumn ( 1, pPropertiesDelegate );
    pTableView->verticalHeader()->hide();
    pLayout->addWidget( pTableView, 10 );

    setTitle( tr( "Properties" ) );
}

CDSNWizardProperties::~CDSNWizardProperties()
{
    // free up memory associated with this page (model, delegate, and property list)...
    cleanupPage();
    if ( pPropertiesDelegate )
        delete pPropertiesDelegate;
}

int CDSNWizardProperties::nextId() const
{
    // at this point pWizardData is loaded up...
    return CDSNWizard::PageFini;
}

void CDSNWizardProperties::initializePage()
{
    pPropertiesModel  = new CPropertiesModel( 0, pWizardData->hFirstProperty );
    pTableView->setModel( pPropertiesModel );
}

void CDSNWizardProperties::cleanupPage()
{
    // always free up our property list here because we may end up with a different stringDriver...
    pTableView->setModel( 0 );

    if ( pPropertiesModel )
        delete pPropertiesModel;
    pPropertiesModel = 0;

    if ( pWizardData->hFirstProperty )
        ODBCINSTDestructProperties( &pWizardData->hFirstProperty );
}

bool CDSNWizardProperties::validatePage()
{
    // handle case where we did not ODBCINSTConstructProperties()...
    if ( !pWizardData->hFirstProperty )
    {
        QMessageBox::warning( this, tr( "Page Validate" ), tr( "Sorry; can no proceed without a viable property list. Possible installation/configuration issue - please contact your SysAdmin or driver vendor (or select a different driver)." ) );
        return false;
    }

    // the first property is always Name... ensure we have one...
    QString stringName( pWizardData->hFirstProperty->szValue );
    stringName = stringName.simplified();
    if ( stringName.isEmpty() )
    {
        QMessageBox::warning( this, tr( "Page Validate" ), tr( "Please specify a Name." ) );
        return false;
    }

    // do the User a favour - remove extra white space (if any)...
    strcpy( pWizardData->hFirstProperty->szValue, stringName.toAscii().data() );

    // driver is also mandatory but its a readonly property picked up from previous page - so we have one

    return true;
}


