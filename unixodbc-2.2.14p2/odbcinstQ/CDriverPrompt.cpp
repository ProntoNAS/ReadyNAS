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
#include "CDriverPrompt.h"

CDriverPrompt::CDriverPrompt( QWidget* parent, const char* name )
	: QDialog( parent, name, TRUE )
{
	setCaption( "Select a Driver..." );
        setSizeGripEnabled( true );

	qsDriverName	= "";
	qsDescription		= "";
	qsDriver		= "";
	qsSetup			= "";

	pDrivers = new CDrivers( this, "Drivers" );
	pDrivers->setGeometry( 1, 25, 390, 300 );
	pDrivers->setMinimumSize( 0, 0 );
	pDrivers->setMaximumSize( 32767, 32767 );
#ifdef QT_V4LAYOUT
	pDrivers->setFocusPolicy( Qt::NoFocus );
	pDrivers->setBackgroundMode( Qt::PaletteBackground );
#else
	pDrivers->setFocusPolicy( QWidget::NoFocus );
	pDrivers->setBackgroundMode( QWidget::PaletteBackground );
#endif
    pDrivers->show();

	QLabel* qtarch_Label_10;
	qtarch_Label_10 = new QLabel( this, "Label_10" );
	qtarch_Label_10->setGeometry( 10, 10, 400, 20 );
	qtarch_Label_10->setMinimumSize( 0, 0 );
	qtarch_Label_10->setMaximumSize( 32767, 32767 );
#ifdef QT_V4LAYOUT
	qtarch_Label_10->setFocusPolicy( Qt::NoFocus );
	qtarch_Label_10->setBackgroundMode( Qt::PaletteBackground );
#else
	qtarch_Label_10->setFocusPolicy( QWidget::NoFocus );
	qtarch_Label_10->setBackgroundMode( QWidget::PaletteBackground );
#endif
	qtarch_Label_10->setText( "Select the DRIVER to use or Add a new one..." );
	qtarch_Label_10->setAlignment( 289 );
	qtarch_Label_10->setMargin( -1 );

	QFrame *fra	= new QFrame( this );
	fra->setGeometry( 400, 10, 2, 300 );
	fra->setFrameStyle( 33 );


	QPushButton* qtarch_pbOk;
	qtarch_pbOk = new QPushButton( this, "pbOk" );
	qtarch_pbOk->setGeometry( 420, 230, 100, 30 );
	qtarch_pbOk->setMinimumSize( 0, 0 );
	qtarch_pbOk->setMaximumSize( 32767, 32767 );
	connect( qtarch_pbOk, SIGNAL(clicked()), SLOT(pbOk_Clicked()) );
#ifdef QT_V4LAYOUT
	qtarch_pbOk->setFocusPolicy( Qt::TabFocus );
	qtarch_pbOk->setBackgroundMode( Qt::PaletteBackground );
#else
	qtarch_pbOk->setFocusPolicy( QWidget::TabFocus );
	qtarch_pbOk->setBackgroundMode( QWidget::PaletteBackground );
#endif
	qtarch_pbOk->setText( "&Ok" );
	qtarch_pbOk->setAutoRepeat( FALSE );
#ifndef QT_V4LAYOUT
	qtarch_pbOk->setAutoResize( FALSE );
#endif

	QPushButton* qtarch_pbCancel;
	qtarch_pbCancel = new QPushButton( this, "pbCancel" );
	qtarch_pbCancel->setGeometry( 420, 270, 100, 30 );
	qtarch_pbCancel->setMinimumSize( 0, 0 );
	qtarch_pbCancel->setMaximumSize( 32767, 32767 );
	connect( qtarch_pbCancel, SIGNAL(clicked()), SLOT(pbCancel_Clicked()) );
#ifdef QT_V4LAYOUT
	qtarch_pbCancel->setFocusPolicy( Qt::TabFocus );
	qtarch_pbCancel->setBackgroundMode( Qt::PaletteBackground );
#else
	qtarch_pbCancel->setFocusPolicy( QWidget::TabFocus );
	qtarch_pbCancel->setBackgroundMode( QWidget::PaletteBackground );
#endif
	qtarch_pbCancel->setText( "&Cancel" );
	qtarch_pbCancel->setAutoRepeat( FALSE );
#ifndef QT_V4LAYOUT
	qtarch_pbCancel->setAutoResize( FALSE );
#endif

	resize( 530,335 );
	setMinimumSize( 0, 0 );
	setMaximumSize( 32767, 32767 );
}


CDriverPrompt::~CDriverPrompt()
{
}

/********************************************
 * protected slots
 ********************************************/

void CDriverPrompt::pbCancel_Clicked()
{
	reject();
}

void CDriverPrompt::pbOk_Clicked()
{
#ifdef QT_V4LAYOUT
    Q3ListView		*pListView;
    Q3ListViewItem	*pListViewItem;
#else
    QListView		*pListView;
    QListViewItem	*pListViewItem;
#endif

	pListView		= pDrivers->getListView();
	pListViewItem	= pListView->currentItem();

	if ( pListViewItem )
	{
		qsDriverName	= pListViewItem->text( 0 );
		qsDescription		= pListViewItem->text( 1 );
		qsDriver		= pListViewItem->text( 2 );
		qsSetup			= pListViewItem->text( 3 );
		if ( qsDriverName == "" )
			QMessageBox::information(	this, "ODBC Config",  "Please select a listing which contains a Driver file name" );
		else
			accept();
	}
	else
		QMessageBox::information(	this, "ODBC Config",  "Please select a Driver from the list or click Cancel" );
}



