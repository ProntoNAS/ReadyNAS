/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CDATASOURCENAMESFILEMODEL_H
#define CDATASOURCENAMESFILEMODEL_H

#include "CODBCInst.h"

#include <QDirModel>

/*! 
 * \class   CDataSourceNamesFileModel
 *
 * \brief   Presents file-based Data Source Names.
 *
 *          This model can be used to display file-based Data Source Names in a given
 *          directory.
 *
 *          This model extends QDirModel by;
 *
 *          \li using default filter (*.dsn)
 *          \li other, more appropriate, defaults (QDir::Files, QDir::Name)
 *          \li methods to; add, edit, and delete a file-based Data Source Name
 *
 *          The usefullness of this being a model instead of being a widget (QListView
 *          derived perhaps) or some other implementation is debatable. But this is a
 *          nice experiment with using a model in this special way - if nothing else.
 *
 * \code
{
    pDataSourceNamesFileModel   = new CDataSourceNamesFileModel;
    pListView                   = new QListView;

    pListView->setViewMode( QListView::IconMode );
    pListView->setModel( pDataSourceNamesFileModel );
    pListView->setRootIndex( pDataSourceNamesFileModel->index( stringDirectory ) );

    playout->addWidget( pListView, 1, 0 );
}
 * \endcode
 *
 * \sa
 */
class CDataSourceNamesFileModel : public QDirModel
{
    Q_OBJECT
public:
    CDataSourceNamesFileModel( QObject* pobjectParent = NULL );
    virtual ~CDataSourceNamesFileModel();

public slots:
    bool addDataSourceName( const QString &stringDirectory );
    bool editDataSourceName( const QModelIndex &modelindex );
    bool deleteDataSourceName( const QModelIndex &modelindex );
};

#endif

