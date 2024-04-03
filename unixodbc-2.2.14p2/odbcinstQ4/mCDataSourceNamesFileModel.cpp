/****************************************************************************
** Meta object code from reading C++ file 'CDataSourceNamesFileModel.h'
**
** Created: Tue Sep 2 10:58:33 2008
**      by: The Qt Meta Object Compiler version 59 (Qt 4.3.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CDataSourceNamesFileModel.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CDataSourceNamesFileModel.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.3.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

static const uint qt_meta_data_CDataSourceNamesFileModel[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      48,   32,   27,   26, 0x0a,
      86,   75,   27,   26, 0x0a,
     118,   75,   27,   26, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CDataSourceNamesFileModel[] = {
    "CDataSourceNamesFileModel\0\0bool\0"
    "stringDirectory\0addDataSourceName(QString)\0"
    "modelindex\0editDataSourceName(QModelIndex)\0"
    "deleteDataSourceName(QModelIndex)\0"
};

const QMetaObject CDataSourceNamesFileModel::staticMetaObject = {
    { &QDirModel::staticMetaObject, qt_meta_stringdata_CDataSourceNamesFileModel,
      qt_meta_data_CDataSourceNamesFileModel, 0 }
};

const QMetaObject *CDataSourceNamesFileModel::metaObject() const
{
    return &staticMetaObject;
}

void *CDataSourceNamesFileModel::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CDataSourceNamesFileModel))
	return static_cast<void*>(const_cast< CDataSourceNamesFileModel*>(this));
    return QDirModel::qt_metacast(_clname);
}

int CDataSourceNamesFileModel::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDirModel::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { bool _r = addDataSourceName((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 1: { bool _r = editDataSourceName((*reinterpret_cast< const QModelIndex(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        case 2: { bool _r = deleteDataSourceName((*reinterpret_cast< const QModelIndex(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< bool*>(_a[0]) = _r; }  break;
        }
        _id -= 3;
    }
    return _id;
}
