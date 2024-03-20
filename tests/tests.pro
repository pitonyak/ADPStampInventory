QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ADPStampInventoryUnitTests

TEMPLATE = app

DEFINES *= QT_USE_QSTRINGBUILDER

CONFIG += console
QT += testlib

SOURCES += testmain.cpp \
    testall.cpp \
    testvaluecomparer.cpp \
    ../app/valuecomparer.cpp \

HEADERS += \
    testall.h \
    testvaluecomparer.h \
    ../app/valuecomparer.h \

INCLUDEPATH += \
    ../app 

DESTDIR = $$PWD/../build
