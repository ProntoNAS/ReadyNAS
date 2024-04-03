/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CTHREADING_H
#define CTHREADING_H

#include "CODBCInst.h"

class CThreading : public QWidget
{
    Q_OBJECT
public:
	CThreading( QWidget* pwidgetParent = NULL );
	virtual ~CThreading();

    QString windowHelp();
    bool loadData();
    bool saveData();

public slots:
    bool slotApply();
    bool slotDefault();

protected:
    QSpinBox *pspinboxLevel;
};


#endif

