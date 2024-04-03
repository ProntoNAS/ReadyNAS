/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CTRACING_H
#define CTRACING_H

#include "CODBCInst.h"

class CFileSelector;

#define ODBC_HELP_TRACING "The ability to trace ODBC activity is important to anyone trying to debug an ODBC application centric problem. When tracing is on; all calls are logged to a file. It is very bad to leave tracing on after a debugging session as it slows application performance an causes disk space to be used up."

/*! 
 * \class   CTracing
 * \brief   This widget allows User to view/change tracing options.
 *
 *          With this widget the User can;
 *
 *          \li turn tracing on/off
 *          \li elect to force tracing
 *          \li specify a trace file
 *
 * \sa      CManageDataSourceNames
 *          CManageDrivers
 *          CMonitor
 *          CAdvanced
 *          CAbout
 */
class CTracing : public QWidget
{
	Q_OBJECT
public:
	CTracing( QWidget* pwidgetParent = NULL );
	virtual ~CTracing();

    QString windowHelp() { return QString( tr( ODBC_HELP_TRACING ) ); }
    bool loadData();
    bool saveData();

public slots:
    bool slotApply();
    bool slotDefault();

protected:
    QCheckBox *     pcheckboxEnable;
    QCheckBox *     pcheckboxForce;
    CFileSelector * pfileselectorTraceFile;
    CFileSelector * pfileselectorTraceLibrary;
};


#endif

