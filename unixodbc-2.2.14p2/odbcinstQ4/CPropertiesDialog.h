/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CPROPERTIESDIALOG_H
#define CPROPERTIESDIALOG_H

#include "CODBCInst.h"

#include <QDialog>

class CPropertiesModel;
class CPropertiesDelegate;

class CPropertiesDialog : public QDialog
{
    Q_OBJECT
public:
    CPropertiesDialog( QWidget* pwidgetParent = NULL, HODBCINSTPROPERTY hTheFirstProperty = NULL );
    virtual ~CPropertiesDialog();

protected:
    void doLoadState();
    void doSaveState();

private:
    CPropertiesModel *      ppropertiesmodel;
    CPropertiesDelegate *   ppropertiesdelegate;
};

#endif

