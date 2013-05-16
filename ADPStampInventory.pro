#-------------------------------------------------
#
# Project created by QtCreator 2012-01-08T18:24:15
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++0x -g


#-------------------------------------------------
#
# I use c++0x so that I can use things such as nullptr
# I use __STRICT_ANSI__ because there is a bug in mingw when used with c++0x.
#
#-------------------------------------------------
#QMAKE_CXXFLAGS += -std=c++0x -U__STRICT_ANSI__ -g

DEFINES *= QT_USE_QSTRINGBUILDER

TARGET = ADPStampInventory
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    stampdb.cpp \
    scrollmessagebox.cpp \
    sqlfieldtype.cpp \
    sqlfieldtypemaster.cpp \
    csvreader.cpp \
    csvcontroller.cpp \
    csvwriter.cpp \
    csvreaderdialog.cpp \
    csvline.cpp \
    csvcolumn.cpp \
    typemapper.cpp \
    constants.cpp

HEADERS  += mainwindow.h \
    stampdb.h \
    scrollmessagebox.h \
    sqlfieldtype.h \
    sqlfieldtypemaster.h \
    csvreader.h \
    csvcontroller.h \
    nullptr.h \
    csvwriter.h \
    csvreaderdialog.h \
    csvline.h \
    csvcolumn.h \
    typemapper.h \
    constants.h

FORMS    += mainwindow.ui




















