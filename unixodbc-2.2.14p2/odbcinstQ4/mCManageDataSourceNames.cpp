/****************************************************************************
** Meta object code from reading C++ file 'CManageDataSourceNames.h'
**
** Created: Tue Sep 2 11:03:13 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CManageDataSourceNames.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CManageDataSourceNames.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CManageDataSourceNames[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CManageDataSourceNames[] = {
    "CManageDataSourceNames\0\0slotLoad()\0"
};

const QMetaObject CManageDataSourceNames::staticMetaObject = {
    { &QTabWidget::staticMetaObject, qt_meta_stringdata_CManageDataSourceNames,
      qt_meta_data_CManageDataSourceNames, 0 }
};

const QMetaObject *CManageDataSourceNames::metaObject() const
{
    return &staticMetaObject;
}

void *CManageDataSourceNames::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CManageDataSourceNames))
	return static_cast<void*>(const_cast< CManageDataSourceNames*>(this));
    return QTabWidget::qt_metacast(_clname);
}

int CManageDataSourceNames::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTabWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotLoad(); break;
        }
        _id -= 1;
    }
    return _id;
}
