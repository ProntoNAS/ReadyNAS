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
#ifndef classConnectionFrame_included
#define classConnectionFrame_included

#ifdef QT_V4LAYOUT
#include <Qt/qwidget.h>
#include <Qt/qevent.h>
#include <Qt/qlayout.h>
#include <Qt/q3table.h>
#include <Qt/qmessagebox.h>
#include <Qt/qnamespace.h>
#include <Qt/qwmatrix.h>
#include <Qt/q3mainwindow.h>
#include <Qt/q3toolbar.h>
#include <Qt/q3popupmenu.h>
#include <Qt/qtoolbutton.h>
#include <Qt/qmenubar.h>
#include <Qt/qstatusbar.h>
#include <Qt/qwhatsthis.h>
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
#include <qstatusbar.h>
#include <qwhatsthis.h>
#endif

#include <sqlext.h>

#include "classISQL.h"

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

class classConnectionFrame : public MWindow
{
    Q_OBJECT

public:

    classConnectionFrame( SQLHDBC hDbc, QString qsDataSource, QWidget* parent = 0, const char* name = 0 );
    ~classConnectionFrame();

    void resizeEvent( QResizeEvent * );

public slots:
    void New();         // CLEAR QUERY AND DATA
    void Open();
    void Save();        // DATA
    void SaveAs();      // DATA
    void Exec();        // RUN QUERY
    void setViewHTML();
    void setViewHTMLSource();
    void setViewGUI();
    void setViewText();
    void setViewTextDelimited();
    void Resize(int,int); 

protected:
    QMenuBar *      menubarMain;
    TBar *      	toolbarMain;
    PMenu *    		pView;
    QStatusBar *    statusbarMain;
    classISQL *     isql;
    int             nViewHTML;
    int             nViewHTMLSource;
    int             nViewGUI;
    int             nViewText;
    int             nViewTextDelimited;
    int             nView;
};

#endif


