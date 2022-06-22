#include "qlibapplication.h"

#include <QDebug>
#include <QApplication>
#include <QMetaType>


QlibApplication::QlibApplication(QWidget* parent) : QWidget(parent) {

    qRegisterMetaType<management_menu>("form_management_menu");
    qRegisterMetaType<management_roles>("form_management_roles");
    qRegisterMetaType<management_users>("form_management_users");

    qRegisterMetaType<setting_line_one>("form_setting_line_one");
    qRegisterMetaType<setting_line_two>("form_setting_line_two");
    qRegisterMetaType<stage_testing_one>("form_stage_testing_one");
    qRegisterMetaType<stage_testing_two>("form_stage_testing_two");

}

QlibApplication::~QlibApplication() {}

QWidget* QlibApplication::getApp(QString object_name_app) {

    int id = QMetaType::type(object_name_app.toStdString().c_str());
    if (id != QMetaType::UnknownType) {

        return static_cast<QWidget*>(QMetaType::create(id));

     } else {

        return nullptr;

    }
}

bool QlibApplication::check_connection(QString object_name_app) {

    int id = QMetaType::type(object_name_app.toStdString().c_str());
    if (id != QMetaType::UnknownType) {

        return true;

     } else {

        return false;

    }
}
