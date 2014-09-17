#include "describesqltable.h"
#include "sqlfieldtypemaster.h"
#include <QXmlStreamWriter>

DescribeSqlTable::DescribeSqlTable()
{
}

DescribeSqlTable::DescribeSqlTable(const DescribeSqlTable& table)
{
  copy(table);
}

DescribeSqlTable::DescribeSqlTable(const QString definitions[], const int n, bool idIsAutoInc, const SqlFieldTypeMaster *typemaster)
{
  if (definitions != nullptr) {
    // first three describe the table
    // Next, groups of 5: Name, View Name, Type, Description, length
    // Find k such that n = 3 + 5 * k
    int delta = 5;
    if (n < 3) {
      qDebug(qPrintable(QString("Cannot build a table with %1 strings").arg(n)));
      return;
    }
    int k = (n - 3) / delta;
    if (n != (k * delta + 3)) {
      qDebug(qPrintable(QString("Cannot build a table with %1 strings, must be representable as (3 + %2k) for some value of k").arg(n).arg(delta)));
      return;
    }
    int i = 0;
    bool ownTypeMaster = false;
    bool ok = true;
    setName(definitions[i++]);
    setViewName(definitions[i++]);
    setDescription(definitions[i++]);
    if (i < n && typemaster == nullptr) {
      ownTypeMaster = true;
      typemaster = new SqlFieldTypeMaster();
    }
    while (i < n) {
      DescribeSqlField field;
      field.setName(definitions[i++]);
      field.setViewName(definitions[i++]);
      SqlFieldType fieldType = typemaster->findByName(definitions[i++]);
      if (!fieldType.isValid()) {
        qDebug(qPrintable(QString("Cannot build a valid type from '%1'").arg(definitions[i-1])));
      }
      field.setDescription(definitions[i++]);
      field.setFieldLength(QString(definitions[i++]).toInt(&ok));
      if (!ok) {
        qDebug(qPrintable(QString("Cannot build a length from '%1'").arg(definitions[i-1])));
      }
      if (idIsAutoInc && field.getName().compare("id") == 0) {
        field.setIsAutoIncrement(true);
      }
      if (!addField(field)) {
        qDebug(qPrintable(QString("Failed to add field at index %1 with name").arg(definitions[i-delta]).arg(field.getName())));
      }
    }
    if (ownTypeMaster) {
      delete typemaster;
    }
  }
}

const DescribeSqlTable& DescribeSqlTable::copy(const DescribeSqlTable& table)
{
  if (this != &table) {
    setName(table.getName());
    setViewName(table.getViewName());
    setDescription(table.getDescription());
    m_names = table.getFieldNames();
    m_fields = table.m_fields;
  }
  return *this;
}

QString DescribeSqlTable::getFieldNameByIndex(const int index) const
{
  if (0 <= index && index < getFieldCount()) {
    return m_names.at(index);
  } else {
    qDebug(qPrintable(QString("Field index = %1 is out of range for table %2").arg(index).arg(getName())));
  }
  return "";
}

DescribeSqlField DescribeSqlTable::getFieldByName(const QString& name) const
{
  QString simpleName = name.toLower();
  if (m_fields.contains(simpleName)) {
    return m_fields.value(simpleName);
  } else {
    qDebug(qPrintable(QString("Field name = '%1' is contained in table %2").arg(name).arg(getName())));
  }
  return DescribeSqlField();
}

DescribeSqlField DescribeSqlTable::getFieldByIndex(const int index) const
{
  return getFieldByName(getFieldNameByIndex(index));
}

bool DescribeSqlTable::addField(const DescribeSqlField& field)
{
  QString simpleName = field.getName().toLower();
  if (m_fields.contains(simpleName)) {
    qDebug(qPrintable(QString("Field name = '%1' is already contained in table %2").arg(simpleName).arg(getName())));
    return false;
  } else {
    m_names.append(simpleName);
    m_fields.insert(simpleName, field);
  }
  return true;
}

void DescribeSqlTable::setFieldKey(const QString& name, bool x)
{
  QString simpleName = name.toLower();
  if (m_fields.contains(simpleName)) {
    m_fields[simpleName].setIsKey(x);
  } else {
    qDebug(qPrintable(QString("Field name = '%1' is not found in table %2").arg(simpleName).arg(getName())));
  }
}

void DescribeSqlTable::setFieldAutoIncrement(const QString& name, bool x)
{
  QString simpleName = name.toLower();
  if (m_fields.contains(simpleName)) {
    m_fields[simpleName].setIsAutoIncrement(x);
  } else {
    qDebug(qPrintable(QString("Field name = '%1' is not found in table %2").arg(simpleName).arg(getName())));
  }
}

void DescribeSqlTable::setFieldRequired(const QString& name, bool x)
{
  QString simpleName = name.toLower();
  if (m_fields.contains(simpleName)) {
    m_fields[simpleName].setIsRequired(x);
  } else {
    qDebug(qPrintable(QString("Field name = '%1' is not found in table %2").arg(simpleName).arg(getName())));
  }
}

void DescribeSqlTable::setFieldLink(const QString& name, const QString& linkTableName, const QString& linkFieldName)
{
  QString simpleName = name.toLower();
  if (m_fields.contains(simpleName)) {
    m_fields[simpleName].setLinkTableName(linkTableName);
    m_fields[simpleName].setLinkFieldName(linkFieldName);
  } else {
    qDebug(qPrintable(QString("Field name = '%1' is not found in table %2").arg(simpleName).arg(getName())));
  }
}

QXmlStreamWriter& DescribeSqlTable::writeXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("Table");
  if (!getName().isEmpty())
    writer.writeAttribute("name", getName());
  if (!getViewName().isEmpty())
    writer.writeAttribute("viewname", getViewName());
  if (!getDescription().isEmpty()) {
    writer.writeAttribute("description", getDescription());
  }
  for (int i=0; i<m_names.size(); ++i) {
    m_fields.value(m_names.at(i)).writeXml(writer);
  }
  writer.writeEndElement();
  return writer;
}
