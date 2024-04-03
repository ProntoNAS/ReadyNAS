/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CMANAGEDATASOURCENAMES_H
#define CMANAGEDATASOURCENAMES_H

#include "CODBCInst.h"

#include <QTabWidget>

#define ODBC_HELP_DATASOURCES "The most common way to connect to a Data Source is via a Data Source Name (DSN). A DSN is a saved set of connection options - a very convenient way to repeatedly connect to a Data Source."

class CDataSourceNames;
class CDataSourceNamesFile;

/*!
 * \class   CManageDataSourceNames
 *
 * \brief   Widget to allow the Data Source Name's (DSN's) to be managed.
 *
 *          This widget presents a set of 3 tabs where DSN's can be managed;
 *
 *          \li User DSN's
 *          \li System DSN's
 *          \li File DSN's
 *
 * \note    System DSN's and often File DSN's - can only be managed when elevated privileges 
 *          such as 'root' is in effect.
 *
 * \sa      CManageDrivers
 */
class CManageDataSourceNames : public QTabWidget
{
    Q_OBJECT
public:
    CManageDataSourceNames( QWidget *pwidgetParent = 0 );
    virtual ~CManageDataSourceNames();

    QString windowHelp() { return QString( tr( ODBC_HELP_DATASOURCES ) ); }
    bool saveData() { return true; }

public slots:
    void slotLoad();

protected:
    CDataSourceNames *      pDataSourceNamesUser;
    CDataSourceNames *      pDataSourceNamesSystem;
    CDataSourceNamesFile *  pDataSourceNamesFile;
};

#endif

