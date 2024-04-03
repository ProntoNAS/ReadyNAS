#ifndef CSPLASHDIALOG_H
#define CSPLASHDIALOG_H

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qobject.h>
#include <Qt/qwidget.h>
#include <Qt/qdialog.h>
#include <Qt/qcheckbox.h>
#include <Qt/qlayout.h>
#include <Qt/qpushbutton.h>
#include <Qt/qpixmap.h>
#include <Qt/qsettings.h>
#else
#include <qobject.h>
#include <qwidget.h>
#include <qdialog.h>
#include <qcheckbox.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qpixmap.h>
#if (QT_VERSION>=300)
#include <qsettings.h>
#endif
#endif

class CSplashDialog : public QDialog
{
public:
    CSplashDialog( QWidget *pwidgetParent );
    virtual ~CSplashDialog();

protected:
   QCheckBox *pcheckbox;
};

#endif

