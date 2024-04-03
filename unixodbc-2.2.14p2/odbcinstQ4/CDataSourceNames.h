/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CDATASOURCENAMES_H
#define CDATASOURCENAMES_H

#include "CODBCInst.h"

#define ODBC_HELP_DATASOURCES_USER "User Data Source Names (DSN's) exist for use by a specific system account (User). The DSN's are typically stored somewhere in the User's home directory. These have precedence over System DSN's when DSN exists in both."
#define ODBC_HELP_DATASOURCES_SYSTEM "System Data Source Names (DSN's) exist for use by all system accounts. Elevated privileges (such as 'root') are normally required to manage these."

class CDataSourceNameList;

/*!
 * \class   CDataSourceNames
 * 
 * \brief   Widget to allow Data Source Names (DSN's) to be managed.
 *
 *          This is used to support \sa CManageDataSourceNames by implementing
 *          the bulk of the features in a way which is useful for a specific scope.
 *          This widget can be instantiated with ODBC_USER_DSN to support managing
 *          User DSN's or with ODBC_SYSTEM_DSN to support managing System DSN's.
 * 
 *          Basically; this widget wraps some buttons around a CDataSourceNames so that
 *          the DSN's can be; Added, Edited, or Removed.
 *
 * \note    Do NOT try to use this to manage ODBC_BOTH_DSN. See \sa CManageDataSourceNames
 *          for that.
 *
 * \sa      CManageDataSourceNames
 *          CDataSourceNamesFile
 */
class CDataSourceNames : public QWidget
{
    Q_OBJECT
public:
    CDataSourceNames( QWidget* pwidgetParent = NULL, int nSource = ODBC_USER_DSN );
    virtual ~CDataSourceNames();

    QString windowHelp() { if ( nSource == ODBC_USER_DSN ) return QString( tr( ODBC_HELP_DATASOURCES_USER ) ); return QString( tr( ODBC_HELP_DATASOURCES_SYSTEM ) ); }

public slots:
    void slotLoad();

protected:
    int                     nSource;
    CDataSourceNameList *   pDataSourceNameList;
};

#endif

