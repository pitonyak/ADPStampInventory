#include <QApplication>
#include "mainwindow.h"
#include "simpleloggeradp.h"
#include "globals.h"
#include <QLoggingCategory>
#include <iostream>

SimpleLoggerADP logger;
QtEnumMapper enumMapper;

//
// I can write my own message handler, and then install it, but that seems very heavy handed.
// But this is vaguely how I would do it.
//

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // Ignore the qt events but allow my events to still be printed.
    if (type == QtDebugMsg && context.category != nullptr) {
      QString catStr(context.category);
      if (catStr.startsWith("qt.")) {
        return;
      }
    }

    // consider these:
    //const char *file = context.file ? context.file : "";
    //const char *function = context.function ? context.function : "";

    // Write the date of recording
    std::cout << qPrintable(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz "));
    // By type determine to what level belongs message
    switch (type)
    {
    case QtInfoMsg:     std::cout << "INF "; break;
    case QtDebugMsg:    std::cout << "DBG "; break;
    case QtWarningMsg:  std::cout << "WRN "; break;
    case QtCriticalMsg: std::cout << "CRT "; break;
    case QtFatalMsg:    std::cout << "FTL "; break;
    }
    // Write to the output category of the message and the message itself
    std::cout << context.category << ": " << qPrintable(msg) << std::endl;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // qInstallMessageHandler(messageHandler);

    // Fedora disables qDebug output at the moment
    // New configuration thing, so, re-enable it as follows (if you want).
    // But, you do NOT want all those mouse movement messages
    QLoggingCategory::setFilterRules("*.debug=true\n"
                                     "qt.gui.shortcutmap=false\n"
                                     "qt.qpa.input*.debug=false\n"
                                     "qt.qpa.events*.debug=false\n"
                                     "qt.qpa.wayland=false\n"
                                     "qt.qpa.wayland.backingstore=false\n"
                                     "qt.pointer*.debug=false\n"
                                     "qt.accessibility*.debug=false\n"
                                     "qt.text.layout*.debug=false\n"
                                     "qt.text*.debug=false\n"
                                     "qt.qpa.wayland.input=false\n"
                                     "qt.widgets.gestures*.debug=false");
// text.layout drawing

    QCoreApplication::setOrganizationDomain("pitonyak.org");
    QCoreApplication::setOrganizationName("Pitonyak");
    QCoreApplication::setApplicationName("Stamp Inventory ADP");
    QCoreApplication::setApplicationVersion("1.0.0");

    MainWindow w;
    w.show();

    return a.exec();
}

QtEnumMapper& getEnumMapper()
{
  return enumMapper;
}

//**************************************************************************
//**
//** Logging helpers.
//**
//**************************************************************************
SimpleLoggerADP& getLogger()
{
  return logger;
}

void errorMessage(const QString& message, const QString& location, const QDateTime& dateTime, int level)
{
  logger.receiveMessage(message, location, dateTime, SimpleLoggerRoutingInfo::ErrorMessage, level);
}

void warnMessage(const QString& message, const QString& location, const QDateTime& dateTime, int level)
{
  logger.receiveMessage(message, location, dateTime, SimpleLoggerRoutingInfo::WarningMessage, level);
}

void infoMessage(const QString& message, const QString& location, const QDateTime& dateTime, int level)
{
  logger.receiveMessage(message, location, dateTime, SimpleLoggerRoutingInfo::InformationMessage, level);
}

void traceMessage(const QString& message, const QString& location, const QDateTime& dateTime, int level)
{
  logger.receiveMessage(message, location, dateTime, SimpleLoggerRoutingInfo::TraceMessage, level);
}

void debugMessage(const QString& message, const QString& location, const QDateTime& dateTime, int level)
{
  logger.receiveMessage(message, location, dateTime, SimpleLoggerRoutingInfo::DebugMessage, level);
}

void userMessage(const QString& message, const QString& location, const QDateTime& dateTime, int level)
{
    logger.receiveMessage(message, location, dateTime, SimpleLoggerRoutingInfo::UserMessage, level);
}
