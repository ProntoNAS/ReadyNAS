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
#ifndef classBrowse_included
#define classBrowse_included

#ifdef QT_V4LAYOUT
#include <Qt/qwidget.h>
#include <Qt/qevent.h>
#include <Qt/q3table.h>
#include <Qt/qlabel.h>
#include <Qt/q3listbox.h>
#include <Qt/q3valuelist.h>
#include <Qt/qfile.h>
#include <Qt/q3garray.h>
#else
#include <qwidget.h>
#include <qtable.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qvaluelist.h>
#include <qfile.h>
#include <qarray.h>
#endif

#include <sqlext.h>

// Typesdefs
#ifdef QT_V4LAYOUT
typedef Q3ValueList<int> ListRows ;
#else
typedef QValueList<int> ListRows ;
#endif

// QTable subclass that will sort columns with whole row support
#ifdef QT_V4LAYOUT
class myQTable : public Q3Table
{
public:
  myQTable( int r, int c, QWidget * w = 0, const char * n = 0 ) : Q3Table( r, c, w, n ) {} ;
  void sortColumn( int c, bool s, bool w) { Q3Table::sortColumn( c, s, TRUE ) ; }
};
#else
class myQTable : public QTable
{
public:
  myQTable( int r, int c, QWidget * w = 0, const char * n = 0 ) : QTable( r, c, w, n ) {} ;
  void sortColumn( int c, bool s, bool w) { QTable::sortColumn( c, s, TRUE ) ; }
};
#endif

#define QUERY_ROW_EXPRESSION 0
#define QUERY_ROW_SHOW       1
#define QUERY_ROW_SORT       2

class classBrowse : public QWidget
{
    Q_OBJECT

public:

    classBrowse( SQLHDBC hDbc, const QString &qsTable, const QString &qsLibrary, QWidget* parent = 0, const char* name = 0 );
    ~classBrowse() {};

    QString qsDataFileName;
    QString qsSQLFileName;
    void resizeEvent( QResizeEvent * );

public slots:
    void Clear();       // CLEAR QUERY AND DATA
    bool Save();        // SAVE DATA
    bool SaveAs();      // SAVE DATA AS
    bool SaveSQL();     // SAVE QUERY
    void Exec();        // RUN QUERY
    void SelectAllColumns();
    void UnSelectAllColumns();
    void UnSortAllColumns();
    void UnExprAllColumns();
    void DeleteRows();
    void InsertRows();
    int  AddRow();
    int  CopyRow();
    void AutoRefresh( bool );
    void WriteHTML( QFile *hFile, bool bPage );

protected:
#ifdef QT_V4LAYOUT
    Q3Table          *tableQuery;
    Q3Table          *tableData;
    Q3ListBox        *listStatus;
#else
    QTable          *tableQuery;
    QTable          *tableData;
    QListBox        *listStatus;
#endif
    QLabel          *labelFilter;
    QLabel          *labelResults;
    SQLHDBC         hDbc;
    QString         qsLibrary;
    QString         qsTable;
    bool            bAutoRefresh;

    void InitQuery();
    void ExecHeader( SQLHSTMT hStmt, SWORD nColumns );
    int  ExecBody( SQLHSTMT hStmt, SWORD nColumns );
    bool CreateSQL(QString & qsSQL );
#ifdef QT_V4LAYOUT
    void ClearCells( Q3Table *table );
#else
    void ClearCells( QTable *table );
#endif
    void ChangeAllColumns( int row, const char * text );
    int  GetSelectedRows( QString &qsRowsSelected, ListRows &listRows ) ;
    bool OkWithUser( const QString &shortDesc, const QString &qsQuestion ) ;
    int  RowAction( const QString &shortDesc, const QString &sql, const ListRows &listRows, SQLRETURN &nReturn ) ;
    bool CheckRowsAffected(const QString &shortDesc, const ListRows &listRows ) ;
    void addStatus( const QString &statusMsg ) ;
	QString GetColumnQuote( SQLHDBC hDbc );

};
#endif

