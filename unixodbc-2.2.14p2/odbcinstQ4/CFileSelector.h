/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CFILESELECTOR_H
#define CFILESELECTOR_H

#include "CODBCInst.h"

#include <QLineEdit>

/*! 
 * \class   CFileSelector
 * \brief   File selector.
 * 
 *          Displays a file name to the User and allows it to be changed by way
 *          of clicking a button which invokes a file selection dialog.
 *
 *          This is used by CTracing to allow the User to select a log file.
 *
 * \sa      CTracing
 */
class CFileSelector : public QWidget
{
    Q_OBJECT
public:
    enum CFileSelectorMode
    {
        TraceFile,
        TraceLibrary,
        FileDSNDirectory,
        Driver,
        Setup
    };

    CFileSelector( CFileSelectorMode nMode, const QString &stringLabel = QString::null, bool bCompact = true, bool bButton = true, QWidget* pwidgetParent = NULL );
    virtual ~CFileSelector();

    void setText( const QString &stringText );
    QString getText();

signals:
    void signalChanged();

public slots:
	void slotInvokeDialog();

protected:
    CFileSelectorMode   nMode;
    QLineEdit *         pLineEdit;
};

#endif

