#-------------------------------------------------
#
# Project created by QtCreator 2022-05-01T21:09:10
#
#-------------------------------------------------

QT       += widgets sql

TARGET = QlibApplication
TEMPLATE = lib

DEFINES += QLIBAPPLICATION_LIBRARY

SOURCES += qlibapplication.cpp \
    management_menu.cpp \
    management_roles.cpp \
    management_users.cpp \
    setting_line_one.cpp \
    setting_line_two.cpp \
    stage_testing_one.cpp \
    stage_testing_two.cpp

HEADERS += qlibapplication.h\
    management_menu.h \
    management_roles.h \
    management_users.h \
        qlibapplication_global.h \
    setting_line_one.h \
    setting_line_two.h \
    stage_testing_one.h \
    stage_testing_two.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
