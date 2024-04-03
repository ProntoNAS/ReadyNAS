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
#ifndef CAbout_included
#define CAbout_included

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qwidget.h>
#include <Qt/qpixmap.h>
#include <Qt/qlayout.h>
#include <Qt/qmessagebox.h>
#include <Qt/qlabel.h>
#include <Qt/qpushbutton.h>
#include <Qt/qframe.h>
#include <Qt/qmovie.h>
#else
#include <qwidget.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qpushbt.h>
#include <qframe.h>
#include <qmovie.h>
#endif

#include "CCredits.h"


class CAboutDiagram : public QWidget
{
    Q_OBJECT

public:

    CAboutDiagram( QWidget* pwidgetParent = NULL, const char* pszName = NULL );
    virtual ~CAboutDiagram();

protected slots:
    void pbODBCConfig_Clicked();
    void pbODBC_Clicked();
    void pbDatabase_Clicked();
    void pbDriverManager_Clicked();
    void pbDriver_Clicked();
    void pbODBCDrivers_Clicked();
    void pbApplication_Clicked();
};

class CAbout : public QWidget
{
    Q_OBJECT

public:

    CAbout( QWidget* pwidgetPrent = NULL, const char* pszName = NULL );
    virtual ~CAbout();

protected slots:
    void pbCredits_Clicked();
};

#endif 
