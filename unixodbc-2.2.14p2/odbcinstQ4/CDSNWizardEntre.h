/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CDSNWIZARDENTRE_H
#define CDSNWIZARDENTRE_H

#include "CODBCInst.h"

#include <QWizardPage>

/*! 
 * \class   CDSNWizardEntre
 * \brief   First page of create data source name wizard.
 * 
 *          This page describes what the wizard is for.
 *
 * \sa      CDSNWizard
 */
class CDSNWizardEntre : public QWizardPage
{
     Q_OBJECT
public:
     CDSNWizardEntre( CDSNWizardData *pWizardData, QWidget *pwidgetParent = 0 );

     int nextId() const;
     void initializePage();
     void cleanupPage();

protected:
     CDSNWizardData *pWizardData;
};

#endif

