#ifndef CLASSSTATS_H
#define CLASSSTATS_H

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qwidget.h>
#include <Qt/q3grid.h>
#include <Qt/q3vbox.h>
#include <Qt/qlayout.h>
#include <Qt/qtabbar.h>
#include <Qt/qpixmap.h>
#else
#include <qwidget.h>
#include <qgrid.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qtabbar.h>
#include <qpixmap.h>
#endif

#include "CStatSummary.h"
#include "CStatDetails.h"

class CStats : public QWidget
{

	Q_OBJECT

public:
	CStats( QWidget* parent = NULL, const char* name = NULL );
	virtual ~CStats();

public slots:

protected:

protected slots:

private:
    CStatSummary * pSummary;
    CStatDetails * pDetails;

};

#endif


