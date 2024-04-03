/**************************************************
 * 
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign. 
 * Released under GPL 31.JAN.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 **************************************************/
#ifndef CDriverPrompt_included
#define CDriverPrompt_included

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qdialog.h>
#include <Qt/qframe.h>
#include <Qt/qpixmap.h>
#include <Qt/qlayout.h>
#include <Qt/qlabel.h>
#include <Qt/q3listview.h>
#include <Qt/qpushbutton.h>
#else
#include <qdialog.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbt.h>
#endif

#include "CDrivers.h"

class CDriverPrompt : public QDialog
{
    Q_OBJECT

public:
    CDriverPrompt( QWidget* parent = NULL, const char* name = NULL );
    ~CDriverPrompt();

	QString	qsDriverName;
	QString	qsDescription;
	QString	qsDriver;
	QString	qsSetup;

protected slots:

    void pbCancel_Clicked();
    void pbOk_Clicked();

protected:
    CDrivers* pDrivers;
};
#endif 
