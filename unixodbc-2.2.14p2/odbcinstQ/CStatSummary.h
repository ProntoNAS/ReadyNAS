#ifndef CLASSSTATSUMMARY_H
#define CLASSSTATSUMMARY_H

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
#include <Qt/qslider.h>
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
#include <qslider.h>
#endif

#include <uodbc_stats.h>

class CStatSummary : public QWidget
{

	Q_OBJECT

public:
	CStatSummary( QWidget* parent = NULL, const char* name = NULL );
	virtual ~CStatSummary();

    QTimer *pTimer;

    QLabel *pEnv;
    QLabel *pCon;
    QLabel *pSta;
    QLabel *pDes;

    QSlider *pEnvSlider;
    QSlider *pConSlider;
    QSlider *pStaSlider;
    QSlider *pDesSlider;

protected:
    int nSliderMax;

protected slots:
    void showStats();

private:
    void *hStats;
    uodbc_stats_retentry aStats[4];
};

#endif


