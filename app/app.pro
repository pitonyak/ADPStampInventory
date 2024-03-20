#-------------------------------------------------
#
# Project created by QtCreator 2012-01-08T18:24:15
#
#-------------------------------------------------

QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#QMAKE_CXXFLAGS += -std=c++0x -g
QMAKE_CXXFLAGS += -g


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
    changedobject.cpp \
    changedobjectbase.cpp \
    changetracker.cpp \
    changetrackerbase.cpp \
    checkboxonlydelegate.cpp \
    comparer.cpp \
    configuredialog.cpp \
    constants.cpp \
    csvcolumn.cpp \
    csvcontroller.cpp \
    csvline.cpp \
    csvreader.cpp \
    csvreaderdialog.cpp \
    csvwriter.cpp \
    dbtransactionhandler.cpp \
    describesqlfield.cpp \
    describesqltable.cpp \
    describesqltables.cpp \
    genericdatacollection.cpp \
    genericdatacollections.cpp \
    genericdatacollectionstablemodel.cpp \
    genericdatacollectionstableproxy.cpp \
    genericdatacollectiontabledialog.cpp \
    genericdatacollectiontablefilterdialog.cpp \
    genericdatacollectiontablemodel.cpp \
    genericdatacollectiontablesearchdialog.cpp \
    genericdataobject.cpp \
    genericdataobjectfilter.cpp \
    genericdataobjectlessthan.cpp \
    imageutility.cpp \
    linkbackfilterdelegate.cpp \
    linkedfieldcache.cpp \
    linkedfieldselectioncache.cpp \
    mainwindow.cpp \
    qtenummapper.cpp \
    scrollmessagebox.cpp \
    searchoptions.cpp \
    sqldialog.cpp \
    sqlfieldtype.cpp \
    sqlfieldtypemaster.cpp \
    stampdb.cpp \
    stampschema.cpp \
    stringutil.cpp \
    tableeditfielddescriptor.cpp \
    tableeditfielddescriptors.cpp \
    tableeditorgenericgrid.cpp \
    tablefieldbinarytreeevalnode.cpp \
    tablefieldevalnode.cpp \
    tablesortfield.cpp \
    tablesortfielddialog.cpp \
    tablesortfieldtablemodel.cpp \
    typemapper.cpp \
    valuecomparer.cpp \
    variantcomparer.cpp \
    xmlutility.cpp \

HEADERS  += \
    changedobject.h \
    changedobjectbase.h \
    changetracker.h \
    changetrackerbase.h \
    checkboxonlydelegate.h \
    comparer.h \
    configuredialog.h \
    constants.h \
    csvcolumn.h \
    csvcontroller.h \
    csvline.h \
    csvreader.h \
    csvreaderdialog.h \
    csvwriter.h \
    dbtransactionhandler.h \
    describesqlfield.h \
    describesqltable.h \
    describesqltables.h \
    genericdatacollection.h \
    genericdatacollections.h \
    genericdatacollectionstablemodel.h \
    genericdatacollectionstableproxy.h \
    genericdatacollectiontabledialog.h \
    genericdatacollectiontablefilterdialog.h \
    genericdatacollectiontablemodel.h \
    genericdatacollectiontablesearchdialog.h \
    genericdataobject.h \
    genericdataobjectfilter.h \
    genericdataobjectlessthan.h \
    globals.h \
    imageutility.h \
    linkbackfilterdelegate.h \
    linkedfieldcache.h \
    linkedfieldselectioncache.h \
    mainwindow.h \
    nullptr.h \
    qtenummapper.h \
    scrollmessagebox.h \
    searchoptions.h \
    sqldialog.h \
    sqlfieldtype.h \
    sqlfieldtypemaster.h \
    stampdb.h \
    stampschema.h \
    stringutil.h \
    tableeditfielddescriptor.h \
    tableeditfielddescriptors.h \
    tableeditorgenericgrid.h \
    tablefieldbinarytreeevalnode.h \
    tablefieldevalnode.h \
    tablesortfield.h \
    tablesortfielddialog.h \
    tablesortfieldtablemodel.h \
    typemapper.h \
    valuecomparer.h \
    variantcomparer.h \
    xmlutility.h \

FORMS    += mainwindow.ui

DESTDIR = $$PWD/../build

