/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CADVANCED_h
#define CADVANCED_h

#include "CODBCInst.h"

#include <QTabWidget>

#define ODBC_HELP_ADVANCED "Most of the features which usually require elevated privileges (read; 'root' access) are administered here."

class CManageDrivers;
class CPooling;
class CTracing;
class CThreading;

/*! 
 * \class   CAdvanced
 * \brief   Groups the advanced administration features.
 *
 *          This widget presents a set of tabs for the various 'advanced' features.
 *
 * \sa      CManageDataSourceNames
 *          CManageDrivers
 *          CStats
 *          CAbout
 */
class CAdvanced : public QTabWidget
{
    Q_OBJECT
public:
    CAdvanced( QWidget* pwidgetParent = NULL );
    virtual ~CAdvanced();

    QString windowHelp() { return QString( tr( ODBC_HELP_ADVANCED ) ); }
    bool saveData();

signals:
    void signalChanged();

protected:
    CManageDrivers *pManageDrivers;
    CPooling *      pPooling;
    CTracing *      pTracing;
    CThreading *    pThreading;
};

#endif

