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

#ifndef CONN_H
#define CONN_H

#ifdef QT_V4LAYOUT
#include <Qt/qwidget.h>
#include <Qt/qdialog.h>
#include <Qt/qpushbutton.h>
#include <Qt/qcombobox.h>
#include <Qt/qcheckbox.h>
#include <Qt/qlabel.h>
#include <Qt/q3listbox.h>
#include <Qt/q3buttongroup.h>
#include <Qt/qradiobutton.h>
#include <Qt/qlineedit.h>
#include <Qt/q3multilineedit.h>
#else
#include <qwidget.h>
#include <qdialog.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qmultilineedit.h>
#endif
#include "odbctest.h"

class dConnect : public QDialog 
{
    Q_OBJECT

    public:
        dConnect( OdbcTest *parent, QString name );
        ~dConnect();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *server, *user, *auth;
		QComboBox *server_len, *user_len, *auth_len;
		QLabel *l_handle, *l_server, *l_user, *l_auth;
		QLabel *l_server_len, *l_user_len, *l_auth_len;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void Ok();
};

class dDisconnect : public QDialog 
{
    Q_OBJECT

    public:
        dDisconnect( OdbcTest *parent, QString name );
        ~dDisconnect();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles;
		QLabel *l_handle;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void Ok();
};

class dFullConnect : public QDialog 
{
    Q_OBJECT

    public:
        dFullConnect( OdbcTest *parent, QString name );
        ~dFullConnect();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles;
		QLineEdit *dsn, *usr, *pwd;
		QLabel *l_handle, *l_dsn, *l_usr, *l_pwd;
#ifdef QT_V4LAYOUT
		Q3ListBox *dsn_list;
		Q3MultiLineEdit *in_win;
		Q3ButtonGroup *version, *cursor;
#else
		QListBox *dsn_list;
		QMultiLineEdit *in_win;
		QButtonGroup *version, *cursor;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QRadioButton *ver_2, *ver_3, *ver_def; 
		QRadioButton *cur_ifneeded, *cur_use, *cur_driver, *cur_default;
		QString txt;

	protected slots:
		void Ok();
        void set_dsn( const QString &);
};

class dGetInfo : public QDialog 
{
    Q_OBJECT

    public:
        dGetInfo( OdbcTest *parent, QString name );
        ~dGetInfo();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *type;
		QLineEdit *buffer_len;
		QCheckBox *target_valid, *strlen_valid;
		QLabel *l_handle, *l_type, *l_buffer_len;
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

class dGetFunctions : public QDialog 
{
    Q_OBJECT

    public:
        dGetFunctions( OdbcTest *parent, QString name );
        ~dGetFunctions();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *type;
		QCheckBox *target_valid;
		QLabel *l_handle, *l_type;
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

class dBrowseConnect : public QDialog 
{
    Q_OBJECT

    public:
        dBrowseConnect( OdbcTest *parent, QString name );
        ~dBrowseConnect();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *in_str, *str_len;
		QLineEdit *buffer_len;
		QCheckBox *out_str_valid, *strlen_valid;
		QLabel *l_handle, *l_out_str, *l_buffer_len;
		QLabel *l_str_len, *l_in_str;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void out_str_clkd();
		void strlen_clkd();
		void Ok();
};

class dDriverConnect : public QDialog 
{
    Q_OBJECT

    public:
        dDriverConnect( OdbcTest *parent, QString name );
        ~dDriverConnect();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *in_str, *str_len, *driver_completion;
		QLineEdit *buffer_len;
		QCheckBox *out_str_valid, *strlen_valid;
		QLabel *l_handle, *l_out_str, *l_buffer_len;
		QLabel *l_str_len, *l_in_str, *l_driver_completion;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void out_str_clkd();
		void strlen_clkd();
		void Ok();
};

class dNativeSQL : public QDialog 
{
    Q_OBJECT

    public:
        dNativeSQL( OdbcTest *parent, QString name );
        ~dNativeSQL();

	protected:
		QPushButton *ok, *cancel, *help;
		QComboBox *handles, *in_str, *str_len;
		QLineEdit *buffer_len;
		QCheckBox *out_str_valid, *strlen_valid;
		QLabel *l_handle, *l_out_str, *l_buffer_len;
		QLabel *l_str_len, *l_in_str;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void out_str_clkd();
		void strlen_clkd();
		void Ok();
};

class dAllocConnect : public QDialog 
{
    Q_OBJECT

    public:
        dAllocConnect( OdbcTest *parent, QString name );
        ~dAllocConnect();

	protected:
		QPushButton *ok, *cancel, *help;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		QCheckBox *handle_valid;
        QComboBox *handles;
        QLabel *l_handles;
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void Ok();
        void handle_clkd();
};

class dFreeConnect : public QDialog 
{
    Q_OBJECT

    public:
        dFreeConnect( OdbcTest *parent, QString name );
        ~dFreeConnect();

	protected:
		QPushButton *ok, *cancel, *help;
#ifdef QT_V4LAYOUT
		Q3MultiLineEdit *in_win;
#else
		QMultiLineEdit *in_win;
#endif
		QComboBox *handles;
		QLabel *l_handles;
		OutputWin *out_win;
		OdbcTest *odbctest;
		QString txt;

	protected slots:
		void Ok();
};

#endif
