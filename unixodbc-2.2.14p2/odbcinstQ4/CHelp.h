/*!
 * \file
 *
 * \author  Peter Harvey <pharvey@peterharvey.org>
 * \author  \sa AUTHORS file
 * \version 2
 * \date    2007
 * \license Copyright unixODBC Project 2007-2008, LGPL
 */
#ifndef CHELP_H
#define CHELP_H

#include "CODBCInst.h"

#include <QWidget>
#include <QPixmap>

class QFrame;

/*! 
 * \class   CHelp
 * \brief   Help widget for tabs/property pages.
 * 
 *          This widget presents the given Icon and Text in a manner useful for the 
 *          bottom (or other border area) of another widget where Helpful information
 *          (read; hints about usage/purpose) may be needed.
 *
 *          This widget is used by the CODBCConfig tab/property widgets.
 *
 */
class CHelp : public QWidget
{
    Q_OBJECT
public:
    CHelp( const QString &stringKey, const QIcon &icon, const QString &stringText, QWidget *pwidgetParent = 0 );
    ~CHelp();

protected:
    bool            bVisibleView;
    QString         stringKey;
    QToolButton *   pToolButton;
    QFrame *        pFrame;
    QLabel *        plabelIcon;
    QLabel *        plabelText;

    void loadState();
    void saveState();

protected slots:
    void slotToggleView();
};

#endif

