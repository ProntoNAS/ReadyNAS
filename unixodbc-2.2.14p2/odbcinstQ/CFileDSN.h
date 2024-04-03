/**************************************************
 * 
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign. 
 * Released under GPL 31.JAN.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 * Nick Gorham      - nick@easysoft.com
 **************************************************/
#ifndef CFileDSN_included
#define CFileDSN_included

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qwidget.h>
#include <Qt/qpushbutton.h>
#include <Qt/qpixmap.h>
#include <Qt/qlayout.h>
#else
#include <qwidget.h>
#include <qpushbt.h>
#include <qpixmap.h>
#include <qlayout.h>
#endif

#include "CFileList.h"

class CFileDSN : public QWidget
{
    Q_OBJECT

public:

    CFileDSN( QWidget* parent = NULL, const char* name = NULL, QString *cwd = NULL );
    ~CFileDSN();

public slots:
	void NewDir();

protected:
    QPushButton* pbAdd;
    QPushButton* pbRemove;
    QPushButton* pbConfigure;
    QPushButton* pbDir;
    CFileList* pFileList;
    QString path;
    QLabel* dirlab;
};
#endif
