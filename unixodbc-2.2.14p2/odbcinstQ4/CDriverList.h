/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CDRIVERLIST_H
#define CDRIVERLIST_H

#include "CODBCInst.h"

#include <QTableWidget>

/*! 
 * \brief   A simple table widget listing the installed drivers.
 * 
 *          You can use this widget to present a list of installed drivers to the User
 *          so that one can simply be selected. Simply instantiating the widget is
 *          enough to display the installed drivers. Then one can get the selected
 *          driver by calling the getter functions;
 *
 *          \li getFriendlyName
 *          \li getDriver
 *          \li getSetup
 *          \li getDescription
 *
 *          This widget can also be used as the core widget for managing drivers by 
 *          connecting to the following slots;
 *
 *          \li slotAdd
 *          \li slotEdit
 *          \li slotDelete
 *
 * \note    Typically; anyone can view installed drivers but managing drivers will require
 *          elevated system privileges such as 'root' access.
 *
 * \sa      CDataSourceNameList
 */
class CDriverList : public QTableWidget
{
    Q_OBJECT
public:
    CDriverList( QWidget* pwidgetParent = NULL );
    virtual ~CDriverList();

    QString getFriendlyName();
    QString getDescription();
    QString getDriver();
    QString getSetup();

signals:
    void signalChanged();

public slots:
	void slotAdd();
	void slotEdit();
	void slotDelete();
    void slotLoad();
    void slotDoubleClick( QTableWidgetItem *pItem );
};

#endif

