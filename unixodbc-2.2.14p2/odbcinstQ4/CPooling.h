/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CPOOLING_H
#define CPOOLING_H

#include "CODBCInst.h"

#define ODBC_HELP_POOLING "Connection pooling is most useful for ODBC enabled server processes. It can increase performance but its usefulness is dependent upon the stability of the driver. Additional Pooling options are set for individual drivers."

class CPooling : public QWidget
{
    Q_OBJECT
public:
	CPooling( QWidget* pwidgetParent = NULL );
	virtual ~CPooling();

    QString windowHelp() { return QString( tr( ODBC_HELP_POOLING ) ); }
    bool loadData();
    bool saveData();

public slots:
    bool slotApply();
    bool slotDefault();

protected:
    QCheckBox *     pcheckboxEnable;
//    QSpinBox *      pspinboxTimeout;    /*<! CPTimeout      */
//    QSpinBox *      pspinboxTimeToLive; /*<! CPTimeToLive   */
};


#endif

