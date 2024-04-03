/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CDSNWIZARD_h
#define CDSNWIZARD_h

#include "CODBCInst.h"

#include <QWizard>

/*! 
 * \class   CDSNWizard
 * \brief   Data Source Name creation wizard.
 *
 *          This dialog is used to guide a User through creating a; user, system, or file DSN. This
 *          is used to support a SQLCreateDataSource call when neither driver, nor drivers setup, is 
 *          supplying a ConfigDSN to the Driver Manager. 
 *
 *          This will try to find the unixODBC generic attributes call in the Driver and/or Setup 
 *          (ODBCINSTGetProperties). Failing that it will simply allow free-form editing of keyword/value 
 *          pairs.
 *
 * \sa      CODBCConfig 
 */
class CDSNWizard : public QWizard
{
    Q_OBJECT
public:
    enum 
    { 
        PageType, 
        PageEntre, 
        PageDriver, 
        PageProperties, 
        PageFini
    };

    CDSNWizard( CDSNWizardData *pWizardData, QWidget* pwidgetParent = 0 );
    virtual ~CDSNWizard();

protected slots:
    void slotHelp();

protected:
    CDSNWizardData *pWizardData;
};

#endif

