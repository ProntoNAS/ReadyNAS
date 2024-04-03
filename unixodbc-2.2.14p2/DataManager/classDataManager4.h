/**************************************************
 *
 *
 **************************************************
 * This code was created by Peter Harvey @ CodeByDesign.
 * Released under GPL 18.FEB.99
 *
 * Contributions from...
 * -----------------------------------------------
 * Peter Harvey		- pharvey@codebydesign.com
 **************************************************/
#ifndef classDataManager_included
#define classDataManager_included

#include <stdlib.h>

#include <Qt/qpixmap.h>
#include <Qt/qmessagebox.h>
#include <Qt/qwidget.h>
#include <Qt/qlayout.h>
#include <Qt/qsplitter.h>
#include <Qt/qdir.h>

#include <ini.h>

#include "classCanvas.h"
#include "classODBC.h"

class classDataManager : public QWidget
{
    Q_OBJECT

public:
    classDataManager( QWidget *pParent = 0, const char *pszName = 0 );
    ~classDataManager();

    void resizeEvent( QResizeEvent * );

public slots:
    void ItemMenu( Q3ListViewItem*,const QPoint&,int);
    void ItemChanged( Q3ListViewItem*);

protected slots:
    void Exit();
    void About();

protected:
	QSplitter	*splSplitter;
    LView	*lvwBrowser;
    classCanvas	*pCanvas;

	classODBC	*pODBC;

private:
    void LoadState();
    void SaveState();
};
#endif



