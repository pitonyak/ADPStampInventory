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
    constants.cpp \
    sqldialog.cpp \
    simpleloggerroutinginfo.cpp \
    simpleloggeradp.cpp \
    logroutinginfotablemodel.cpp \
    logroutinginfodialog.cpp \
    logmessagequeue.cpp \
    logmessagecontainer.cpp \
    logconfigdialog.cpp \
    checkboxonlydelegate.cpp \
    xmlutility.cpp \
    messagecomponenttablemodel.cpp \
    linkbackfilterdelegate.cpp \
    qtenummapper.cpp \
    dataobjectbase.cpp \
    dataobjectcountry.cpp \
    dataobjectdealer.cpp \
    dataobjectcatalogtype.cpp \
    dataobjectvaluetype.cpp \
    dataobjectcatalog.cpp \
    dataobjectstamplocation.cpp \
    dataobjectinventory.cpp \
    dataobjectvaluesource.cpp \
    genericdataobject.cpp \
    genericdatacollection.cpp \
    tableeditorgenericgrid.cpp \
    genericdatacollectiontablemodel.cpp \
    actiontrackerobject.cpp \
    actiontrackerobjectbase.cpp \
    actiontracker.cpp \
    comparer.cpp \
    tablefieldevalnode.cpp \
    tablefieldbinarytreeevalnode.cpp \
    valuefilter.cpp

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
    constants.h \
    sqldialog.h \
    simpleloggerroutinginfo.h \
    simpleloggeradp.h \
    logroutinginfotablemodel.h \
    logroutinginfodialog.h \
    logmessagequeue.h \
    logmessagecontainer.h \
    logconfigdialog.h \
    checkboxonlydelegate.h \
    xmlutility.h \
    messagecomponenttablemodel.h \
    linkbackfilterdelegate.h \
    globals.h \
    qtenummapper.h \
    dataobjectbase.h \
    dataobjectcountry.h \
    dataobjectdealer.h \
    dataobjectcatalogtype.h \
    dataobjectvaluetype.h \
    dataobjectcatalog.h \
    dataobjectstamplocation.h \
    dataobjectinventory.h \
    dataobjectvaluesource.h \
    genericdataobject.h \
    genericdatacollection.h \
    tableeditorgenericgrid.h \
    genericdatacollectiontablemodel.h \
    actiontrackerobject.h \
    actiontrackerobjectbase.h \
    actiontracker.h \
    comparer.h \
    tablefieldevalnode.h \
    tablefieldbinarytreeevalnode.h \
    valuefilter.h

FORMS    += mainwindow.ui \
    logroutinginfodialog.ui




















