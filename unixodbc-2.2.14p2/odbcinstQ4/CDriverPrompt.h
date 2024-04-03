/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CDRIVERPROMPT_H
#define CDRIVERPROMPT_H

#include "CODBCInst.h"

#include <QDialog>

class CDriverList;

/*! 
 * \class   CDriverPrompt 
 *
 * \brief   Allows the User to select a Driver.
 *
 *          Displays a list of installed Drivers and allows a User to; add, delete, configure and 
 *          ultimately - return a selection. This is used to support a request to create a DSN.
 *
 * \sa      CUserDataSources
 *          CSystemDataSources
 */
class CDriverPrompt : public QDialog
{
    Q_OBJECT
public:
    CDriverPrompt( QWidget* pwidgetParent = NULL );
    virtual ~CDriverPrompt();

    QString getFriendlyName();
    QString getDescription();
    QString getDriver();
    QString getSetup();

protected slots:
    void slotOk();

protected:
    CDriverList *  pDriverList;

    void doSaveState();
    void doLoadState();
};

#endif

