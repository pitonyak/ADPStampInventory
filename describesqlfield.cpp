#include "describesqlfield.h"

#include <QXmlStreamWriter>

DescribeSqlField::DescribeSqlField()
{
  m_fieldLength = 0;
  m_isAutoIncrement = false;
  m_isKey = false;
  m_isRequired = false;
}

DescribeSqlField::DescribeSqlField(const DescribeSqlField& field)
{
  copy(field);
}

const DescribeSqlField& DescribeSqlField::copy(const DescribeSqlField& field)
{
  if (this != &field) {
    setName(field.getName());
    setViewName(field.getViewName());
    setDescription(field.getDescription());
    setFieldType(field.getFieldType());
    setPreferredTypeName(field.getPreferredTypeName());
    setLinkTableName(field.getLinkTableName());
    setLinkFieldName(field.getLinkFieldName());
    setIsAutoIncrement(field.isAutoIncrement());
    setIsRequired(field.isRequired());
    setIsKey(field.isKey());
    setFieldLength(field.getFieldLength());
  }
  return *this;
}

bool DescribeSqlField::isLinkField() const
{
  return !getLinkTableName().isEmpty() && !getLinkFieldName().isEmpty() ;
}

DescribeSqlField DescribeSqlField::readXml(QXmlStreamReader& reader)
{
    DescribeSqlField field;
    bool foundFieldTag = false;
    while (!reader.atEnd()) {
        if (reader.isStartElement()) {
            if (reader.name().compare("Field", Qt::CaseInsensitive)) {
                if (foundFieldTag) {
                    // Found a second Table tag.
                    break;
                }
                if (reader.attributes().hasAttribute("name"))
                    field.setName(reader.attributes().value("name").toString());
                if (reader.attributes().hasAttribute("viewname"))
                    field.setViewName(reader.attributes().value("viewname").toString());
                if (reader.attributes().hasAttribute("description"))
                    field.setDescription(reader.attributes().value("description").toString());
                // TODO: add the rest of the tags!
                reader.readNext();
            } else if (reader.name().compare("Link", Qt::CaseInsensitive)) {
                // TODO: Handle links
                reader.readNext();
            } else {
                // Unexpected element, what to do!
                qDebug(qPrintable(QString("Found unexpected XML element '%1' in field '%2'").arg(reader.name().toString()).arg(field.getName())));
                break;
            }
        } else if (reader.isStartDocument()) {
            reader.readNext();
        } else if (reader.isEndElement()) {
            if (foundFieldTag && reader.name().compare("Field", Qt::CaseInsensitive)) {
                reader.readNext();
                break;
            }
            reader.readNext();
        } else {
            reader.readNext();
        }
    }
    return field;
}

QXmlStreamWriter& DescribeSqlField::writeXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("Field");
  if (!getName().isEmpty())
    writer.writeAttribute("name", getName());
  if (!getViewName().isEmpty())
    writer.writeAttribute("viewname", getViewName());
  if (!getPreferredTypeName().isEmpty()) {
    writer.writeAttribute("type", getPreferredTypeName());
  } else if (getFieldType().isValid() && getFieldType().getSupportedNames().size() > 0) {
    writer.writeAttribute("type", getFieldType().getSupportedNames().at(0));
  }
  if (isAutoIncrement())
    writer.writeAttribute("autoincrement", "true");
  if (isRequired())
    writer.writeAttribute("required", "true");
  if (isKey())
    writer.writeAttribute("key", "true");
  if (getFieldLength() > 0)
      writer.writeAttribute("len", QString::number(getFieldLength()));
  if (getFieldPrecision() > 0)
      writer.writeAttribute("precision", QString::number(getFieldPrecision()));
  if (!getLinkTableName().isEmpty()) {
    writer.writeStartElement("Link");
    writer.writeAttribute("table", getLinkTableName());
    writer.writeAttribute("field", getLinkFieldName());
    writer.writeEndElement();
  }
  if (!getDescription().isEmpty()) {
    writer.writeAttribute("description", getDescription());
  }
  writer.writeEndElement();
  return writer;
}

