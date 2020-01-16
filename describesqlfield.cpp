#include "describesqlfield.h"
#include "xmlutility.h"
#include "sqlfieldtypemaster.h"

#include <QDebug>
#include <QXmlStreamWriter>

DescribeSqlField::DescribeSqlField()
{
  m_fieldLength = 0;
  m_fieldPrecision = 0;
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
    setFieldPrecision(field.getFieldPrecision());
    setCurrencySymbol(field.getCurrencySymbol());
    setLinkDisplayField(field.m_linkDisplayField);
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
    bool ok;
    while (!reader.atEnd()) {
        if (reader.isStartElement()) {
            if (reader.name().compare(QLatin1String("Field"), Qt::CaseInsensitive) == 0) {
                if (foundFieldTag) {
                    // Found a second Table tag.
                    break;
                }
                foundFieldTag = true;
                if (reader.attributes().hasAttribute("name"))
                    field.setName(reader.attributes().value("name").toString());
                if (reader.attributes().hasAttribute("viewname"))
                    field.setViewName(reader.attributes().value("viewname").toString());
                if (reader.attributes().hasAttribute("description"))
                    field.setDescription(reader.attributes().value("description").toString());
                if (reader.attributes().hasAttribute("type")) {
                    QString sType = reader.attributes().value("type").toString();
                    SqlFieldTypeMaster typeMaster;
                    SqlFieldType fieldType = typeMaster.findByName(sType);
                    if (!fieldType.isValid()) {
                        qDebug() << qPrintable(QString("Invalid field type found '%1' in field '%2'").arg(sType).arg(field.getName()));
                    } else {
                        field.setFieldType(fieldType);
                        field.setPreferredTypeName(sType);
                    }
                }
                if (reader.attributes().hasAttribute("currency")) {
                    field.setCurrencySymbol(reader.attributes().value("autoincrement").toString());
                }
                if (reader.attributes().hasAttribute("autoincrement")) {
                    field.setIsAutoIncrement(XMLUtility::stringToBoolean(reader.attributes().value("autoincrement").toString()));
                }
                if (reader.attributes().hasAttribute("required")) {
                    field.setIsRequired(XMLUtility::stringToBoolean(reader.attributes().value("required").toString()));
                }
                if (reader.attributes().hasAttribute("key")) {
                    field.setIsKey(XMLUtility::stringToBoolean(reader.attributes().value("key").toString()));
                }
                if (reader.attributes().hasAttribute("precision")) {
                    field.setFieldPrecision(reader.attributes().value("precision").toInt(&ok));
                    if (!ok) {
                        qDebug() << qPrintable(QString("Failed to set field precision '%1' in field '%2'").arg(reader.attributes().value("precision").toString()).arg(field.getName()));
                    }
                }
                if (reader.attributes().hasAttribute("len")) {
                    field.setFieldLength(reader.attributes().value("len").toInt(&ok));
                    if (!ok) {
                        qDebug() << qPrintable(QString("Failed to set field length '%1' in field '%2'").arg(reader.attributes().value("len").toString()).arg(field.getName()));
                    }
                }
                reader.readNext();
            } else if (reader.name().compare(QLatin1String("Link"), Qt::CaseInsensitive) == 0) {
                if (reader.attributes().hasAttribute("table") && reader.attributes().hasAttribute("field")) {
                    field.setLinkTableName(reader.attributes().value("table").toString());
                    field.setLinkFieldName(reader.attributes().value("field").toString());
                    field.m_linkDisplayField = reader.attributes().value("displayfield").toString();
                } else {
                    qDebug() << qPrintable(QString("Link in field '%1' does not reference a table and field").arg(field.getName()));
                }
                reader.readNext();
            } else {
                // Unexpected element, so obviously we are finished with the Field!
                // qDebug(qPrintable(QString("Found unexpected XML element '%1' in field '%2'").arg(reader.name().toString()).arg(field.getName())));
                break;
            }
        } else if (reader.isStartDocument()) {
            reader.readNext();
        } else if (reader.isEndElement()) {
            //qDebug(qPrintable(QString("End element with name '%1' in field '%2'").arg(reader.name().toString()).arg(field.getName())));
            if (foundFieldTag && reader.name().compare(QLatin1String("Field"), Qt::CaseInsensitive) == 0) {
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

  if (isCurrency())
    writer.writeAttribute("currency", getCurrencySymbol());
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
  if (!getDescription().isEmpty()) {
    writer.writeAttribute("description", getDescription());
  }

  if (!getLinkTableName().isEmpty()) {
    writer.writeStartElement("Link");
    writer.writeAttribute("table", getLinkTableName());
    writer.writeAttribute("field", getLinkFieldName());
    writer.writeAttribute("displayfield", m_linkDisplayField);
    writer.writeEndElement();
  }
  writer.writeEndElement();
  return writer;
}

QString DescribeSqlField::getDDL() const
{
  QString ddl = QString("%1 %2").arg(m_name).arg(getFieldType().getFirstSupportedName());

  if (getFieldType().supportsLength() && getFieldLength() > 0) {
    //ddl = ddl.append("(").append(getFieldLength()).append(")");
    ddl = ddl.append(QString("(%1)").arg(getFieldLength()));
  }

  if (isKey()) {
    ddl = ddl.append(" PRIMARY KEY");
  }

  if (isAutoIncrement()) {
    ddl = ddl.append(" AUTOINCREMENT");
  }

  return ddl;
}

QStringList DescribeSqlField::getLinkDisplayField() const
{
  if (!m_linkDisplayField.contains(',')) {
    QStringList l;
    l << m_linkDisplayField;
    return l;
  }
  return m_linkDisplayField.split(',');
}
