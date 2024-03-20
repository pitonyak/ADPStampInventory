QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ADPStampInventoryUnitTests

TEMPLATE = app

DEFINES *= QT_USE_QSTRINGBUILDER

CONFIG += console
QT += testlib

SOURCES += \
    testmain.cpp \
    testall.cpp \
    ../app/imageutility.cpp \

HEADERS += \
    testall.h \
    ../app/imageutility.h \

INCLUDEPATH += \
    ../app 

DESTDIR = $$PWD/../build
