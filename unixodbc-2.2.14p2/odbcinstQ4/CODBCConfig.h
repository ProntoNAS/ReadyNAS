/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CODBCCONFIG_H
#define CODBCCONFIG_H

#include "CODBCInst.h"

#include <QAssistantClient>
#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;

class CManageDataSourceNames;
class CMonitor;
class CAdvanced;
class CAbout;

/*! 
 * \class   CODBCConfig
 *
 * \brief   ODBC Administrator dialog.
 *
 *          This dialog allows the User to manage;
 *
 *          \li User Data Source Names
 *          \li System Data Source Names
 *          \li File Data Source Names
 *          \li Drivers
 *          \li Tracing
 *          \li Connection Pooling
 *
 *          The User may also view;
 *
 *          \li Usage statistics
 *          \li Version information and other 'About' information
 * 
 *          This is the main dialog/window for the ODBCConfig application and is also used
 *          to support a call to SQLManageDataSources.
 *
 * \sa      ODBCManageDataSources
 *          ODBCCreateDataSource
 */
class CODBCConfig : public QDialog
{
    Q_OBJECT
public:
    CODBCConfig( QWidget *pwidgetParent = 0, Qt::WindowFlags nFlags = 0 );
    virtual ~CODBCConfig();

    bool saveData();

protected slots:
    void slotChangePage( QListWidgetItem *plistwidgetitemCurrent, QListWidgetItem *plistwidgetitemPrevious );
    void slotHelp();
    void slotHelpError( const QString &stringMessage );
    void slotAccept();

protected:
     void createIconMenu();
     void createConfigWidgets();

     void loadState();
     void saveState();

     QListWidget *      plistwidgetIcons;
     QStackedWidget *   pstackedwidgetConfigWidgets;

     CManageDataSourceNames *   pManageDataSourceNames;
     CMonitor *                 pMonitor;
     CAdvanced *                pAdvanced;
     CAbout *                   pAbout;
     QAssistantClient *         pAssistantClient;
};

#endif

