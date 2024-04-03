#ifndef CLASSTRACING_H
#define CLASSTRACING_H

#include <odbcinstext.h>

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qwidget.h>
#include <Qt/qframe.h>
#include <Qt/q3groupbox.h>
#include <Qt/qlabel.h>
#include <Qt/qcheckbox.h>
#include <Qt/q3grid.h>
#include <Qt/q3vbox.h>
#include <Qt/qlayout.h>
#include <Qt/qpushbutton.h>
#else
#include <qwidget.h>
#include <qframe.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qgrid.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#endif

#include "CFileSelector.h"

class CTracing : public QWidget
{
	Q_OBJECT
public:
	CTracing( QWidget* parent = NULL, const char* name = NULL );
	virtual ~CTracing();

public slots:
    void setDefault();
	void apply();

protected:
	QCheckBox *         pTracing;
	QCheckBox *         pForce;
	CFileSelector *     pTraceFile;
	QCheckBox *         pPooling;
};

#endif


