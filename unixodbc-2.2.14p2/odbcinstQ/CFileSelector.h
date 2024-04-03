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
 **************************************************/
#ifndef CFileSelector_included
#define CFileSelector_included

#ifdef QT_V4LAYOUT
#define QT3_SUPPORT
#include <Qt/qlineedit.h>
#include <Qt/qpushbutton.h>
#include <Qt/qlayout.h>
#include <Qt/qmessagebox.h>
#include <Qt/qfiledialog.h>
#else
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#endif

class CFileSelector : public QWidget
{
    Q_OBJECT

public:

    CFileSelector( QWidget* parent = NULL, const char* name = NULL );
    ~CFileSelector();

	QLineEdit	*pLineEdit;
	QPushButton	*pButton;

protected slots:
	void pButton_Clicked();

protected:

};
#endif 
