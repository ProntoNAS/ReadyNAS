/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#include <QtGui>

#include "CPage.h"

#include "CHelp.h"

CPage::CPage( QWidget* pwidgetParent, const QString &stringTitle, QWidget *pwidgetContent, const QIcon &icon, const QString &stringHelp )
    : QWidget( pwidgetParent )
{
    Q_ASSERT(pwidgetContent);

    QVBoxLayout *   playout = new QVBoxLayout;

    this->pwidgetContent = pwidgetContent;

    if ( !stringTitle.isEmpty() )
    {
        QLabel *        plabel  = new QLabel( stringTitle );
        QFrame *        pframe  = new QFrame;
        QFont           font( plabel->font() );

        font.setPointSize( font.pointSize() + 2 ) ;
        font.setBold( true );

        plabel->setFont( font );
        pframe->setFrameStyle( QFrame::HLine );

        playout->addWidget( plabel, 1 );
        playout->addWidget( pframe );
    }

    playout->addWidget( pwidgetContent, 10 );

    if ( !stringHelp.isEmpty() )
    {
        CHelp *pHelp = new CHelp( pwidgetContent->metaObject()->className(), icon, stringHelp );
        playout->addWidget( pHelp );
    }

    setLayout( playout );
}

CPage::~CPage()
{
}


