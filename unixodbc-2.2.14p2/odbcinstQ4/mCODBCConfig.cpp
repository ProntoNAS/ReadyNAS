/****************************************************************************
** Meta object code from reading C++ file 'CODBCConfig.h'
**
** Created: Tue Sep 2 11:04:00 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CODBCConfig.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CODBCConfig.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CODBCConfig[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      60,   13,   12,   12, 0x09,
     110,   12,   12,   12, 0x09,
     135,  121,   12,   12, 0x09,
     158,   12,   12,   12, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_CODBCConfig[] = {
    "CODBCConfig\0\0"
    "plistwidgetitemCurrent,plistwidgetitemPrevious\0"
    "slotChangePage(QListWidgetItem*,QListWidgetItem*)\0"
    "slotHelp()\0stringMessage\0"
    "slotHelpError(QString)\0slotAccept()\0"
};

const QMetaObject CODBCConfig::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CODBCConfig,
      qt_meta_data_CODBCConfig, 0 }
};

const QMetaObject *CODBCConfig::metaObject() const
{
    return &staticMetaObject;
}

void *CODBCConfig::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CODBCConfig))
	return static_cast<void*>(const_cast< CODBCConfig*>(this));
    return QDialog::qt_metacast(_clname);
}

int CODBCConfig::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotChangePage((*reinterpret_cast< QListWidgetItem*(*)>(_a[1])),(*reinterpret_cast< QListWidgetItem*(*)>(_a[2]))); break;
        case 1: slotHelp(); break;
        case 2: slotHelpError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: slotAccept(); break;
        }
        _id -= 4;
    }
    return _id;
}
