/**************************************************
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 18.FEB.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 **************************************************/
#ifndef classISQL_included
#define classISQL_included

#ifdef QT_V4LAYOUT
#include <Qt/qwidget.h>
#include <Qt/qmenubar.h>
#include <Qt/q3combobox.h>
#include <Qt/qslider.h>
#include <Qt/qtabbar.h>
#include <Qt/qfile.h>
#include <Qt/qevent.h>
#include <Qt/qfiledialog.h>
#include <Qt/q3textstream.h>
#include <Qt/qpixmap.h>
#include <Qt/qlayout.h>
#include <Qt/qpushbutton.h>
#include <Qt/qmessagebox.h>
#include <Qt/qlabel.h>
#include <Qt/q3valuelist.h>
#include <Qt/q3multilineedit.h>
#include <Qt/qstyle.h>
#else
#include <qwidget.h>
#include <qmenubar.h>
#include <qcombo.h>
#include <qslider.h>
#include <qtabbar.h>
#if (QT_VERSION>=300)
#else
#include <qmlined.h>
#endif
#include <qfile.h>
#include <qfiledialog.h>
#include <qtextstream.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qpushbt.h>
#include <qkeycode.h>
#include <qmessagebox.h>
#include <qlabel.h>
#include <qvaluelist.h>
#include <qmultilineedit.h>
#endif

#include <sqlext.h>

#ifndef max
#define max( a, b ) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min( a, b ) (((a) > (b)) ? (b) : (a))
#endif

class classISQL : public QWidget
{
    Q_OBJECT

public:

    classISQL( SQLHDBC hDbc, QString qsDataSource, QWidget* parent = 0, const char* name = 0 );
    ~classISQL();

    void resizeEvent( QResizeEvent * );

public slots:
    void ExecSQL();
    void NewSQL();
    void OpenSQL();
    void SaveSQL();
    void SaveAsSQL();

protected slots:
    void ChangeTextType( int nTab );
    void gotoHistoryItem( int nValue );

protected:
    QTabBar *           pTabBar;
    QSlider *           pSliderRecentSQL;
#ifdef QT_V4LAYOUT
    Q3MultiLineEdit *    txtSQL;
    Q3MultiLineEdit *    txtResults;
#else
    QMultiLineEdit *    txtSQL;
    QMultiLineEdit *    txtResults;
#endif
    SQLHDBC             hDbc;
    QString             qsDataSource;
    QString             qsResultsFileName;
    QString             qsSQLFileName;
#ifdef QT_V4LAYOUT
    Q3ValueList<QString> listSQL;
#else
    QValueList<QString> listSQL;
#endif
    int                 nSQL;

private:
    int getResultsHeader( SQLHSTMT hStmt, SWORD nColumns, QString *pqsHorizSep );
    int getResultsBody( SQLHSTMT hStmt, SWORD nColumns );
    void setTextType( int nIndex );
    void appendHistoryItem();
};

#endif


