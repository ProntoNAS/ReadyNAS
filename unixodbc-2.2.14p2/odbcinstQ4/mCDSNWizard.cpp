/****************************************************************************
** Meta object code from reading C++ file 'CDSNWizard.h'
**
** Created: Tue Sep 2 11:01:40 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CDSNWizard.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CDSNWizard.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CDSNWizard[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_CDSNWizard[] = {
    "CDSNWizard\0\0slotHelp()\0"
};

const QMetaObject CDSNWizard::staticMetaObject = {
    { &QWizard::staticMetaObject, qt_meta_stringdata_CDSNWizard,
      qt_meta_data_CDSNWizard, 0 }
};

const QMetaObject *CDSNWizard::metaObject() const
{
    return &staticMetaObject;
}

void *CDSNWizard::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CDSNWizard))
	return static_cast<void*>(const_cast< CDSNWizard*>(this));
    return QWizard::qt_metacast(_clname);
}

int CDSNWizard::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizard::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotHelp(); break;
        }
        _id -= 1;
    }
    return _id;
}
