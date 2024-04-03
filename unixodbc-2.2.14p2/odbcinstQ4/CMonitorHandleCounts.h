/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CMONITORHANDLECOUNTS_H
#define CMONITORHANDLECOUNTS_H

#include "CODBCInst.h"

#include <uodbc_stats.h>

class CMonitorHandleCounts : public QWidget
{
	Q_OBJECT
public:
	CMonitorHandleCounts( QWidget* pwidgetParent = NULL );
	virtual ~CMonitorHandleCounts();

protected:
    bool        bEnabled;
    void *      hStats;
    int         nSliderMax;
    QTimer *    pTimer;
    QLabel *    plabelEnvironment;
    QLabel *    plabelConnection;
    QLabel *    plabelStatement;
    QLabel *    plabelDescriptor;
    QSlider *   psliderEnvironment;
    QSlider *   psliderConnection;
    QSlider *   psliderStatement;
    QSlider *   psliderDescriptor;

protected slots:
    void slotLoad();
};


#endif

