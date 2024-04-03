/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CDSNWIZARDPROPERTIES_H
#define CDSNWIZARDPROPERTIES_H

#include "CODBCInst.h"

#include <QWizardPage>

class CPropertiesModel;
class CPropertiesDelegate;

class CDSNWizardProperties : public QWizardPage
{
     Q_OBJECT
public:
     CDSNWizardProperties( CDSNWizardData *pWizardData, QWidget *pwidgetParent = 0 );
     virtual ~CDSNWizardProperties();

     int nextId() const;
     void initializePage();
     void cleanupPage();
     bool validatePage();

protected:
     CDSNWizardData *       pWizardData;
     CPropertiesModel *     pPropertiesModel;
     CPropertiesDelegate *  pPropertiesDelegate;
     QTableView *           pTableView;
};

#endif

