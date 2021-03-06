#ifndef GLOBALCOPYLINEUTIL_H
#define GLOBALCOPYLINEUTIL_H

#include "simpleloggeradp.h"
#include "qtenummapper.h"
#include <QDateTime>
#include <QSettings>

//**************************************************************************
/*! \brief Get the single global instance of the logger.
 *
 * \returns The single global instance of the logger.
 ***************************************************************************/
SimpleLoggerADP& getLogger();

QtEnumMapper& getEnumMapper();

#define ERROR_MSG(msg, level) errorMessage((msg), QString(QObject::tr("%1:%2")).arg(__FILE__, QString::number(__LINE__)), QDateTime::currentDateTime(), (level));
#define WARN_MSG( msg, level) warnMessage((msg),  QString(QObject::tr("%1:%2")).arg(__FILE__, QString::number(__LINE__)), QDateTime::currentDateTime(), (level));
#define INFO_MSG( msg, level) infoMessage((msg),  QString(QObject::tr("%1:%2")).arg(__FILE__, QString::number(__LINE__)), QDateTime::currentDateTime(), (level));
#define TRACE_MSG(msg, level) traceMessage((msg), QString(QObject::tr("%1:%2")).arg(__FILE__, QString::number(__LINE__)), QDateTime::currentDateTime(), (level));
#define DEBUG_MSG(msg, level) debugMessage((msg), QString(QObject::tr("%1:%2")).arg(__FILE__, QString::number(__LINE__)), QDateTime::currentDateTime(), (level));
#define USER_MSG( msg, level) userMessage((msg),  QString(QObject::tr("%1:%2")).arg(__FILE__, QString::number(__LINE__)), QDateTime::currentDateTime(), (level));


void errorMessage(const QString& message, const QString& location, const QDateTime& dateTime, int level=1);
void warnMessage(const QString& message, const QString& location, const QDateTime& dateTime, int level=1);
void infoMessage(const QString& message, const QString& location, const QDateTime& dateTime, int level=1);
void traceMessage(const QString& message, const QString& location, const QDateTime& dateTime, int level=1);
void debugMessage(const QString& message, const QString& location, const QDateTime& dateTime, int level=1);
void userMessage(const QString& message, const QString& location, const QDateTime& dateTime, int level=1);

inline QSettings* getQSettings() {
    return new QSettings(QSettings::IniFormat, QSettings::UserScope, "ADPStampInventory", "Stamp Inventory ADP");
}

#endif // GLOBALCOPYLINEUTIL_H
