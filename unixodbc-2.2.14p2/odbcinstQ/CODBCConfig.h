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
#ifndef CODBCConfig_included
#define CODBCConfig_included

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qwidget.h>
#include <Qt/qmessagebox.h>
#include <Qt/qpixmap.h>
#include <Qt/qdialog.h>
#include <Qt/qtabbar.h>
#include <Qt/qframe.h>
#include <Qt/qpixmap.h>
#include <Qt/qlayout.h>
#include <Qt/q3tabdialog.h>
#include <Qt/qsettings.h>
#else
#include <qwidget.h>
#include <qmessagebox.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qtabbar.h>
#include <qframe.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qtabdialog.h>
#if QT_VERSION>=300
#include <qsettings.h>
#endif
#endif

#include "CUserDSN.h"
#include "CSystemDSN.h"
#include "CFileDSN.h"
#include "CDrivers.h"
#include "CAbout.h"
#include "CTracing.h"
#include "CStats.h"

#ifdef QT_V4LAYOUT
#define TDialog			Q3TabDialog
#else
#define TDialog			QTabDialog
#endif

class CODBCConfig : public TDialog
{
    Q_OBJECT

public:
#ifdef QT_V4LAYOUT
    CODBCConfig( QWidget* parent = 0, const char* name = 0, Qt::WFlags nFlags = 0 );
#else
    CODBCConfig( QWidget* parent = 0, const char* name = 0, WFlags nFlags = 0 );
#endif
    virtual ~CODBCConfig();

protected:
    CUserDSN    *pUserDSN;
    CSystemDSN  *pSystemDSN;
    CFileDSN    *pFileDSN;
    CDrivers    *pDrivers;
    CStats      *pStats;
    CTracing    *pTracing;
    CAbout      *pAbout;

    virtual void LoadState();
    virtual void SaveState();
};
#endif 

