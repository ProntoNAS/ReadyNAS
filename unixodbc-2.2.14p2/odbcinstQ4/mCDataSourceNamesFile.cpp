/****************************************************************************
** Meta object code from reading C++ file 'CDataSourceNamesFile.h'
**
** Created: Tue Sep 2 10:58:10 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CDataSourceNamesFile.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CDataSourceNamesFile.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CDataSourceNamesFile[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x0a,
      33,   21,   21,   21, 0x09,
      43,   21,   21,   21, 0x09,
      54,   21,   21,   21, 0x09,
      67,   21,   21,   21, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_CDataSourceNamesFile[] = {
    "CDataSourceNamesFile\0\0slotLoad()\0"
    "slotAdd()\0slotEdit()\0slotDelete()\0"
    "slotSetDefault()\0"
};

const QMetaObject CDataSourceNamesFile::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_CDataSourceNamesFile,
      qt_meta_data_CDataSourceNamesFile, 0 }
};

const QMetaObject *CDataSourceNamesFile::metaObject() const
{
    return &staticMetaObject;
}

void *CDataSourceNamesFile::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CDataSourceNamesFile))
	return static_cast<void*>(const_cast< CDataSourceNamesFile*>(this));
    return QWidget::qt_metacast(_clname);
}

int CDataSourceNamesFile::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotLoad(); break;
        case 1: slotAdd(); break;
        case 2: slotEdit(); break;
        case 3: slotDelete(); break;
        case 4: slotSetDefault(); break;
        }
        _id -= 5;
    }
    return _id;
}
