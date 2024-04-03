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
#ifdef QT_V4LAYOUT
#include <Qt/qapplication.h>
#include <Qt/qdir.h>
#else
#include <qapp.h>
#include <qdir.h>
#endif

#include <stdlib.h>

#ifdef QT_V4LAYOUT
#include "classDataManager4.h"
#else
#include "classDataManager.h"
#endif

int main( int argc, char **argv )
{
    QApplication        oApplication( argc, argv );		    // create application object
    classDataManager	oMain;

    QDir::home().mkdir( ".DataManager" );

	oApplication.connect( &oApplication, SIGNAL(lastWindowClosed()), &oApplication, SLOT(quit()) );
    oMain.show();	          					            // show widget

    return oApplication.exec();	   		                    // run event loop
}



