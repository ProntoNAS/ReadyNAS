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
#ifndef classBrowse_included
#define classBrowse_included

#ifdef QT_V4LAYOUT
#include <Qt/qwidget.h>
#include <Qt/qevent.h>
#include <Qt/qlayout.h>
#include <Qt/q3table.h>
#include <Qt/q3header.h>
#include <Qt/qlabel.h>
#include <Qt/qmessagebox.h>
#include <Qt/qwmatrix.h>
#include <Qt/qwhatsthis.h>
#include <Qt/qfile.h>
#include <Qt/qfiledialog.h>
#else
#include <qwidget.h>
#include <qlayout.h>
#include <qtable.h>
#include <qheader.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qwmatrix.h>
#include <qwhatsthis.h>
#include <qfile.h>
#include <qfiledialog.h>
#endif

#include <sqlext.h>

#define QUERY_ROW_EXPRESSION 0
#define QUERY_ROW_SHOW 1

class classBrowse : public QWidget
{
    Q_OBJECT

public:

    classBrowse( SQLHDBC hDbc, QString qsTable, QWidget* parent = 0, const char* name = 0 );
    ~classBrowse();

    QString qsDataFileName;
    void resizeEvent( QResizeEvent * );
    
public slots:
    void Clear();         // CLEAR QUERY AND DATA
    bool Save();        // SAVE DATA
    bool SaveAs();      // SAVE DATA AS
    void Exec();        // RUN QUERY
    void SelectAllColumns();
    void UnSelectAllColumns();
    void WriteHTML( QFile *hFile, bool bPage );

protected:
    QVBoxLayout     *layoutMain;
#ifdef QT_V4LAYOUT
    Q3Table          *tableQuery;
    Q3Table          *tableData;
#else
    QTable          *tableQuery;
    QTable          *tableData;
#endif
	SQLHDBC 		hDbc;
    QString     	qsTable;
    QString         qsSQL;
    QLabel          *labelFilter;
    QLabel          *labelResults;

    void InitQuery();
    void ExecHeader( SQLHSTMT hStmt, SWORD nColumns );
    int  ExecBody( SQLHSTMT hStmt, SWORD nColumns );
    bool CreateSQL();
#ifdef QT_V4LAYOUT
    void ClearCells( Q3Table *table );
#else
    void ClearCells( QTable *table );
#endif
};

#endif



