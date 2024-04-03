/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CDATASOURCENAMELIST_H
#define CDATASOURCENAMELIST_H

#include "CODBCInst.h"

#include <QTableWidget>

/*! 
 * \brief   A simple table widget listing Data Source Names (DSN's).
 * 
 *          You can use this widget to present a list of DSN's so that one can
 *          simply be selected. Simply instantiating the widget is enough to
 *          display the DSN's. Then one can get the selected DSN by calling the 
 *          getter functions;
 *
 *          \li getDataSourceName
 *          \li getDescription
 *
 *          This widget can also be used as the core widget for managing the DSN's
 *          by connecting to the following slots;
 *
 *          \li slotAdd
 *          \li slotEdit
 *          \li slotDelete
 *
 * \note    ODBC_USER_DSN's can be edited with normal privileges but ODBC_SYSTEM_DSN's, 
 *          typically, will require elevated system privileges such as 'root' access.
 *
 * \sa      CDriverList
 *          CDataSources
 */
class CDataSourceNameList : public QTableWidget
{
    Q_OBJECT
public:
    CDataSourceNameList( QWidget* pwidgetParent = NULL, int nSource = ODBC_BOTH_DSN );
    virtual ~CDataSourceNameList();

    QString getDataSourceName();
    QString getDescription();

public slots:
    void slotAdd();
    void slotEdit();
    void slotDelete();
    void slotLoad();
    void slotDoubleClick( QTableWidgetItem *pItem );

private:
    int nSource;
};

#endif

