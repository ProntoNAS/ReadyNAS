/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CDSNWIZARDDRIVER_H
#define CDSNWIZARDDRIVER_H

#include "CODBCInst.h"

#include <QWizardPage>

class CDriverList;

class CDSNWizardDriver : public QWizardPage
{
     Q_OBJECT
public:
     CDSNWizardDriver( CDSNWizardData *pWizardData, QWidget *pwidgetParent = 0 );

     int nextId() const;
     void initializePage();
     void cleanupPage();
     bool validatePage();

protected:
     CDSNWizardData *   pWizardData;
     CDriverList *      pDriverList;
};

#endif


