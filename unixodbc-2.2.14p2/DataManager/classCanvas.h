/**************************************************
 * classCanvas
 **************************************************
 *
 * PURPOSE
 *
 * This is *THE* parent canvas for *ALL* property widgets. I
 * use a QLabel instead of a QVBoxLayout because I want to 
 * stack many custom widgets on this (and hide/show them) and
 * I am concerned that QVBoxLayout will have a problem with this.
 * I did not even try QVBoxLayout because, in anycase, we need
 * a real widget (not a list item) to allow node processing 
 * to create popup dialogs, such as the login (they need a 
 * widget based parent).
 *
 ***************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 18.FEB.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 **************************************************/
#ifndef classCanvas_included
#define classCanvas_included

#ifdef QT_V4LAYOUT
#include <Qt/qevent.h>
#include <Qt/qpixmap.h>
#include <Qt/qmessagebox.h>
#include <Qt/qwidget.h>
#include <Qt/qlistview.h>
#include <Qt/qframe.h>
#include <Qt/qlayout.h>
#include <Qt/qlabel.h>
#else
#include <qkeycode.h>
#include <qpixmap.h>
#include <qmessagebox.h>
#include <qwidget.h>
#include <qlistview.h>
#include <qframe.h>
#include <qlayout.h>
#include <qlabel.h>
#endif

class classCanvas : public QLabel
{
    Q_OBJECT

public:
    classCanvas( QWidget *pParent = 0, const char *pszName = 0 );
    ~classCanvas();

	void resizeEvent( QResizeEvent * );


signals:
	void changedSize( int nW, int nH );

};
#endif

