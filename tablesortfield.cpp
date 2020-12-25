#include "tablesortfield.h"
#include "typemapper.h"
#include "xmlutility.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>

TableSortField::TableSortField(QObject *parent) : QObject(parent),
    m_comparer(nullptr), m_fieldIndex(-1), m_sortOrder(Qt::AscendingOrder), m_MetaType(QMetaType::UnknownType)
{
    m_comparer = new ValueComparer(Qt::CaseInsensitive);
}

TableSortField::TableSortField(const TableSortField& obj) : TableSortField(obj, nullptr) {

}

TableSortField::TableSortField(const TableSortField& obj, QObject *parent) : QObject(parent)
{
    m_comparer = new ValueComparer(obj.caseSensitivity());
    operator=(obj);
}

TableSortField::TableSortField(const QString name, int index, Qt::SortOrder order, Qt::CaseSensitivity sensitive) : QObject(nullptr),
    m_comparer(nullptr), m_fieldIndex(index), m_sortOrder(order), m_fieldName(name)
{
    m_comparer = new ValueComparer(sensitive);
}

TableSortField::TableSortField(QObject *parent, const QString name, int index, Qt::SortOrder order, Qt::CaseSensitivity sensitive) : QObject(parent),
    m_comparer(nullptr), m_fieldIndex(index), m_sortOrder(order), m_fieldName(name)
{
    m_comparer = new ValueComparer(sensitive);
}

TableSortField::~TableSortField()
{
    if (m_comparer != nullptr)
    {
        delete m_comparer;
        // wasted cycles...
        m_comparer = nullptr;
    }
}

const TableSortField& TableSortField::operator=(const TableSortField& obj)
{
    if (this != &obj)
    {
        *m_comparer = *obj.m_comparer;
        setFieldIndex(obj.fieldIndex());
        setCase(obj.caseSensitivity());
        setSortOrder(obj.sortOrder());
        setFieldName(obj.fieldName());
        setFieldType(obj.fieldType());
    }
    return *this;
}

QString TableSortField::sortOrderToName(const Qt::SortOrder sortOrder)
{
    return sortOrder == Qt::AscendingOrder ? "Ascending" : "Descending";
}

Qt::SortOrder TableSortField::sortOrderFromName(const QString& name)
{
    return name.compare("Descending", Qt::CaseInsensitive) == 0 ? Qt::DescendingOrder : Qt::AscendingOrder;
}

QStringList TableSortField::sortOrderNames()
{
    QStringList list;
    list << "Ascending";
    list << "Descending";
    return list;
}

QXmlStreamWriter& TableSortField::write(QXmlStreamWriter& writer) const
{
  TypeMapper mapper;
  writer.writeStartElement("TableSortField");
  writer.writeAttribute("Name", m_fieldName);
  writer.writeAttribute("Index", QString("%1").arg(m_fieldIndex));
  writer.writeAttribute("CaseSensitive", m_comparer != nullptr && m_comparer->caseSensitivity() == Qt::CaseSensitive ? "True" : "False");
  writer.writeAttribute("Type", mapper.getMetaName(m_MetaType));
  writer.writeAttribute("Ascending", isAscending() ? "True" : "False");
  writer.writeEndElement();
  return writer;
}

QXmlStreamWriter& TableSortField::write(const QList<TableSortField>& list, QXmlStreamWriter& writer)
{
  writer.writeStartElement("TableSortFields");
  for (int i=0; i<list.size(); ++i)
  {
    list.at(i).write(writer);
  }
  writer.writeEndElement();
  return writer;
}

QXmlStreamReader& TableSortField::read(QList<TableSortField>& list, QXmlStreamReader& reader)
{
  TypeMapper mapper;
  QString name;
  while (!reader.atEnd() && !reader.isEndElement()) {
    if (reader.isStartElement())
    {
      name = reader.name().toString();
      if (QString::compare(name, "TableSortFields", Qt::CaseInsensitive) == 0) {
        reader.readNext();
        while (!reader.atEnd())
        {
          if (reader.isEndElement())
          {
            if (reader.name().toString().compare("TableSortField", Qt::CaseInsensitive) != 0)
            {
              return reader;
            }
          }
          else if (reader.isStartElement())
          {
            //qDebug(qPrintable(QString("start = %1").arg(reader.name().toString())));
            if (QString::compare(reader.name().toString(), "TableSortField", Qt::CaseInsensitive) != 0)
            {
              reader.raiseError(QObject::tr("Not sort field"));
            }
            else
            {
              // Read the fields.
              TableSortField field;
              QXmlStreamAttributes attr = reader.attributes();
              if (attr.hasAttribute("Name")) {
                field.setFieldName(attr.value("Name").toString());
              }
              if (attr.hasAttribute("Index")) {
                bool ok = false;
                int i = attr.value("Index").toInt(&ok, 10);
                if (ok) {
                  field.setFieldIndex(i);
                }
              }
              if (attr.hasAttribute("CaseSensitive")) {
                field.setCase(XMLUtility::stringToBoolean(attr.value("CaseSensitive").toString()) ? Qt::CaseSensitive : Qt::CaseInsensitive);
              }
              if (attr.hasAttribute("Type")) {
                field.setFieldType(mapper.getMetaType(attr.value("Type").toString()));
              }
              if (attr.hasAttribute("Ascending")) {
                //qDebug(qPrintable(QString("ascending = %1").arg(attr.value("Ascending").toString())));
                field.setSortOrder(XMLUtility::stringToBoolean(attr.value("Ascending").toString()) ? Qt::AscendingOrder : Qt::DescendingOrder);
              }
              list.append(field);
              //qDebug(qPrintable(QString("Read field %1").arg(field.fieldName())));
            }
          }
          reader.readNext();
        }
        return reader;
      } else {
        reader.raiseError(QObject::tr("Not sort fields"));
      }
    }
    reader.readNext();
  }
  return reader;
}
