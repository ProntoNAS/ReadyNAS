/**************************************************
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 18.FEB.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey   - pharvey@codebydesign.com
 **************************************************/
#ifndef classISQL_included
#define classISQL_included

#ifdef QT_V4LAYOUT
#include <Qt/qwidget.h>
#include <Qt/qmenubar.h>
#include <Qt/q3combobox.h>
#include <Qt/qslider.h>
#include <Qt/qtabbar.h>
#include <Qt/qlabel.h>
#include <Qt/qfile.h>
#include <Qt/qfiledialog.h>
#include <Qt/qtextstream.h>
#include <Qt/q3valuelist.h>
#include <Qt/q3multilineedit.h>
#include <Qt/qlayout.h>
#include <Qt/qstring.h>
#include <Qt/q3memarray.h>
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
#include <qlabel.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qtextstream.h>
#include <qvaluelist.h>
#include <qmultilineedit.h>
#include <qlayout.h>
#include <qstring.h>
#include <qarray.h>
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
    ~classISQL() {}

    enum View { Text=1, TextDelimited=2, HTMLSource=3 } ;

public slots:
    void ExecSQL( View );
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
    Q3ListBox *          listStatus;
    Q3ValueList<QString> listSQL;
#else
    QMultiLineEdit *    txtSQL;
    QMultiLineEdit *    txtResults;
    QListBox *          listStatus;
    QValueList<QString> listSQL;
#endif
    SQLHDBC             hDbc;
    QString             qsDataSource;
    QString             qsResultsFileName;
    QString             qsSQLFileName;
    int                 nSQL;

private:
#ifdef QT_V4LAYOUT
    void getResultsHeader( SQLHSTMT hStmt, SWORD nColumns, QString &qsHorizSep, Q3MemArray<int> &colWidths );
    int getResultsBody( SQLHSTMT hStmt, SWORD nColumns, const QString &qsHorizSep, const Q3MemArray<int> &colWidths );
#else
    void getResultsHeader( SQLHSTMT hStmt, SWORD nColumns, QString &qsHorizSep, QArray<int> &colWidths );
    int getResultsBody( SQLHSTMT hStmt, SWORD nColumns, const QString &qsHorizSep, const QArray<int> &colWidths );
#endif
    void appendHistoryItem();
    void addStatus( const QString &statusMsg ) ;
};
#endif

