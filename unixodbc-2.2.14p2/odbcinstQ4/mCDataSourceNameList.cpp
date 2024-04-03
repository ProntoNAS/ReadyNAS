/****************************************************************************
** Meta object code from reading C++ file 'CDataSourceNameList.h'
**
** Created: Tue Sep 2 10:57:47 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CDataSourceNameList.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CDataSourceNameList.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CDataSourceNameList[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x0a,
      31,   20,   20,   20, 0x0a,
      42,   20,   20,   20, 0x0a,
      55,   20,   20,   20, 0x0a,
      72,   66,   20,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CDataSourceNameList[] = {
    "CDataSourceNameList\0\0slotAdd()\0"
    "slotEdit()\0slotDelete()\0slotLoad()\0"
    "pItem\0slotDoubleClick(QTableWidgetItem*)\0"
};

const QMetaObject CDataSourceNameList::staticMetaObject = {
    { &QTableWidget::staticMetaObject, qt_meta_stringdata_CDataSourceNameList,
      qt_meta_data_CDataSourceNameList, 0 }
};

const QMetaObject *CDataSourceNameList::metaObject() const
{
    return &staticMetaObject;
}

void *CDataSourceNameList::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CDataSourceNameList))
	return static_cast<void*>(const_cast< CDataSourceNameList*>(this));
    return QTableWidget::qt_metacast(_clname);
}

int CDataSourceNameList::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTableWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotAdd(); break;
        case 1: slotEdit(); break;
        case 2: slotDelete(); break;
        case 3: slotLoad(); break;
        case 4: slotDoubleClick((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        }
        _id -= 5;
    }
    return _id;
}
