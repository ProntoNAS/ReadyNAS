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

#include "classConnectionFrame.h"
#ifdef QT_V4LAYOUT
#include <Qt/q3toolbar.h>
#include <Qt/qmenubar.h>
#include <Qt/qtoolbutton.h>
#include <Qt/qwhatsthis.h>
#else
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qtoolbutton.h>
#include <qwhatsthis.h>
#endif
#include "run.xpm"
#include "new.xpm"
#include "open.xpm"
#include "save.xpm"

classConnectionFrame::classConnectionFrame( SQLHDBC hDbc, QString qsDataSource, QWidget *parent, const char *name )
#ifdef QT_V4LAYOUT
        : Q3MainWindow( parent, name, 0 )
#else
        : QMainWindow( parent, name, 0 )
#endif
{
    // CLIENT AREA
    isql = new classISQL( hDbc, qsDataSource, this );
    setCentralWidget( isql );

    // SETUP MAIN MENU
    QMenuBar *menubarMain = new QMenuBar( this );

    // FILE MENU
#ifdef QT_V4LAYOUT
    Q3PopupMenu *pFile = new Q3PopupMenu();
    pFile->insertItem( QPixmap( new_xpm  ), tr("&New" )   , isql, SLOT(NewSQL()   ), Qt::CTRL+Qt::Key_N );
    pFile->insertItem( QPixmap( open_xpm ), tr("&Open")   , isql, SLOT(OpenSQL()  ), Qt::CTRL+Qt::Key_O );
    pFile->insertItem( QPixmap( save_xpm ), tr("&Save")   , isql, SLOT(SaveSQL()  ), Qt::CTRL+Qt::Key_S );
    pFile->insertItem( QPixmap( save_xpm ), tr("Save &As"), isql, SLOT(SaveAsSQL()), Qt::CTRL+Qt::Key_A );
    pFile->insertItem( QPixmap( run_xpm  ), tr("&Run")    , this, SLOT(Exec()     ), Qt::CTRL+Qt::Key_R );
#else
    QPopupMenu *pFile = new QPopupMenu();
    pFile->insertItem( QPixmap( new_xpm  ), tr("&New" )   , isql, SLOT(NewSQL()   ), CTRL+Key_N );
    pFile->insertItem( QPixmap( open_xpm ), tr("&Open")   , isql, SLOT(OpenSQL()  ), CTRL+Key_O );
    pFile->insertItem( QPixmap( save_xpm ), tr("&Save")   , isql, SLOT(SaveSQL()  ), CTRL+Key_S );
    pFile->insertItem( QPixmap( save_xpm ), tr("Save &As"), isql, SLOT(SaveAsSQL()), CTRL+Key_A );
    pFile->insertItem( QPixmap( run_xpm  ), tr("&Run")    , this, SLOT(Exec()     ), CTRL+Key_R );
#endif
    menubarMain->insertItem( tr("&File"), pFile );

    // VIEW MENU
#ifdef QT_V4LAYOUT
    pView = new Q3PopupMenu();
#else
    pView = new QPopupMenu();
#endif
    pView->insertItem( tr("Text Table"),     this, SLOT(setViewText()         ), 0, classISQL::Text          );
    pView->insertItem( tr("Text Delimited"), this, SLOT(setViewTextDelimited()), 0, classISQL::TextDelimited );
    pView->insertItem( tr("HTML Source"),    this, SLOT(setViewHTMLSource()   ), 0, classISQL::HTMLSource    );
    pView->setItemChecked( view=classISQL::Text, true );
    menubarMain->insertItem( tr("&View"), pView );

    // DUMMY DESCRIPTION MENU
    QString ds ; ds.sprintf("  [%s]  ", qsDataSource.ascii() ) ;
    menubarMain->insertItem( ds );

    // SETUP TOOLBAR
#ifdef QT_V4LAYOUT
    Q3ToolBar *toolbarMain = new Q3ToolBar( this );
    addToolBar( toolbarMain, tr( "ToolBar" ), Qt::Top, FALSE );
#else
    QToolBar *toolbarMain = new QToolBar( this );
    addToolBar( toolbarMain, tr( "ToolBar" ), Top, FALSE );
#endif
    new QToolButton( QPixmap( new_xpm  ), QString(tr("New" )), "", isql, SLOT(NewSQL() ), toolbarMain );
    new QToolButton( QPixmap( open_xpm ), QString(tr("Open")), "", isql, SLOT(OpenSQL()), toolbarMain );
    new QToolButton( QPixmap( save_xpm ), QString(tr("Save")), "", isql, SLOT(SaveSQL()), toolbarMain );
    new QToolButton( QPixmap( run_xpm  ), QString(tr("Run" )), "", this, SLOT(Exec()   ), toolbarMain );
    QWhatsThis::whatsThisButton ( toolbarMain );

    // RESIZE
#ifdef QT_V4LAYOUT
	connect( parent, SIGNAL(changedSize(int,int)), SLOT(Resize(int,int)) );
#else
    connect( parent, SIGNAL(changedSize(int,int)), SLOT(resize(int,int)) );
#endif
    resize( parent->size() );
    setMinimumSize( 50, 50 );
    setMaximumSize( 32767, 32767 );
}

void classConnectionFrame::resizeEvent( QResizeEvent *p )
{
    resize( p->size() );
}

void classConnectionFrame::Exec()
{
    isql->ExecSQL( view ) ;
}

void classConnectionFrame::setViewHTMLSource()
{
    pView->setItemChecked( view, false );
    view = classISQL::HTMLSource;
    pView->setItemChecked( view, true );
}

void classConnectionFrame::setViewText()
{
    pView->setItemChecked( view, false );
    view = classISQL::Text;
    pView->setItemChecked( view, true );
}

void classConnectionFrame::setViewTextDelimited()
{
    pView->setItemChecked( view, false );
    view = classISQL::TextDelimited;
    pView->setItemChecked( view, true );
}

void classConnectionFrame::Resize( int x, int y )
{
	resize( x, y );
}
