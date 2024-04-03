/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CDSNWIZARDTYPE_H
#define CDSNWIZARDTYPE_H

#include "CODBCInst.h"

#include <QWizardPage>

/*! 
 * \class   CDSNWizardEntre
 * \brief   First page of create data source name wizard.
 * 
 *          This page asks for the type of DSN to create;
 *
 *          \li User
 *          \li System
 *          \li File
 *
 * \sa      CDSNWizard
 */
class CDSNWizardType : public QWizardPage
{
     Q_OBJECT
public:
     CDSNWizardType( CDSNWizardData *pWizardData, QWidget *pwidgetParent = 0 );

     int nextId() const;
     void initializePage();
     void cleanupPage();

protected:
     CDSNWizardData *pWizardData;
     QRadioButton *  pradiobuttonUser;
     QRadioButton *  pradiobuttonSystem;
     QRadioButton *  pradiobuttonFile;
};

#endif

