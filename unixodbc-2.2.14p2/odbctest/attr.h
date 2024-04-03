/*********************************************************************
 *
 * Written by Nick Gorham
 * (nick@lurcher.org).
 *
 * copyright (c) 1999 Nick Gorham
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 **********************************************************************/

#ifndef ATTR_H
#define ATTR_H

#ifdef QT_V4LAYOUT
#include <Qt/qwidget.h>
#include <Qt/qdialog.h>
#include <Qt/qpushbutton.h>
#include <Qt/qcombobox.h>
#include <Qt/qcheckbox.h>
#include <Qt/qlabel.h>
#else
#include <qwidget.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#endif
#include "odbctest.h"

class dSetStmtAttr : public QDialog 
{
    Q_OBJECT

    public:
        dSetStmtAttr( OdbcTest *parent, QString name );
        ~dSetStmtAttr();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *types, *value, *stringlen;
		QLabel *l_handle, *l_types, *l_value, *l_slen;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void Activated( int index );
		void Ok();
};

class dSetStmtOption : public QDialog 
{
    Q_OBJECT

    public:
        dSetStmtOption( OdbcTest *parent, QString name );
        ~dSetStmtOption();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *types, *value;
		QLabel *l_handle, *l_types, *l_value;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void Activated( int index );
		void Ok();
};

class dGetStmtOption : public QDialog 
{
    Q_OBJECT

    public:
        dGetStmtOption( OdbcTest *parent, QString name );
        ~dGetStmtOption();

    protected:
        QPushButton *ok, *cancel, *help;
        QComboBox *handles, *types;
        QCheckBox *target_valid;
        QLabel *l_handle, *l_types;
#ifdef QT_V4LAYOUT
        Q3MultiLineEdit *in_win;
#else
        QMultiLineEdit *in_win;
#endif
        OutputWin *out_win;
        OdbcTest *odbctest;
        QString txt;

    protected slots:
        void target_clkd();
        void Ok();
};

class dGetStmtAttr : public QDialog 
{
    Q_OBJECT

    public:
        dGetStmtAttr( OdbcTest *parent, QString name );
        ~dGetStmtAttr();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *types;
		QCheckBox *target_valid, *strlen_valid;
		QLabel *l_handle, *l_types, *l_buffer_len;
		QLineEdit *buffer_len;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void target_clkd();
		void strlen_clkd();
		void Ok();
};

class dSetConnAttr : public QDialog 
{
    Q_OBJECT

    public:
        dSetConnAttr( OdbcTest *parent, QString name );
        ~dSetConnAttr();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *types, *value, *stringlen;
		QLabel *l_handle, *l_types, *l_value, *l_slen;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void Activated( int index );
		void Ok();
};

class dGetConnAttr : public QDialog 
{
    Q_OBJECT

    public:
        dGetConnAttr( OdbcTest *parent, QString name );
        ~dGetConnAttr();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *types;
		QCheckBox *target_valid, *strlen_valid;
		QLabel *l_handle, *l_types, *l_buffer_len;
		QLineEdit *buffer_len;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void target_clkd();
		void strlen_clkd();
		void Ok();
};

class dSetConnectOption : public QDialog 
{
    Q_OBJECT

    public:
        dSetConnectOption( OdbcTest *parent, QString name );
        ~dSetConnectOption();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *types, *value;
		QLabel *l_handle, *l_types, *l_value;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void Activated( int index );
		void Ok();
};

class dGetConnectOption : public QDialog 
{
    Q_OBJECT

    public:
        dGetConnectOption( OdbcTest *parent, QString name );
        ~dGetConnectOption();

    protected:
        QPushButton *ok, *cancel, *help;
        QComboBox *handles, *types;
        QCheckBox *target_valid;
        QLabel *l_handle, *l_types;
#ifdef QT_V4LAYOUT
        Q3MultiLineEdit *in_win;
#else
        QMultiLineEdit *in_win;
#endif
        OutputWin *out_win;
        OdbcTest *odbctest;
        QString txt;

    protected slots:
        void target_clkd();
        void Ok();
};

class dSetEnvAttr : public QDialog 
{
    Q_OBJECT

    public:
        dSetEnvAttr( OdbcTest *parent, QString name );
        ~dSetEnvAttr();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *types, *value, *stringlen;
		QLabel *l_handle, *l_types, *l_value, *l_slen;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void Activated( int index );
		void Ok();
};

class dGetEnvAttr : public QDialog 
{
    Q_OBJECT

    public:
        dGetEnvAttr( OdbcTest *parent, QString name );
        ~dGetEnvAttr();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *types;
		QCheckBox *target_valid, *strlen_valid;
		QLabel *l_handle, *l_types, *l_buffer_len;
		QLineEdit *buffer_len;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void target_clkd();
		void strlen_clkd();
		void Ok();
};

#endif
