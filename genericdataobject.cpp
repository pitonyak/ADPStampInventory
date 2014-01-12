#include "genericdataobject.h"

GenericDataObject::GenericDataObject(QObject *parent) :
  QObject(parent)
{
}

const QVariant& GenericDataObject::getValue(const QString& name) const
{
  return m_properties.value(name.toLower());
}

void GenericDataObject::setValue(const QString &name, const QVariant& value)
{
  m_properties.insert(name.toLower(), value);
}

QString GenericDataObject::getString(const QString& name) const
{
  QString lower = name.toLower();
  return hasValueNoCase(lower) ? m_properties.value(lower).toString() : "";
}

QString GenericDataObject::getString(const QString& name, const QString& defaultValue) const
{
  QString lower = name.toLower();
  return hasValueNoCase(lower) ? m_properties.value(lower).toString() : defaultValue;
}


int GenericDataObject::getInt(const QString& name, const int defaultValue) const
{
  QString lower = name.toLower();
  if (hasValueNoCase(lower))
  {
    bool ok = false;
    int i = m_properties.value(lower).toInt(&ok);
    if (ok)
    {
      return i;
    }
  }
  return defaultValue;
}

double GenericDataObject::getDouble(const QString& name, const double defaultValue) const
{
  QString lower = name.toLower();
  if (hasValueNoCase(lower))
  {
    bool ok = false;
    double i = m_properties.value(lower).toDouble(&ok);
    if (ok)
    {
      return i;
    }
  }
  return defaultValue;
}

QDate GenericDataObject::getDate(const QString& name) const
{
  QString lower = name.toLower();
  return hasValueNoCase(lower) ? m_properties.value(lower).toDate() : QDate::currentDate();
}

QDate GenericDataObject::getDate(const QString& name, const QDate& defaultValue) const
{
  QString lower = name.toLower();
  return hasValueNoCase(lower) ? m_properties.value(lower).toDate() : defaultValue;
}


QDateTime GenericDataObject::getDateTime(const QString& name) const
{
  QString lower = name.toLower();
  return hasValueNoCase(lower) ? m_properties.value(lower).toDateTime() : QDateTime::currentDateTime();
}

QDateTime GenericDataObject::getDateTime(const QString& name, const QDateTime& defaultValue) const
{
  QString lower = name.toLower();
  return hasValueNoCase(lower) ? m_properties.value(lower).toDateTime() : defaultValue;
}

