#include "tableeditfielddescriptor.h"
#include "xmlutility.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

TableEditFieldDescriptor::TableEditFieldDescriptor(QObject *parent) :
  QObject(parent), m_readOnly(false), m_fieldDisplaySeparator("/")
{
}

TableEditFieldDescriptor::TableEditFieldDescriptor(const TableEditFieldDescriptor& fieldDescriptor, QObject *parent) :
  QObject(parent)
{
  copy(fieldDescriptor);
}

TableEditFieldDescriptor::TableEditFieldDescriptor(const TableEditFieldDescriptor& fieldDescriptor) :
  QObject(nullptr)
{
  copy(fieldDescriptor);
}

TableEditFieldDescriptor::TableEditFieldDescriptor(const QString& tableName, const QString& fieldName, const QString& displayName, const QString& separator, const bool readOnly, QObject *parent) :
  QObject(parent), m_readOnly(readOnly), m_fieldDisplaySeparator(separator),
  m_tableName(tableName), m_fieldName(fieldName), m_displayName(displayName)
{
}

TableEditFieldDescriptor::TableEditFieldDescriptor(const TableEditFieldDescriptor& linkField, const QString& tableName, const QString& fieldName, const QString& displayName, const QString& separator, const bool readOnly, QObject *parent) :
  QObject(parent), m_readOnly(readOnly), m_fieldDisplaySeparator(separator),
  m_tableName(tableName), m_fieldName(fieldName), m_displayName(displayName)
{
  append(linkField);
}

TableEditFieldDescriptor::TableEditFieldDescriptor(const QList<TableEditFieldDescriptor>& linkFields, const QString& tableName, const QString& fieldName, const QString& displayName, const QString& separator, const bool readOnly, QObject *parent) :
  QObject(parent), m_readOnly(readOnly), m_fieldDisplaySeparator(separator),
  m_tableName(tableName), m_fieldName(fieldName), m_displayName(displayName)
{
  append(linkFields);
}



const TableEditFieldDescriptor& TableEditFieldDescriptor::copy(const TableEditFieldDescriptor& o)
{
  if (&o != this)
  {
    setTableName(o.getTableName());
    setFieldName(o.getFieldName());
    setDisplayName(o.getDisplayName());
    setReadOnly(o.isReadOnly());
    setFieldDisplaySeparator(o.getFieldDisplaySeparator());
    m_fields = o.m_fields;
  }
  return *this;
}


QXmlStreamWriter& TableEditFieldDescriptor::writeXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("FieldViewDescriptor");
  if (!getTableName().isEmpty())
    writer.writeAttribute("table", getTableName());
  if (!getFieldName().isEmpty())
    writer.writeAttribute("field", getFieldName());
  if (!getDisplayName().isEmpty())
    writer.writeAttribute("display", getDisplayName());
  if (!getFieldDisplaySeparator().isEmpty())
    writer.writeAttribute("separator", getFieldDisplaySeparator());
  writer.writeAttribute("readonly", XMLUtility::booleanToString(isReadOnly()));

  if (isLinked())
  {
    writer.writeStartElement("Descriptors");
    for (int i=0; i<linkFieldSize(); ++i)
    {
      writer << getLinkField(i);
    }
    writer.writeEndElement();
  }
  writer.writeEndElement();
  return writer;
}

TableEditFieldDescriptor TableEditFieldDescriptor::readXml(QXmlStreamReader& reader)
{
  TableEditFieldDescriptor fieldDescriptor;
  bool foundMainTag = false;
  while (!reader.atEnd()) {
    if (reader.isStartElement()) {
      if (reader.name().compare("FieldViewDescriptor", Qt::CaseInsensitive) == 0) {
        if (foundMainTag) {
          // Found the main tag a second time!
          break;
        }
        foundMainTag = true;
        if (reader.attributes().hasAttribute("table"))
          fieldDescriptor.setTableName(reader.attributes().value("table").toString());
        if (reader.attributes().hasAttribute("field"))
          fieldDescriptor.setFieldName(reader.attributes().value("field").toString());
        if (reader.attributes().hasAttribute("display"))
          fieldDescriptor.setDisplayName(reader.attributes().value("display").toString());
        if (reader.attributes().hasAttribute("separator"))
          fieldDescriptor.setFieldDisplaySeparator(reader.attributes().value("separator").toString());

        if (reader.attributes().hasAttribute("readonly")) {
          fieldDescriptor.setReadOnly(XMLUtility::stringToBoolean(reader.attributes().value("readonly").toString()));
        }
        reader.readNext();
      } else if (reader.name().compare("Descriptors", Qt::CaseInsensitive) == 0) {
        reader.readNext();
        while (atDescriptor(reader))
        {
          fieldDescriptor.append(readXml(reader));
        }
      } else {
        // Unexpected element, so obviously we are finished with the Field!
        // qDebug(qPrintable(QString("Found unexpected XML element '%1' in field '%2'").arg(reader.name().toString()).arg(field.getName())));
        break;
      }
    } else if (reader.isStartDocument()) {
      reader.readNext();
    } else if (reader.isEndElement()) {
      //qDebug(qPrintable(QString("End element with name '%1' in field '%2'").arg(reader.name().toString()).arg(field.getName())));
      if (foundMainTag && reader.name().compare("FieldViewDescriptor", Qt::CaseInsensitive) == 0) {
        reader.readNext();
        break;
      }
      reader.readNext();
    } else {
      reader.readNext();
    }
  }

  return fieldDescriptor;
}

bool TableEditFieldDescriptor::atDescriptor(QXmlStreamReader& reader)
{
  if (!reader.atEnd())
  {
    if (reader.isStartElement())
    {
      return reader.name().compare("FieldViewDescriptor", Qt::CaseInsensitive) == 0;
    }
    if (reader.isEndElement())
    {
      if (reader.name().compare("FieldViewDescriptor", Qt::CaseInsensitive) == 0)
      {
        reader.readNext();
        return atDescriptor(reader);
      }
      else if (reader.name().compare("Descriptors", Qt::CaseInsensitive) == 0)
      {
        reader.readNext();
      }
    }
  }
  return false;
}
