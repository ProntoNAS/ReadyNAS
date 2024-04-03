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
#ifndef CSystemDSN_included
#define CSystemDSN_included

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qwidget.h>
#include <Qt/qpushbutton.h>
#include <Qt/qpixmap.h>
#include <Qt/qlayout.h>
#else
#include <qwidget.h>
#include <qpushbt.h>
#include <qpixmap.h>
#include <qlayout.h>
#endif

#ifdef QT_V4LAYOUT
#include "CDSNList4.h"
#else
#include "CDSNList.h"
#endif

class CSystemDSN : public QWidget
{
    Q_OBJECT

public:

    CSystemDSN( QWidget* parent = NULL, const char* name = NULL );
    ~CSystemDSN();

protected:
    QPushButton* pbAdd;
    QPushButton* pbRemove;
    QPushButton* pbConfigure;
    CDSNList* pDSNList;
};
#endif
