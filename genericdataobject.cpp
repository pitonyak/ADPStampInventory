#include "genericdataobject.h"
#include "genericdatacollection.h"

#include <QUuid>

GenericDataObject::GenericDataObject(QObject *parent) :
  QObject(parent)
{
}

GenericDataObject::GenericDataObject(const GenericDataObject& obj, QObject *parent) : QObject(parent)
{
  GenericDataObject::operator=(obj);
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


const GenericDataObject& GenericDataObject::operator=(const GenericDataObject& obj)
{
  // Note that the parent object is NOT copied.
  if (this != &obj)
  {
    m_properties.clear();
    QHashIterator<QString, QVariant> i(obj.m_properties);
    while (i.hasNext())
    {
      i.next();
      m_properties.insert(i.key(), i.value());
    }
  }
  return *this;
}

bool GenericDataObject::lessThan(const GenericDataObject& obj, const QList<TableSortField *> &sortFields) const
{
    return compare(obj, sortFields) < 0;
}

int GenericDataObject::compare(const GenericDataObject& obj, const QList<TableSortField *> &sortFields) const
{
    int rc = 0;
    QListIterator<TableSortField*> i(sortFields);
    while (rc == 0 && i.hasNext())
    {
        const TableSortField* sortField = i.next();
        if (sortField != nullptr)
        {
            if (!obj.hasValueNoCase(sortField->fieldName()))
            {
                if (hasValueNoCase(sortField->fieldName()))
                {
                    return sortField->isAscending() ? 1 : -1;
                }
            }
            else if (!hasValueNoCase(sortField->fieldName()))
            {
                return sortField->isAscending() ? -1 : 1;
            }
            else
            {
                QVariant v1 = getValue(sortField->fieldName());
                QVariant v2 = obj.getValue(sortField->fieldName());
                rc = sortField->valueCompare(v1, v2);
            }
        }
    }
    return rc;
}

int GenericDataObject::compare(const GenericDataObject& obj, const QStringList& fields, Qt::CaseSensitivity sensitive) const
{
    int rc = 0;
    for (int i=0; rc==0 && i<fields.count(); ++i)
    {
        if (!obj.hasValueNoCase(fields.at(i)))
        {
            if (hasValueNoCase(fields.at(i)))
            {
                return 1;
            }
        }
        else if (!hasValueNoCase(fields.at(i)))
        {
            return -1;
        }
        else
        {
            QVariant v1 = getValue(fields.at(i));
            QVariant v2 = obj.getValue(fields.at(i));
            if (v1.type() != v2.type())
            {
                // This should never happen.
                qDebug(qPrintable(QString(tr("Comparing two variants of different types in column : %1")).arg(fields.at(i))));
                rc = v1.toString().compare(v2.toString(), sensitive);
            }
            else if (v1 != v2)
            {
                switch (v1.type())
                {
                case QVariant::Int:
                    rc = v1.toInt() < v2.toInt() ? -1 : 1;
                    break;
                case QVariant::String:
                    rc = v1.toString().compare(v2.toString(), sensitive);
                    break;
                case QVariant::Double:
                    rc = v1.toDouble() < v2.toDouble() ? -1 : 1;
                    break;
                case QVariant::Date:
                    rc = v1.toDate() < v2.toDate() ? -1 : 1;
                    break;
                case QVariant::DateTime:
                    rc = v1.toDateTime() < v2.toDateTime() ? -1 : 1;
                    break;
                case QVariant::LongLong:
                    rc = v1.toLongLong() < v2.toLongLong() ? -1 : 1;
                    break;
                case QVariant::ULongLong:
                    rc = v1.toULongLong() < v2.toULongLong() ? -1 : 1;
                    break;
                case QVariant::UInt:
                    rc = v1.toUInt() < v2.toUInt() ? -1 : 1;
                    break;
                case QVariant::Time:
                    rc = v1.toTime() < v2.toTime() ? -1 : 1;
                    break;
                case QVariant::Uuid:
                    rc = v1.toUuid() < v2.toUuid() ? -1 : 1;
                    break;
                case QVariant::Char:
                    rc = v1.toChar() < v2.toChar() ? -1 : 1;
                    break;
                case QVariant::Bool:
                    rc = v1.toBool() < v2.toBool() ? -1 : 1;
                    break;
                default:
                    qDebug(qPrintable(QString(tr("Comparing two unsupported types in column : %1")).arg(fields.at(i))));
                    rc = v1.toString().compare(v2.toString(), sensitive);
                    break;
                }
            }
        }
    }
    return rc;
}

