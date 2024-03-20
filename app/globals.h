#ifndef GLOBALCOPYLINEUTIL_H
#define GLOBALCOPYLINEUTIL_H

#include "qtenummapper.h"
#include <QDateTime>
#include <QSettings>

QtEnumMapper& getEnumMapper();

inline QSettings* getQSettings() {
    return new QSettings(QSettings::IniFormat, QSettings::UserScope, "ADPStampInventory", "Stamp Inventory ADP");
}

#endif // GLOBALCOPYLINEUTIL_H
