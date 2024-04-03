/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CMANAGEDRIVERS_H
#define CMANAGEDRIVERS_H

#include <ini.h>

#include "CODBCInst.h"

#define ODBC_HELP_DRIVERS "An ODBC Driver allows your ODBC enabled applications to get to your data. Many ODBC drivers can be downloaded from the Internet while others are obtained from your database vendor. Typically; you must be a 'root' user to manage drivers."

/*! 
 * \class   CManageDrivers
 * \brief   Widget to allow User to manage Drivers.
 *
 *          This widget presents a list of installed Drivers. The Drivers listed will be 64bit if source 
 *          of unixODBC built with PLATFORM64 - otherwise they will be 32bit. 
 *
 *          The User may (given proper privs); add, delete, and configure a selected Driver.
 *
 *          This widget is used as a tab for CODBCConfig.
 *
 * \sa      CManageDataSourceNames
 *          CStats
 *          CTracing
 *          CAbout
 */
class CManageDrivers : public QWidget
{
    Q_OBJECT
public:
    CManageDrivers( QWidget* pwidgetParent = NULL );
    virtual ~CManageDrivers();

    QString windowHelp() { return QString( tr( ODBC_HELP_DRIVERS ) ); }
    bool saveData() { return true; }

signals:
    void signalChanged();
};

#endif

