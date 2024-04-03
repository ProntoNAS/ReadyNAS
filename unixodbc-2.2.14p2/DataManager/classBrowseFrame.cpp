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
#include "classBrowseFrame.h"

#include "run.xpm"
#include "new.xpm"
#include "open.xpm"
#include "save.xpm"

classBrowseFrame::classBrowseFrame( SQLHDBC hDbc, QString qsTable, QWidget *parent, const char *name )
#ifdef QT_V4LAYOUT
	: Q3MainWindow( parent, name, 0 )
#else
	: QMainWindow( parent, name, 0 )
#endif
{
    QString         qsCaption;

    this->hDbc	 	= hDbc;
    this->qsTable	= qsTable;
    qsCaption.sprintf( "Browse %s", (const char *)qsTable );
    setCaption( qsCaption );
    
    // CLIENT AREA
    browse = new classBrowse( hDbc, qsTable, this ); 
    setCentralWidget( browse );

    // SETUP MAIN MENU
    menubarMain = new QMenuBar( this );
    menubarMain->setFrameStyle( QFrame::NoFrame );

#ifdef QT_V4LAYOUT
    Q3PopupMenu *pFile = new Q3PopupMenu();
#else
    QPopupMenu *pFile = new QPopupMenu();
#endif
    pFile->insertItem( QPixmap( new_xpm ), tr("Clear"),  this, SLOT(Clear()) );
#ifdef QT_V4LAYOUT
    pFile->insertItem( QPixmap( save_xpm ), tr("&Save"), this, SLOT(Save()), Qt::CTRL+Qt::Key_S );
    pFile->insertItem( tr("Save &As"), this, SLOT(SaveAs()), Qt::CTRL+Qt::Key_A );
#else
    pFile->insertItem( QPixmap( save_xpm ), tr("&Save"), this, SLOT(Save()), CTRL+Key_S );
    pFile->insertItem( tr("Save &As"), this, SLOT(SaveAs()), CTRL+Key_A );
#endif
    menubarMain->insertItem( tr("&File"), pFile );
    
#ifdef QT_V4LAYOUT
    Q3PopupMenu *pEdit = new Q3PopupMenu();
#else
    QPopupMenu *pEdit = new QPopupMenu();
#endif
    pEdit->insertItem( tr("Se&lect All Columns"),  browse, SLOT(SelectAllColumns()) );
    pEdit->insertItem( tr("&Unselect All Columns"),  browse, SLOT(UnSelectAllColumns()) );
    menubarMain->insertItem( tr("&Edit"), pEdit );
    
    menubarMain->setSeparator( QMenuBar::InWindowsStyle );

    // SETUP TOOLBAR
#ifdef QT_V4LAYOUT
    toolbarMain = new Q3ToolBar( this );
    addToolBar( toolbarMain, tr( "ToolBar" ), Qt::Top, FALSE );
#else
    toolbarMain = new QToolBar( this );
    addToolBar( toolbarMain, tr( "ToolBar" ), Top, FALSE );
#endif

    QToolButton *toolbutton = new QToolButton( QPixmap( new_xpm ), QString(tr("Clear")), QString(""), this, SLOT(Clear()), toolbarMain );
    toolbutton = new QToolButton( QPixmap( save_xpm ), QString(tr("Save")), QString(""), this, SLOT(Save()), toolbarMain );
    toolbutton = new QToolButton( QPixmap( run_xpm ), QString(tr("Run")), QString(""), this, SLOT(Exec()), toolbarMain );
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


classBrowseFrame::~classBrowseFrame()
{
}

void classBrowseFrame::resizeEvent( QResizeEvent *p )
{
    resize( p->size() );
}


void classBrowseFrame::Clear()         // CLEAR QUERY AND DATA
{
    browse->Clear();
}

void classBrowseFrame::Save()        // DATA
{
    browse->Save();
}

void classBrowseFrame::SaveAs()      // DATA
{
    browse->SaveAs();
}

void classBrowseFrame::Exec()        // RUN QUERY
{
    browse->Exec();
}

void classBrowseFrame::Resize( int x, int y )
{
	resize( x, y );
}

