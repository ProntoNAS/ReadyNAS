/****************************************************************************
** Meta object code from reading C++ file 'CFileSelector.h'
**
** Created: Tue Sep 2 11:02:27 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CFileSelector.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CFileSelector.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CFileSelector[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      31,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CFileSelector[] = {
    "CFileSelector\0\0signalChanged()\0"
    "slotInvokeDialog()\0"
};

const QMetaObject CFileSelector::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CFileSelector,
      qt_meta_data_CFileSelector, 0 }
};

const QMetaObject *CFileSelector::metaObject() const
{
    return &staticMetaObject;
}

void *CFileSelector::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CFileSelector))
	return static_cast<void*>(const_cast< CFileSelector*>(this));
    return QWidget::qt_metacast(_clname);
}

int CFileSelector::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: signalChanged(); break;
        case 1: slotInvokeDialog(); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void CFileSelector::signalChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
