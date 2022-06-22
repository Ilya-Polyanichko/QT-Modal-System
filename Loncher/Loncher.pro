#-------------------------------------------------
#
# Project created by QtCreator 2022-05-01T19:53:46
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

TARGET = Loncher
TEMPLATE = app


SOURCES += main.cpp\
        loncher.cpp \
    access.cpp

HEADERS  += loncher.h \
    access.h

INCLUDEPATH += ../

win32:RC_FILE = file.rc

LIBS += "../build-Loncher-Desktop_Qt_6_2_4_MinGW_64_bit-Debug/debug/QlibApplication.dll"

LIBS += "../build-Loncher-Desktop_Qt_6_2_4_MinGW_64_bit-Release/release/QlibApplication.dll"
