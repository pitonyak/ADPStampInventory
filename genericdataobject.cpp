#include "genericdataobject.h"

GenericDataObject::GenericDataObject(QObject *parent) :
  QObject(parent)
{
}

const QVariant& GenericDataObject::getValue(const QString& name) const
{
  return m_properties.value(name);
}

void GenericDataObject::setValue(const QString &name, const QVariant& value)
{
  m_properties.insert(name, value);
}

QString GenericDataObject::getString(const QString& name) const
{
  return hasValue(name) ? m_properties.value(name).toString() : "";
}

QString GenericDataObject::getString(const QString& name, const QString& defaultValue) const
{
  return hasValue(name) ? m_properties.value(name).toString() : defaultValue;
}


int GenericDataObject::getInt(const QString& name, const int defaultValue) const
{
  if (hasValue(name))
  {
    bool ok = false;
    int i = m_properties.value(name).toInt(&ok);
    if (ok)
    {
      return i;
    }
  }
  return defaultValue;
}

double GenericDataObject::getDouble(const QString& name, const double defaultValue) const
{
  if (hasValue(name))
  {
    bool ok = false;
    double i = m_properties.value(name).toDouble(&ok);
    if (ok)
    {
      return i;
    }
  }
  return defaultValue;
}

QDate GenericDataObject::getDate(const QString& name) const
{
  return hasValue(name) ? m_properties.value(name).toDate() : QDate::currentDate();
}

QDate GenericDataObject::getDate(const QString& name, const QDate& defaultValue) const
{
  return hasValue(name) ? m_properties.value(name).toDate() : defaultValue;
}


QDateTime GenericDataObject::getDateTime(const QString& name) const
{
  return hasValue(name) ? m_properties.value(name).toDateTime() : QDateTime::currentDateTime();
}

QDateTime GenericDataObject::getDateTime(const QString& name, const QDateTime& defaultValue) const
{
  return hasValue(name) ? m_properties.value(name).toDateTime() : defaultValue;
}

