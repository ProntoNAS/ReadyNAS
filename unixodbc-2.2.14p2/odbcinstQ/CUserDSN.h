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
#ifndef CUserDSN_included
#define CUserDSN_included

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qwidget.h>
#include <Qt/q3button.h>
#include <Qt/qpixmap.h>
#include <Qt/qlayout.h>
#include <Qt/qlabel.h>
#include <Qt/qframe.h>
#else
#include <qwidget.h>
#include <qpushbt.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>
#endif

#ifdef QT_V4LAYOUT
#include "CDSNList4.h"
#else
#include "CDSNList.h"
#endif

class CUserDSN : public QWidget
{
    Q_OBJECT

public:

    CUserDSN( QWidget* parent = NULL, const char* name = NULL );
    ~CUserDSN();

protected:
    QPushButton* pbAdd;
    QPushButton* pbRemove;
    QPushButton* pbConfigure;
    CDSNList* pDSNList;
};
#endif 
