/****************************************************************************
** Meta object code from reading C++ file 'CDriverList.h'
**
** Created: Tue Sep 2 10:59:19 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CDriverList.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CDriverList.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CDriverList[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      29,   12,   12,   12, 0x0a,
      39,   12,   12,   12, 0x0a,
      50,   12,   12,   12, 0x0a,
      63,   12,   12,   12, 0x0a,
      80,   74,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CDriverList[] = {
    "CDriverList\0\0signalChanged()\0slotAdd()\0"
    "slotEdit()\0slotDelete()\0slotLoad()\0"
    "pItem\0slotDoubleClick(QTableWidgetItem*)\0"
};

const QMetaObject CDriverList::staticMetaObject = {
    { &QTableWidget::staticMetaObject, qt_meta_stringdata_CDriverList,
      qt_meta_data_CDriverList, 0 }
};

const QMetaObject *CDriverList::metaObject() const
{
    return &staticMetaObject;
}

void *CDriverList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CDriverList))
	return static_cast<void*>(const_cast< CDriverList*>(this));
    return QTableWidget::qt_metacast(_clname);
}

int CDriverList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: signalChanged(); break;
        case 1: slotAdd(); break;
        case 2: slotEdit(); break;
        case 3: slotDelete(); break;
        case 4: slotLoad(); break;
        case 5: slotDoubleClick((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void CDriverList::signalChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
