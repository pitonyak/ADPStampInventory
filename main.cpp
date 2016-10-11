#include <QApplication>
#include "mainwindow.h"
#include "simpleloggeradp.h"
#include "globals.h"
#include <QLoggingCategory>

SimpleLoggerADP logger;
QtEnumMapper enumMapper;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Fedora disables qDebug output at the moment
    // New configuration thing, so, re-enable it as follows (if you want).
    // But, you do NOT want all those mouse movement messages
    QLoggingCategory::setFilterRules("*.debug=true\n"
                                     "qt.qpa.input*.debug=false\n"
                                     "qt.widgets.gestures*.debug=false");


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
