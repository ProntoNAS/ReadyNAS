/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#include "CDSNWizardEntre.h"

#include "CDSNWizard.h"

CDSNWizardEntre::CDSNWizardEntre( CDSNWizardData *pWizardData, QWidget *pwidgetParent )
     : QWizardPage( pwidgetParent )
{
    this->pWizardData = pWizardData;

    QHBoxLayout *   pLayout         = new QHBoxLayout( this );
    QTextBrowser *  pTextBrowser    = new QTextBrowser;

    pTextBrowser->setHtml( "<P>This is the Create Data Source Name Wizard.</P> <P>A Data Source Name (DSN) is a convenient way to connect to a data source since the connect options can be set in advance and simply reused, by name, during a connect.</P> <P>This wizard will guide you through the process of creating a new Data Source Name.</P>" );
    pLayout->addWidget( pTextBrowser );

    setTitle( tr( "Introduction" ) );
}

int CDSNWizardEntre::nextId() const
{
    return CDSNWizard::PageType;
}

void CDSNWizardEntre::initializePage()
{
}

void CDSNWizardEntre::cleanupPage()
{
}

