/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CDATASOURCENAMESFILE_H
#define CDATASOURCENAMESFILE_H

#include "CODBCInst.h"

#define ODBC_HELP_DATASOURCES_FILE "File Data Source Names (DSN's) are files which hold connection options. These are more versatile than regular DSN's because the files can be shared over a network and/or easily distributed throughout a network."

class CDataSourceNamesFileModel;
class CFileSelector;
class QListView;
class QLabel;

/*! 
 * \class   CDataSourceNamesFile
 *
 * \brief   Widget to allow User to manage FileDSN's including setting default dir.
 *
 *          Presents a list of FileDSN's in the given Path. The User may;
 *          change the Path and add/remove/edit a selected FileDSN.
 *
 * \note    Elevated privileges (root) may be required to add/remove/edit a FileDSN. This
 *          depends largely on where the FileDSN is located.
 *
 * \sa      CManageDataSources
 *          CManageDrivers
 *          CStats
 *          CTracing
 *          CAbout
 */
class CDataSourceNamesFile : public QWidget
{
    Q_OBJECT
public:
    CDataSourceNamesFile( QWidget* pwidgetParent = NULL );
    virtual ~CDataSourceNamesFile();

    QString windowHelp() { return QString( tr( ODBC_HELP_DATASOURCES_FILE ) ); }

public slots:
    void slotLoad();
   
protected slots:
    void slotAdd();
    void slotEdit();
    void slotDelete();
    void slotSetDefault();

protected:
    QLabel *                    plabelDefault;
    CFileSelector *             pFileSelector;
    CDataSourceNamesFileModel * pDataSourceNamesFileModel;
    QListView *                 pListView;

    QString getDefault();
};

#endif

