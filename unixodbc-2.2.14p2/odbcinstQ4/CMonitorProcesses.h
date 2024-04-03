/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CMONITORPROCESSES_H
#define CMONITORPROCESSES_H

#include "CODBCInst.h"

#include <uodbc_stats.h>

#include <QTableWidget>

/*! 
 * \class   CMonitorProcesses
 * \brief   Displays detailed statistics on the unixODBC environment.
 * 
 *          This widget displays a list of; Environments, Connections, Statements,
 *          Descriptors and their owning Process ID. The details are updated frequently
 *          as long as the widget is visible.
 *
 * \sa      CMonitor
 */
class CMonitorProcesses : public QTableWidget
{
    Q_OBJECT
public:
	CMonitorProcesses( QWidget* pwidgetParent = NULL );
	virtual ~CMonitorProcesses();

protected slots:
    void slotLoad();
    void clearRow( int nRow );

protected:
    bool        bEnabled;
    int         nRowsWithValues;
    QTimer *    pTimer;
    void *      hStats;
};

#endif

