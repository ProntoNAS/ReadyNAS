/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CODBCINST_H
#define CODBCINST_H

#include <odbcinstext.h>

#include <QtGui>

class CDSNWizardData
{
public:
    enum Type
    {
        TypeUser,
        TypeSystem,
        TypeFile
    };

    CDSNWizardData( const QString &stringDataSourceName = QString::null ) ;
    ~CDSNWizardData();

    Type                nType;
    QString             stringDriver;
    QString             stringDataSourceName;
    HODBCINSTPROPERTY   hFirstProperty;
};

/*! 
 * \brief   A namespace.
 * 
 */
class CODBCInst
{
public:
    static bool saveDataSourceName( QWidget *pwidgetParent, HODBCINSTPROPERTY hFirstProperty, CDSNWizardData::Type nType, const QString &stringIni = QString::null );
    static int showErrors( QWidget *pwidgetParent = 0, const QString &stringConsequence = QString::null );
};

#endif

