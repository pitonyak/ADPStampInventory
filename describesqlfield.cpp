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

