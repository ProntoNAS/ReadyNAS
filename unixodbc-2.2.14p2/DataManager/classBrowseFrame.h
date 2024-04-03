/**************************************************
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 30.NOV.00
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 **************************************************/
#ifndef classBrowseFrame_included
#define classBrowseFrame_included

#ifdef QT_V4LAYOUT
#include <Qt/qwidget.h>
#include <Qt/qlayout.h>
#include <Qt/q3table.h>
#include <Qt/qmessagebox.h>
#include <Qt/qwmatrix.h>
#include <Qt/q3mainwindow.h>
#include <Qt/q3toolbar.h>
#include <Qt/qtoolbutton.h>
#include <Qt/qmenubar.h>
#include <Qt/q3whatsthis.h>
#include <Qt/q3popupmenu.h>
#else
#include <qwidget.h>
#include <qlayout.h>
#include <qtable.h>
#include <qmessagebox.h>
#include <qwmatrix.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qmenubar.h>
#include <qwhatsthis.h>
#endif

#include <sqlext.h>

#include "classBrowse.h"

#ifdef QT_V4LAYOUT
#define LView			Q3ListView
#define LViewItem		Q3ListViewItem
#define MWindow			Q3MainWindow
#define TBar			Q3ToolBar
#define PMenu			Q3PopupMenu
#else
#define LView			QListView
#define LViewItem		QListViewItem
#define MWindow			QMainWindow
#define TBar			QToolBar
#define PMenu			QPopupMenu
#endif

class classBrowseFrame : public MWindow
{
    Q_OBJECT

public:

    classBrowseFrame( SQLHDBC hDbc, QString qsTable, QWidget* parent = 0, const char* name = 0 );
    ~classBrowseFrame();

    void resizeEvent( QResizeEvent * );

public slots:
    void Clear();         // CLEAR QUERY AND DATA
    void Save();        // DATA
    void SaveAs();      // DATA
    void Exec();        // RUN QUERY
    void Resize(int,int); 

protected:
    QMenuBar        *menubarMain;
    TBar        	*toolbarMain;
    classBrowse     *browse;
	SQLHDBC 		hDbc;
    QString     	qsTable;

};

#endif


