/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CABOUT_H
#define CABOUT_H

#include "CODBCInst.h"

#define ODBC_HELP_ABOUT "Hopefully you find some interesting leads to more information about unixODBC and ODBC (in general) here."

/*!
 * \class   CAbout
 * \brief   Provides information about this application and this project (unixODBC).
 *
 *          This widget is used as a tab for CODBCConfig.
 *
 * \sa      CManageDataSourceNames
 *          CManageDrivers
 */
class CAbout : public QTabWidget
{
    Q_OBJECT
public:
    CAbout( QWidget* pParent = NULL );
    virtual ~CAbout();

    QString windowHelp() { return QString( tr( ODBC_HELP_ABOUT ) ); }
    bool saveData() { return true; }
};

#endif

