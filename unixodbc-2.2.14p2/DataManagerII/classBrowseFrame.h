/**************************************************
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 30.NOV.00
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey         - pharvey@codebydesign.com
 **************************************************/
#ifndef classBrowseFrame_included
#define classBrowseFrame_included

#include "classBrowse.h"
#ifdef QT_V4LAYOUT
#include <Qt/qwidget.h>
#include <Qt/qevent.h>
#include <Qt/q3mainwindow.h>
#include <Qt/q3popupmenu.h>
#include <Qt/qstring.h>
#else
#include <qwidget.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qstring.h>
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

class classBrowseFrame : public MWindow
{
    Q_OBJECT

public:

    classBrowseFrame( SQLHDBC hDbc, const QString &qsTable, const QString &qsLibrary, QWidget* parent = 0, const char* name = 0 );
    ~classBrowseFrame() {}

    void resizeEvent( QResizeEvent * );

public slots:
    void autoRefresh();
    void Resize(int,int); 

protected:
    classBrowse *browse;
    PMenu  		*pResults;
    int         nAutoRefresh;
};
#endif

