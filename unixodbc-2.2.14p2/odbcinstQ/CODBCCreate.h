/**************************************************
 * 
 *
 **************************************************
 * This code was created by Nick Gorham nick.gorham@easysoft.com
 * Released under GPL 22.MAY.2006
 *
 * Contributions from...
 * -----------------------------------------------
 *
 **************************************************/
#ifndef CODBCCreate_included
#define CODBCCreate_included

#include <sql.h>
#include <sqlext.h>
#include <odbcinstext.h>

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qwidget.h>
#include <Qt/qmessagebox.h>
#include <Qt/q3wizard.h>
#include <Qt/q3vbox.h>
#include <Qt/q3hbox.h>
#include <Qt/qgroupbox.h>
#include <Qt/qlabel.h>
#include <Qt/q3buttongroup.h>
#include <Qt/qradiobutton.h>
#include <Qt/qcheckbox.h>
#include <Qt/q3listview.h>
#include <Qt/qpushbutton.h>
#include <Qt/qlineedit.h>
#include <Qt/qtextedit.h>
#include <Qt/q3filedialog.h>
#include <Qt/qmessagebox.h>
#include <Qt/qsettings.h>
#else
#include <qwidget.h>
#include <qmessagebox.h>
#include <qwizard.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlistview.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#if QT_VERSION>=300
#include <qtextedit.h>
#endif
#include <qfiledialog.h>
#include <qmessagebox.h>
#if QT_VERSION>=300
#include <qsettings.h>
#endif
#endif

#ifdef QT_V4LAYOUT
#define LView			Q3ListView
#define LViewItem		Q3ListViewItem
#define Wiz				Q3Wizard
#else
#define LView			QListView
#define LViewItem		QListViewItem
#define Wiz				QWizard
#endif

#if (QT_VERSION<300)

class QTextEdit : public QMultiLineEdit
{
public:
    QTextEdit (QWidget *, const char *);
    QTextEdit (QWidget *);
    void setMaxLength( int x );
    void append( const char *str );
    void maxLines( int n );
};

#endif

class CODBCCreate : public Wiz
{
    Q_OBJECT

public:
    CODBCCreate( QWidget* parent = 0, const char* name = 0 );
    virtual ~CODBCCreate();
	void setDsn( const char *dsn );
	int getRetCode();

	void setValid( bool val );
	void setKeywords( QString kw );

public slots:
	void fds_click();
	void uds_click();
	void sds_click();
	void file_click();
	void ad_click();
	void dl_click(LViewItem*);
	void page_change(const QString &title );
	void file_changed(const QString &text);

protected:
	void setupPage1();
	void setupPage2();
	void setupPage3();
	void setupPage4();

	void populate();

	bool createDsn();

	QLabel *lab;
#ifdef QT_V4LAYOUT
	Q3VBox *box, *box1, *box2;
	Q3HBox *box3;
	Q3HBox *box1a;
	Q3VButtonGroup *bg;
#else
	QVBox *box, *box1, *box2;
	QHBox *box3;
	QHBox *box1a;
	QVButtonGroup *bg;
#endif
	QGroupBox *gb;
	QPushButton *file_find, *advanced;
	QRadioButton *fds, *sds, *uds;
	LView *driver_list;
	QString current_driver, dsn;
	QTextEdit *arg_list;
	QLineEdit *file_edit;
	QString extra_keywords;

	SQLRETURN ret_code;
	bool verify;

    virtual void LoadState();
    virtual void SaveState();

	bool appropriate ( QWidget *page ) const;
	void accept ();
	void reject ();
};

class CODBCAdvanced : public QDialog
{
    Q_OBJECT

public:
#ifdef QT_V4LAYOUT
    CODBCAdvanced( QWidget* parent = 0, const char* name = 0, Qt::WFlags nFlags = 0 );
#else
    CODBCAdvanced( QWidget* parent = 0, const char* name = 0, WFlags nFlags = 0 );
#endif
    virtual ~CODBCAdvanced();

	void setValid( bool val );
	void setKeywords( QString kw );

public slots:
	void ad_ok();

protected:

	QPushButton *ok, *cancel;
	CODBCCreate *parent;
	QLabel *lab;
	QTextEdit *text_list;
	QCheckBox *valid;
};

#endif 
