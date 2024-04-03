/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#include "CDSNWizardFini.h"

#include "CDSNWizard.h"

CDSNWizardFini::CDSNWizardFini( CDSNWizardData *pWizardData, QWidget *pwidgetParent )
     : QWizardPage( pwidgetParent )
{
    this->pWizardData = pWizardData;

    QHBoxLayout *   pLayout         = new QHBoxLayout( this );
    QTextBrowser *  pTextBrowser    = new QTextBrowser;

    pTextBrowser->setHtml( "<P>Click Finish to save the Data Source Name with the information you have provided.</P>" );
    pLayout->addWidget( pTextBrowser );

    setTitle( tr( "Finish" ) );
}

int CDSNWizardFini::nextId() const
{
    return -1;
}

void CDSNWizardFini::initializePage()
{
}

void CDSNWizardFini::cleanupPage()
{
}


