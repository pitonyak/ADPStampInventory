#include "tableeditfielddescriptors.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

TableEditFieldDescriptors::TableEditFieldDescriptors(QObject *parent) :
  QObject(parent)
{
}

TableEditFieldDescriptors::TableEditFieldDescriptors(const TableEditFieldDescriptors& fieldDescriptors) :
  QObject(nullptr)
{
  copy(fieldDescriptors);
}

TableEditFieldDescriptors::TableEditFieldDescriptors(const TableEditFieldDescriptors& fieldDescriptors, QObject *parent) :
  QObject(parent)
{
  copy(fieldDescriptors);
}

QXmlStreamWriter& TableEditFieldDescriptors::writeXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("FieldViewDescriptors");
  if (!getName().isEmpty())
    writer.writeAttribute("name", getName());

  for (int i=0; i<size(); ++i) {
    at(i).writeXml(writer);
  }
  writer.writeEndElement();
  return writer;
}

TableEditFieldDescriptors TableEditFieldDescriptors::readXml(QXmlStreamReader& reader)
{
  TableEditFieldDescriptors descriptors;
  bool foundMainTag = false;
  while (!reader.atEnd()) {
    if (reader.isStartElement()) {
      if (reader.name().compare("FieldViewDescriptors", Qt::CaseInsensitive) == 0) {

        if (reader.attributes().hasAttribute("name"))
          descriptors.setName(reader.attributes().value("name").toString());

        if (foundMainTag) {
          // Found the main tag a second time!
          break;
        }
        foundMainTag = true;
        reader.readNext();
      } else if (reader.name().compare("FieldViewDescriptor", Qt::CaseInsensitive) == 0) {
        TableEditFieldDescriptor x = TableEditFieldDescriptor::readXml(reader);
        descriptors.append(x);
      } else {
        // Unexpected element, so obviously we are finished with the Field!
        // qDebug(qPrintable(QString("Found unexpected XML element '%1' in field '%2'").arg(reader.name().toString()).arg(field.getName())));
        break;
      }
    } else if (reader.isStartDocument()) {
      reader.readNext();
    } else if (reader.isEndElement()) {
      //qDebug(qPrintable(QString("End element with name '%1' in field '%2'").arg(reader.name().toString()).arg(field.getName())));
      if (foundMainTag && reader.name().compare("FieldViewDescriptors", Qt::CaseInsensitive) == 0) {
        reader.readNext();
        break;
      }
      reader.readNext();
    } else {
      reader.readNext();
    }
  }
  return descriptors;
}

const TableEditFieldDescriptors& TableEditFieldDescriptors::copy(const TableEditFieldDescriptors& o)
{
  if (this != &o)
  {
    setName(o.getName());
    m_fields = o.m_fields;
  }
  return *this;
}
