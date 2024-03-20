#include "qtenummapper.h"


QtEnumMapper::QtEnumMapper()
{
  initialize();
}

QtEnumMapper::~QtEnumMapper()
{
  QMapIterator<QString, QMap<int, QString>* > i(m_valueToName);
  while (i.hasNext())
  {
    i.next();
    delete i.value();
  }
  m_valueToName.clear();
}

QList<QString> QtEnumMapper::getList(const QString& typeName) const
{
  const QMap<int, QString>* enumData = getEnumData(typeName);
  return (enumData != nullptr) ? enumData->values() : QList<QString>();
}

int  QtEnumMapper::stringToEnum(const QString& typeName, const QString& name, const int defaultValue) const
{
  const QMap<int, QString>* enumData = getEnumData(typeName);
  if (enumData != nullptr)
  {
    QMap<int, QString>::const_iterator i = enumData->constBegin();
    while (i != enumData->constEnd())
    {
      if (name.compare(i.value(), Qt::CaseInsensitive) == 0)
      {
        return i.key();
      }
    }
  }
  return defaultValue;
}

QString QtEnumMapper::enumToString(const QString& typeName, const int value, const QString& defaultName) const
{
  const QMap<int, QString>* enumData = getEnumData(typeName);
  return (enumData != nullptr && enumData->contains(value)) ? enumData->value(value) : defaultName;
}

const QMap<int, QString> *QtEnumMapper::getEnumData(const QString &typeName) const
{
  const QMap<int, QString> * rc = m_valueToName.value(typeName, nullptr);
  if (rc == nullptr) {
    rc = m_valueToName.value(typeName.toUpper(), nullptr);
  }
  return rc;
}

void QtEnumMapper::initialize()
{
  // Names match those in the XmlUtility class.
  QMap<int, QString>* caseNames = new QMap<int, QString>();
  (*caseNames)[Qt::CaseInsensitive] = "CaseInsensitive";
  (*caseNames)[Qt::CaseSensitive] = "CaseSensitive";
  m_valueToName["CaseSensitivity"] = caseNames;
}

