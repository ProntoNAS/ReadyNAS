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
#include "classConnectionFrame.h"

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
    QString         qsCaption;

	qsCaption.sprintf( "Connected to %s", (const char *)qsDataSource );
    setCaption( qsCaption );
 
    // CLIENT AREA
    isql = new classISQL( hDbc, qsDataSource, this ); 
    setCentralWidget( isql );

    // SETUP MAIN MENU
    menubarMain = new QMenuBar( this );
    menubarMain->setFrameStyle( QFrame::NoFrame );

#ifdef QT_V4LAYOUT
    Q3PopupMenu *pFile = new Q3PopupMenu();
#else
    QPopupMenu *pFile = new QPopupMenu();
#endif
#ifdef QT_V4LAYOUT
    pFile->insertItem( QPixmap( new_xpm ), tr("&New"),  this, SLOT(New()), Qt::CTRL+Qt::Key_N );
    pFile->insertItem( QPixmap( open_xpm ), tr("&Open"),  this, SLOT(Open()), Qt::CTRL+Qt::Key_O );
    pFile->insertItem( QPixmap( save_xpm ), tr("&Save"), this, SLOT(Save()), Qt::CTRL+Qt::Key_S );
    pFile->insertItem( tr("Save &As"), this, SLOT(SaveAs()), Qt::CTRL+Qt::Key_A );
    pFile->insertItem( QPixmap( run_xpm ), tr("&Run"), this, SLOT(Exec()), Qt::CTRL+Qt::Key_R );
#else
    pFile->insertItem( QPixmap( new_xpm ), tr("&New"),  this, SLOT(New()), CTRL+Key_N );
    pFile->insertItem( QPixmap( open_xpm ), tr("&Open"),  this, SLOT(Open()), CTRL+Key_O );
    pFile->insertItem( QPixmap( save_xpm ), tr("&Save"), this, SLOT(Save()), CTRL+Key_S );
    pFile->insertItem( tr("Save &As"), this, SLOT(SaveAs()), CTRL+Key_A );
    pFile->insertItem( QPixmap( run_xpm ), tr("&Run"), this, SLOT(Exec()), CTRL+Key_R );
#endif
    menubarMain->insertItem( tr("&File"), pFile );
    
#ifdef QT_V4LAYOUT
    pView = new Q3PopupMenu();
#else
    pView = new QPopupMenu();
#endif
    nView = nViewGUI    = pView->insertItem( tr("GUI Table"), this, SLOT(setViewGUI()) );
    nViewText           = pView->insertItem( tr("Text Table"), this, SLOT(setViewText()) );
    nViewTextDelimited  = pView->insertItem( tr("Text Delimited"), this, SLOT(setViewTextDelimited()) );
    nViewHTML           = pView->insertItem( tr("HTML Table"), this, SLOT(setViewHTML()) );
    nViewHTMLSource     = pView->insertItem( tr("HTML Source"), this, SLOT(setViewHTMLSource()) );
    pView->setItemChecked( nView, true );
    menubarMain->insertItem( tr("&View"), pView );

    menubarMain->setSeparator( QMenuBar::InWindowsStyle );

    // SETUP TOOLBAR
#ifdef QT_V4LAYOUT
    toolbarMain = new Q3ToolBar( this );
    addToolBar( toolbarMain, tr( "ToolBar" ), Qt::Top, FALSE );
#else
    toolbarMain = new QToolBar( this );
    addToolBar( toolbarMain, tr( "ToolBar" ), Top, FALSE );
#endif

    QToolButton *toolbutton = new QToolButton( QPixmap( new_xpm ), QString(tr("New")), QString(""), this, SLOT(New()), toolbarMain );
    toolbutton = new QToolButton( QPixmap( open_xpm ), QString(tr("Open")), QString(""), this, SLOT(Open()), toolbarMain );
    toolbutton = new QToolButton( QPixmap( save_xpm ), QString(tr("Save")), QString(""), this, SLOT(Save()), toolbarMain );
    toolbutton = new QToolButton( QPixmap( run_xpm ), QString(tr("Run")), QString(""), this, SLOT(Exec()), toolbarMain );
    QWhatsThis::whatsThisButton ( toolbarMain );

    // STATUS BAR
//    statusbarMain = new QStatusBar( this );

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


classConnectionFrame::~classConnectionFrame()
{
}

void classConnectionFrame::resizeEvent( QResizeEvent *p )
{
    resize( p->size() );
}


void classConnectionFrame::New()         // CLEAR QUERY AND DATA
{
    isql->NewSQL();
}

void classConnectionFrame::Open()         // CLEAR QUERY AND DATA
{
    isql->OpenSQL();
}

void classConnectionFrame::Save()        // DATA
{
    isql->SaveSQL();
}

void classConnectionFrame::SaveAs()      // DATA
{
    isql->SaveAsSQL();
}

void classConnectionFrame::Exec()        // RUN QUERY
{
    isql->ExecSQL();
}

void classConnectionFrame::setViewHTML()
{
    pView->setItemChecked( nView, false );
    nView = nViewHTML;
    pView->setItemChecked( nView, true );
}

void classConnectionFrame::setViewHTMLSource()
{
    pView->setItemChecked( nView, false );
    nView = nViewHTMLSource;
    pView->setItemChecked( nView, true );
}

void classConnectionFrame::setViewGUI()
{
    pView->setItemChecked( nView, false );
    nView = nViewGUI;
    pView->setItemChecked( nView, true );
}

void classConnectionFrame::setViewText()
{
    pView->setItemChecked( nView, false );
    nView = nViewText;
    pView->setItemChecked( nView, true );
}

void classConnectionFrame::setViewTextDelimited()
{
    pView->setItemChecked( nView, false );
    nView = nViewTextDelimited;
    pView->setItemChecked( nView, true );
}

void classConnectionFrame::Resize( int x, int y )
{
	resize( x, y );
}
