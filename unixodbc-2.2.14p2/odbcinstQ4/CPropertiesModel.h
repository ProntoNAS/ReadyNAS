/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CPROPERTIESMODEL_H
#define CPROPERTIESMODEL_H

#include "CODBCInst.h"

#include <QAbstractTableModel>
#include <QVector>

/* declare a non-standard data type so we can have it pass to/from as QVariant */
Q_DECLARE_METATYPE(HODBCINSTPROPERTY)

class CPropertiesModel : public QAbstractTableModel
{
public:
    CPropertiesModel( QObject *pobjectParent = 0, HODBCINSTPROPERTY hFirstProperty = NULL );
    virtual ~CPropertiesModel();

    int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    bool setData( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );
    Qt::ItemFlags flags( const QModelIndex &index ) const;
    QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

private:
    /* index our HODBCINSTPROPERTY list */
    QVector<HODBCINSTPROPERTY>  vectorProperties;
};

#endif

