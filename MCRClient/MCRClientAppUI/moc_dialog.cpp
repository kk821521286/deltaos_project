/****************************************************************************
** Meta object code from reading C++ file 'dialog.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.5)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "dialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.5. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Dialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      27,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      28,    8,    7,    7, 0x08,
      69,    7,    7,    7, 0x08,
      85,    7,    7,    7, 0x08,
     121,    7,    7,    7, 0x08,
     162,  156,    7,    7, 0x08,
     202,    7,    7,    7, 0x08,
     240,    7,    7,    7, 0x08,
     277,    7,    7,    7, 0x08,
     312,    7,    7,    7, 0x08,
     343,    7,    7,    7, 0x08,
     376,  156,    7,    7, 0x08,
     415,    7,    7,    7, 0x08,
     451,    7,    7,    7, 0x08,
     486,    7,    7,    7, 0x08,
     536,  527,    7,    7, 0x08,
     585,  580,    7,    7, 0x08,
     622,  580,    7,    7, 0x08,
     658,    7,    7,    7, 0x08,
     694,    7,    7,    7, 0x08,
     725,    7,    7,    7, 0x08,
     753,    7,    7,    7, 0x08,
     791,    7,    7,    7, 0x08,
     807,    7,    7,    7, 0x08,
     818,    7,    7,    7, 0x08,
     839,    7,    7,    7, 0x08,
     870,    7,    7,    7, 0x08,
     906,    7,    7,    7, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Dialog[] = {
    "Dialog\0\0exitCode,exitStatus\0"
    "finish_process(int,QProcess::ExitStatus)\0"
    "start_process()\0on_pushButton_RecordStart_clicked()\0"
    "on_pushButton_RecordStop_clicked()\0"
    "index\0on_treeView_Module_clicked(QModelIndex)\0"
    "on_pushButton_DownloadStart_clicked()\0"
    "on_pushButton_DownloadStop_clicked()\0"
    "on_pushButton_DeleteFile_clicked()\0"
    "on_pushButton_PageUp_clicked()\0"
    "on_pushButton_PageDown_clicked()\0"
    "on_tableView_File_clicked(QModelIndex)\0"
    "on_pushButton_PlayerStart_clicked()\0"
    "on_pushButton_PlayerStop_clicked()\0"
    "on_pushButton_PlayerFullscreen_clicked()\0"
    "position\0on_horizontalSlider_Player_sliderMoved(int)\0"
    "date\0on_dateEdit_Start_dateChanged(QDate)\0"
    "on_dateEdit_Stop_dateChanged(QDate)\0"
    "on_pushButton_DownloadDir_clicked()\0"
    "on_pushButton_Delete_clicked()\0"
    "on_pushButton_Add_clicked()\0"
    "on_pushButton_DeleteAllFile_clicked()\0"
    "slotShowSmall()\0slotHelp()\0"
    "slotShowMaxRestore()\0"
    "on_pushButton_update_clicked()\0"
    "on_pushButton_PlayerPause_clicked()\0"
    "on_pBt_shift_clicked()\0"
};

void Dialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        Dialog *_t = static_cast<Dialog *>(_o);
        switch (_id) {
        case 0: _t->finish_process((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< QProcess::ExitStatus(*)>(_a[2]))); break;
        case 1: _t->start_process(); break;
        case 2: _t->on_pushButton_RecordStart_clicked(); break;
        case 3: _t->on_pushButton_RecordStop_clicked(); break;
        case 4: _t->on_treeView_Module_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 5: _t->on_pushButton_DownloadStart_clicked(); break;
        case 6: _t->on_pushButton_DownloadStop_clicked(); break;
        case 7: _t->on_pushButton_DeleteFile_clicked(); break;
        case 8: _t->on_pushButton_PageUp_clicked(); break;
        case 9: _t->on_pushButton_PageDown_clicked(); break;
        case 10: _t->on_tableView_File_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 11: _t->on_pushButton_PlayerStart_clicked(); break;
        case 12: _t->on_pushButton_PlayerStop_clicked(); break;
        case 13: _t->on_pushButton_PlayerFullscreen_clicked(); break;
        case 14: _t->on_horizontalSlider_Player_sliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->on_dateEdit_Start_dateChanged((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 16: _t->on_dateEdit_Stop_dateChanged((*reinterpret_cast< const QDate(*)>(_a[1]))); break;
        case 17: _t->on_pushButton_DownloadDir_clicked(); break;
        case 18: _t->on_pushButton_Delete_clicked(); break;
        case 19: _t->on_pushButton_Add_clicked(); break;
        case 20: _t->on_pushButton_DeleteAllFile_clicked(); break;
        case 21: _t->slotShowSmall(); break;
        case 22: _t->slotHelp(); break;
        case 23: _t->slotShowMaxRestore(); break;
        case 24: _t->on_pushButton_update_clicked(); break;
        case 25: _t->on_pushButton_PlayerPause_clicked(); break;
        case 26: _t->on_pBt_shift_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData Dialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject Dialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_Dialog,
      qt_meta_data_Dialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Dialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Dialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Dialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Dialog))
        return static_cast<void*>(const_cast< Dialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int Dialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 27)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 27;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
