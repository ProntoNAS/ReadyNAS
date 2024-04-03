#ifndef CLASSCREDITS_H
#define CLASSCREDITS_H

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qwidget.h>
#include <Qt/qlayout.h>
#include <Qt/qdialog.h>
#include <Qt/qpushbutton.h>
#include <Qt/q3listbox.h>
#else
#include <qwidget.h>
#include <qlayout.h>
#include <qlistbox.h>
#include <qdialog.h>
#include <qpushbutton.h>
#endif

class CCredits : public QDialog
{
	Q_OBJECT
public:
    CCredits( QWidget* parent = 0, const char* name = 0 );
	virtual ~CCredits();
};

#endif


