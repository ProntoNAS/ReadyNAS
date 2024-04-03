/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#include "CDSNWizardType.h"

#include "CDSNWizard.h"

CDSNWizardType::CDSNWizardType( CDSNWizardData *pWizardData, QWidget *pwidgetParent )
     : QWizardPage( pwidgetParent )
{
    this->pWizardData = pWizardData;

    QVBoxLayout *   pLayout     = new QVBoxLayout( this );

    // group of radio buttons...
    {
        QGroupBox *     pGroupBox   = new QGroupBox;
    
        // buttons...
        {
            QVBoxLayout *pLayout = new QVBoxLayout;
        
            pradiobuttonUser      = new QRadioButton( tr( "&User" ) );
            pradiobuttonSystem    = new QRadioButton( tr( "&System" ) );
            pradiobuttonFile      = new QRadioButton( tr( "&File" ) );
        
            pradiobuttonUser->setChecked( true );
        
            pLayout->addWidget( pradiobuttonUser );
            pLayout->addWidget( pradiobuttonSystem );
            pLayout->addWidget( pradiobuttonFile );
            pLayout->addStretch( 1 );
            pGroupBox->setLayout( pLayout );
        }
    
        pLayout->addWidget( pGroupBox );
    }

    setTitle( tr( "Type" ) );
}

int CDSNWizardType::nextId() const
{
    if ( pradiobuttonSystem->isChecked() )
        pWizardData->nType = CDSNWizardData::TypeSystem;
    else if ( pradiobuttonFile->isChecked() )
        pWizardData->nType = CDSNWizardData::TypeFile;
    else
        pWizardData->nType = CDSNWizardData::TypeUser;

    return CDSNWizard::PageDriver;
}

void CDSNWizardType::initializePage()
{
    if ( pWizardData->nType == CDSNWizardData::TypeSystem )
        pradiobuttonSystem->setChecked( true );
    else if ( pWizardData->nType == CDSNWizardData::TypeFile )
        pradiobuttonFile->setChecked( true );
    else    
        pradiobuttonUser->setChecked( true );
}

void CDSNWizardType::cleanupPage()
{
    pWizardData->nType = CDSNWizardData::TypeUser;
}

