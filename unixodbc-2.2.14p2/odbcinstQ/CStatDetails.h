#ifndef CLASSSTATDETAILS_H
#define CLASSSTATDETAILS_H

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qwidget.h>
#include <Qt/qframe.h>
#include <Qt/qlabel.h>
#include <Qt/qcheckbox.h>
#include <Qt/q3grid.h>
#include <Qt/q3vbox.h>
#include <Qt/qlayout.h>
#include <Qt/qpushbutton.h>
#include <Qt/qtimer.h>
#include <Qt/qstring.h>
#include <Qt/q3table.h>
#else
#include <qwidget.h>
#include <qframe.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qgrid.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtimer.h>
#include <qstring.h>
#include <qtable.h>
#endif

#include <uodbc_stats.h>

#define MAXPROCESSES 10
#define MAXHANDLES 4

class CStatDetails : public QWidget
{

	Q_OBJECT

public:
	CStatDetails( QWidget* parent = NULL, const char* name = NULL );
	virtual ~CStatDetails();

    QTimer *pTimer;

protected:
    void clearRow( int nRow );

protected slots:
    void showStats();

private:
#ifdef QT_V4LAYOUT
    Q3Table *    pTable;
#else
    QTable *    pTable;
#endif
    void *      hStats;
    uodbc_stats_retentry aPIDs[MAXPROCESSES];
    uodbc_stats_retentry aHandles[MAXHANDLES];
};

#endif


