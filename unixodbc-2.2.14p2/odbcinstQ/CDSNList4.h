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
#ifndef CDSNList_included
#define CDSNList_included

#define QT3_SUPPORT

#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <ini.h>
#include <odbcinstext.h>

#include <Qt/qapplication.h>
#include <Qt/qmessagebox.h>
#include <Qt/qwidget.h>
#include <Qt/q3listview.h>
#include <Qt/qstring.h>
#include <Qt/qpalette.h>
#include <Qt/qpainter.h>

#include "CDriverPrompt.h"
#include "CPropertiesFrame.h"
#include "CDLL.h"

#define ODBC_HELP_DSN_NAME "*Unique* DSN name. This is the name you use when using ODBC with applications such as StarOffice. Try to keep unusual characters and spaces out of the name."
#define ODBC_HELP_DSN_DESC "DSN description. A long, perhaps more meaningfull name."
#define ODBC_HELP_DSN_UNKNOWN "No help for this DSN property. Please check with the vendor of the driver... perhaps their web site"

#ifdef QT_V4LAYOUT
#define LView			Q3ListView
#define LViewItem		Q3ListViewItem
#else
#define LView			QListView
#define LViewItem		QListViewItem
#endif

class CDSNList : public LView
{
    Q_OBJECT

public:
    CDSNList( QWidget* parent = NULL, const char* name = NULL );
    ~CDSNList();
	
	void Load( int nSource );

public slots:
	void Add();
	void Edit();
	void Delete();
    void DoubleClick( Q3ListViewItem *itm );

private:
	int nSource;
};

#endif
