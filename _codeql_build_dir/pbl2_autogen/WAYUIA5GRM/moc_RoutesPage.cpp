/****************************************************************************
** Meta object code from reading C++ file 'RoutesPage.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../src/UI/RoutesPage.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RoutesPage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_RoutesPage_t {
    uint offsetsAndSizes[18];
    char stringdata0[11];
    char stringdata1[13];
    char stringdata2[1];
    char stringdata3[18];
    char stringdata4[16];
    char stringdata5[11];
    char stringdata6[12];
    char stringdata7[14];
    char stringdata8[13];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_RoutesPage_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_RoutesPage_t qt_meta_stringdata_RoutesPage = {
    {
        QT_MOC_LITERAL(0, 10),  // "RoutesPage"
        QT_MOC_LITERAL(11, 12),  // "onSearchById"
        QT_MOC_LITERAL(24, 0),  // ""
        QT_MOC_LITERAL(25, 17),  // "onSearchByAirline"
        QT_MOC_LITERAL(43, 15),  // "onSearchByRoute"
        QT_MOC_LITERAL(59, 10),  // "onAddRoute"
        QT_MOC_LITERAL(70, 11),  // "onEditRoute"
        QT_MOC_LITERAL(82, 13),  // "onDeleteRoute"
        QT_MOC_LITERAL(96, 12)   // "refreshTable"
    },
    "RoutesPage",
    "onSearchById",
    "",
    "onSearchByAirline",
    "onSearchByRoute",
    "onAddRoute",
    "onEditRoute",
    "onDeleteRoute",
    "refreshTable"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_RoutesPage[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   56,    2, 0x08,    1 /* Private */,
       3,    0,   57,    2, 0x08,    2 /* Private */,
       4,    0,   58,    2, 0x08,    3 /* Private */,
       5,    0,   59,    2, 0x08,    4 /* Private */,
       6,    0,   60,    2, 0x08,    5 /* Private */,
       7,    0,   61,    2, 0x08,    6 /* Private */,
       8,    0,   62,    2, 0x08,    7 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject RoutesPage::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_RoutesPage.offsetsAndSizes,
    qt_meta_data_RoutesPage,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_RoutesPage_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<RoutesPage, std::true_type>,
        // method 'onSearchById'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSearchByAirline'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSearchByRoute'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onAddRoute'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onEditRoute'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDeleteRoute'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'refreshTable'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void RoutesPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<RoutesPage *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onSearchById(); break;
        case 1: _t->onSearchByAirline(); break;
        case 2: _t->onSearchByRoute(); break;
        case 3: _t->onAddRoute(); break;
        case 4: _t->onEditRoute(); break;
        case 5: _t->onDeleteRoute(); break;
        case 6: _t->refreshTable(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *RoutesPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RoutesPage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_RoutesPage.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int RoutesPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
