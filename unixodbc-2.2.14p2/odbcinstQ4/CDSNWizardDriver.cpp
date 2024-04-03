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

#include "CDSNWizardDriver.h"

#include "CDSNWizard.h"
#include "CDriverList.h"

CDSNWizardDriver::CDSNWizardDriver( CDSNWizardData *pWizardData, QWidget *pwidgetParent )
     : QWizardPage( pwidgetParent )
{
    this->pWizardData = pWizardData;

    QVBoxLayout *playout = new QVBoxLayout( this );

    pDriverList = new CDriverList;

    playout->addWidget( pDriverList );

    setTitle( tr( "Driver" ) );
}

int CDSNWizardDriver::nextId() const
{
    pWizardData->stringDriver = pDriverList->getFriendlyName();
    return CDSNWizard::PageProperties;
}

void CDSNWizardDriver::initializePage()
{
}

void CDSNWizardDriver::cleanupPage()
{
    pWizardData->stringDriver = QString::null;
}

bool CDSNWizardDriver::validatePage()
{
    if ( pDriverList->getFriendlyName().isEmpty() )
    {
        QMessageBox::warning( this, tr( "Page Validate" ), tr( "Please select a Driver." ) );
        return false;
    }

    // the following assumptions are made here;
    // - this method is only called just before going to next page
    // - next page cleanUp() will destruct property list (before we can come back here)
    // - no other validation checks, in this method, are placed after this
    if ( ODBCINSTConstructProperties( pDriverList->getFriendlyName().toAscii().data(), &pWizardData->hFirstProperty ) != ODBCINST_SUCCESS )
    {
        CODBCInst::showErrors( this, QString( "Could not construct a property list for (%1). Contact your; SysAdmin, driver vendor, or folks at the unixODBC project for support." ).arg( pDriverList->getFriendlyName() ) );
        return false;
    }

    return true;
}


