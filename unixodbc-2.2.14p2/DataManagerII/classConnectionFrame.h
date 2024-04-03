/**************************************************
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 30.NOV.00
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey   - pharvey@codebydesign.com
 **************************************************/
#ifndef classConnectionFrame_included
#define classConnectionFrame_included

#include "classISQL.h"
#ifdef QT_V4LAYOUT
#include <Qt/qwidget.h>
#include <Qt/qevent.h>
#include <Qt/q3mainwindow.h>
#include <Qt/q3popupmenu.h>
#else
#include <qwidget.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#endif
#include <sqlext.h>

#ifdef QT_V4LAYOUT
#define LView			Q3ListView
#define LViewItem		Q3ListViewItem
#define MWindow			Q3MainWindow
#define PMenu			Q3PopupMenu
#else
#define LView			QListView
#define LViewItem		QListViewItem
#define MWindow			QMainWindow
#define PMenu			QPopupMenu
#endif

class classConnectionFrame : public MWindow
{
    Q_OBJECT

public:

    classConnectionFrame( SQLHDBC hDbc, QString qsDataSource, QWidget* parent = 0, const char* name = 0 );
   ~classConnectionFrame() {}

    void resizeEvent( QResizeEvent * );

public slots:
    void Exec();
    void setViewHTMLSource();
    void setViewText();
    void setViewTextDelimited();
    void Resize(int,int); 

protected:
    PMenu *    		pView;
    classISQL  *    isql;
    classISQL::View view;
};
#endif

