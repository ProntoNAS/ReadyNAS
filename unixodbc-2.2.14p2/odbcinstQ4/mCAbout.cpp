/****************************************************************************
** Meta object code from reading C++ file 'CAbout.h'
**
** Created: Tue Sep 2 10:56:59 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CAbout.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CAbout.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CAbout[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets

       0        // eod
};

static const char qt_meta_stringdata_CAbout[] = {
    "CAbout\0"
};

const QMetaObject CAbout::staticMetaObject = {
    { &QTabWidget::staticMetaObject, qt_meta_stringdata_CAbout,
      qt_meta_data_CAbout, 0 }
};

const QMetaObject *CAbout::metaObject() const
{
    return &staticMetaObject;
}

void *CAbout::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CAbout))
	return static_cast<void*>(const_cast< CAbout*>(this));
    return QTabWidget::qt_metacast(_clname);
}

int CAbout::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
