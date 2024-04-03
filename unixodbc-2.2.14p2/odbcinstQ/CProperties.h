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
#ifndef CProperties_included
#define CProperties_included

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qlayout.h>
#include <Qt/qpushbutton.h>
#include <Qt/qlineedit.h>
#include <Qt/qlabel.h>
#include <Qt/qpixmap.h>
#include <Qt/q3listbox.h>
#include <Qt/q3combobox.h>
#include <Qt/q3toolbar.h>
#include <Qt/qtooltip.h>
#include <Qt/qtoolbutton.h>
#include <Qt/qwhatsthis.h>
#include <Qt/q3mainwindow.h>
#else
#include <qlayout.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qlistbox.h>
#include <qcombobox.h>
#include <qtoolbar.h>
#include <qtooltip.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>
#include <qmainwindow.h>
#endif

#include <odbcinstext.h>

#include "CFileSelector.h"

#ifdef QT_V4LAYOUT
#define MWindow			Q3MainWindow
#define TBar			Q3ToolBar
#define CBox			Q3ComboBox
#else
#define MWindow			QMainWindow
#define TBar			QToolBar
#define CBox			QComboBox
#endif

class CProperties : public MWindow
{
    Q_OBJECT

public:
    CProperties( QWidget* parent = NULL, const char* name = NULL, HODBCINSTPROPERTY hTheFirstProperty = NULL );
    ~CProperties();

protected:
    TBar        *toolbarMain;

//	void resizeEvent( QResizeEvent *p );

protected slots:
	void pbOk_Clicked();
	void pbCancel_Clicked();
    
signals:
    void Ok();
    void Cancel();

private:
    QWidget             *pMainWidget;
	int					nProperties;
	QBoxLayout 			*pTopLayout;
	QGridLayout			*pGridLayout;
	HODBCINSTPROPERTY	hFirstProperty;

	void setCurrentItem( CBox *pComboBox, char *pszItem );
};

#endif
