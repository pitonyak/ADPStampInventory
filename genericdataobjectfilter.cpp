#include "genericdataobjectfilter.h"
#include "typemapper.h"
#include "genericdataobject.h"

#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>
#include <QMetaObject>
#include <QMetaEnum>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QTime>
#include <QDateTime>

GenericDataObjectFilter::GenericDataObjectFilter(QObject *parent) :
    QObject(parent), m_compareType(VariantComparer::Equal), m_caseSensitivity(Qt::CaseInsensitive), m_invertFilterResult(false), m_filterMeansAccept(true), m_multiValued(false), m_values(nullptr), m_expressions(nullptr)
{
  // After this, the lists are guaranteed to exist.
  clearLists(false, true);
}


GenericDataObjectFilter::GenericDataObjectFilter(const GenericDataObjectFilter& filter, QObject *parent) :
  QObject(parent), m_compareType(VariantComparer::Equal), m_caseSensitivity(Qt::CaseInsensitive), m_invertFilterResult(false), m_filterMeansAccept(true), m_multiValued(false), m_values(nullptr), m_expressions(nullptr)
{
  // After this, the lists are guaranteed to exist.
  operator=(filter);
}

GenericDataObjectFilter::GenericDataObjectFilter(const GenericDataObjectFilter& filter) :
  QObject(nullptr), m_compareType(VariantComparer::Equal), m_caseSensitivity(Qt::CaseInsensitive), m_invertFilterResult(false), m_filterMeansAccept(true), m_multiValued(false), m_values(nullptr), m_expressions(nullptr)
{
  // After this, the lists are guaranteed to exist.
  operator=(filter);
}

GenericDataObjectFilter::~GenericDataObjectFilter()
{
  clearLists(true, false);
}

void GenericDataObjectFilter::clearLists(bool deleteLists, bool createIfDoNotExist)
{
  if (m_values != nullptr)
  {
    m_values->clear();
    if (deleteLists)
    {
      delete m_values;
      m_values = nullptr;
    }
  }
  if (m_expressions != nullptr)
  {
    qDeleteAll(*m_expressions);
    m_expressions->clear();
    if (deleteLists)
    {
      delete m_expressions;
      m_expressions = nullptr;
    }
  }
  if (createIfDoNotExist)
  {
    if (m_values == nullptr)
    {
      m_values = new QList<QVariant>();
    }
    if (m_expressions == nullptr)
    {
      m_expressions = new QList<QRegularExpression*>();
    }
  }
}

GenericDataObjectFilter& GenericDataObjectFilter::operator=(const GenericDataObjectFilter& filter)
{
  if (this != & filter)
  {
    // set in this order so that multiple values will be cleared as needed.
    clearLists(false, true);
    m_multiValued = false;
    setCaseSensitivity(filter.getCaseSensitivity());
    setInvertFilterResult(filter.isInvertFilterResult());
    setFilterMeansAccept(filter.isFilterMeansAccept());
    setCompareField(filter.getCompareField());
    setCompareType(filter.getCompareType());
    setMultiValued(filter.isMultiValued());
    setValue(filter.getValue());
  }
  return *this;
}

GenericDataObjectFilter* GenericDataObjectFilter::clone(QObject *parent) const
{
  return new GenericDataObjectFilter(*this, parent);
}


void GenericDataObjectFilter::setCompareType(VariantComparer::CompareType compareType)
{
    if (compareType == VariantComparer::RegularExpression) {
        compareType = VariantComparer::RegExpFull;
    }
  if (compareType != m_compareType)
  {
    m_compareType = compareType;
  }
}

void GenericDataObjectFilter::setCompareField(const QString &compareField)
{
    //?? Handle this
  if (compareField != m_compareField)
  {
    m_compareField = compareField;
  }
}

void GenericDataObjectFilter::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
  if (caseSensitivity != m_caseSensitivity)
  {
    m_caseSensitivity = caseSensitivity;
    for (int i=0; i<m_expressions->size(); ++i)
    {
      if (m_caseSensitivity == Qt::CaseInsensitive) {
        m_expressions->at(i)->setPatternOptions(QRegularExpression::CaseInsensitiveOption);
      } else {
        QRegularExpression* reg = new QRegularExpression(m_expressions->at(i)->pattern());
        delete m_expressions->at(i);
        m_expressions->replace(i, reg);
      }
    }
  }
}

void GenericDataObjectFilter::setInvertFilterResult(bool invertFilterResult)
{
  if (invertFilterResult != m_invertFilterResult)
  {
    m_invertFilterResult = invertFilterResult;
  }
}

bool GenericDataObjectFilter::isMultiValued() const
{
  return m_multiValued;
}

void GenericDataObjectFilter::setMultiValuedDefault()
{
  setMultiValued(false);
}

void GenericDataObjectFilter::setMultiValued(bool multiValued)
{
  if (multiValued != m_multiValued)
  {
    m_multiValued = multiValued;
    clearLists(false, true);
  }
}

void GenericDataObjectFilter::setFilterMeansAccept(bool filterMeansAccept)
{
  if (filterMeansAccept != m_filterMeansAccept)
  {
    m_filterMeansAccept = filterMeansAccept;
  }
}

void GenericDataObjectFilter::setValue(const QVariant& value)
{
  if (value != m_value)
  {
    m_value = value;
    createLists();
  }
}

void GenericDataObjectFilter::createLists()
{
  clearLists(false, true);
  if (!isMultiValued() || m_value.metaType().id() != QMetaType::QString)
  {
    m_values->append(m_value);
  }
  else
  {
    QStringList list = m_value.toString().split(',', Qt::SkipEmptyParts);
    foreach (QString s, list)
    {
      m_values->append(QVariant(s));
    }
  }
  createRegularExpressions();
}

void GenericDataObjectFilter::createRegularExpressions()
{
  foreach (QVariant value, *m_values)
  {
    if (value.isValid() && !value.isNull())
    {
      //if (value.type() == QVariant::RegExp)
      //{
      //  m_expressions->append(new QRegularExpression(value.toRegExp()));
      //}
      //else
      if (m_compareType == VariantComparer::RegularExpression || m_compareType == VariantComparer::RegExpFull || m_compareType == VariantComparer::RegExpPartial)
      {
        QRegularExpression* reg = new QRegularExpression(value.toString());
        if (m_caseSensitivity == Qt::CaseInsensitive) {
          reg->setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        }
      }
      else if (m_compareType == VariantComparer::FileSpec)
      {
        m_expressions->append(new QRegularExpression(QRegularExpression::fromWildcard(value.toString(), m_caseSensitivity)));
      }
      else
      {
        // Ignore because the type is neither a regular expression, nor a file spec.
      }
    }
  }
}


void GenericDataObjectFilter::setValueDefault(const QMetaType::Type aType)
{
    setValue(TypeMapper::getDefaultValue(aType));
}

bool GenericDataObjectFilter::objectMatchesFilter(const GenericDataObject& obj) const
{
  // Perhaps I should match against an empty variant object if this does not match.
  return obj.containsValue(m_compareField) ? variantMatchesFilter(obj.getValue(m_compareField)) : false;
}


bool GenericDataObjectFilter::variantMatchesFilter(const QVariant& obj) const
{
  if (m_expressions != nullptr && (m_compareType == VariantComparer::RegularExpression || m_compareType == VariantComparer::FileSpec || m_compareType == VariantComparer::RegExpPartial))
  {
    foreach (QRegularExpression* expression, *m_expressions)
    {
      if (VariantComparer::matches(obj, obj, m_compareType, m_caseSensitivity, expression))
      {
        return !m_invertFilterResult;
      }
    }
  }
  else if (m_values != nullptr)
  {
    foreach (QVariant value, *m_values)
    {
      if (VariantComparer::matches(obj, value, m_compareType, m_caseSensitivity, nullptr))
      {
        return !m_invertFilterResult;
      }
    }
  }
  return m_invertFilterResult;
}

QString GenericDataObjectFilter::getCompareTypeAsString() const
{
  const QMetaObject* metaObj = metaObject();
  return  metaObj->enumerator(metaObj->indexOfEnumerator("CompareType")).valueToKey(getCompareType());
}

QString GenericDataObjectFilter::getFieldTypeAsString() const
{
  TypeMapper mapper;
  return mapper.getMetaName(m_fieldType);
}

QXmlStreamWriter& GenericDataObjectFilter::operator<<(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("Filter");
  writer.writeTextElement("CompareType", getCompareTypeAsString());
  writer.writeTextElement("FieldType", getFieldTypeAsString());
  writer.writeTextElement("CompareField", getCompareField());
  writer.writeTextElement("Value", m_value.toString());
  writer.writeTextElement("CaseSensitive", (m_caseSensitivity==Qt::CaseSensitive) ? "True" : "False");
  writer.writeTextElement("Invert", m_invertFilterResult ? "True" : "False");
  writer.writeTextElement("Accept", m_filterMeansAccept ? "True" : "False");
  writer.writeTextElement("MultiValued", m_multiValued ? "True" : "False");
  writer.writeEndElement();
  return writer;
}

QXmlStreamReader& GenericDataObjectFilter::operator>>(QXmlStreamReader& reader)
{
  return readFilter(reader);
}

QXmlStreamReader&  GenericDataObjectFilter::readFilter(QXmlStreamReader& reader)
{
  QString version = "1";
  QString name;
  while (!reader.atEnd()) {
    if (reader.isStartElement()) {
      name = reader.name().toString();
      if (QString::compare(name, "Filter", Qt::CaseInsensitive) == 0) {
        if (reader.attributes().hasAttribute("Version"))
        {
          version = reader.attributes().value("Version").toString();
        }
        readInternals(reader, version);
        return reader;
      } else {
        reader.raiseError(QObject::tr("Not a Filter"));
      }
    } else if (reader.isEndElement()) {
      return reader;
    }
    reader.readNext();
  }
  return reader;
}

void GenericDataObjectFilter::readInternals(QXmlStreamReader& reader, const QString&)
{
  const QMetaObject* metaObj = metaObject();
  QString name;
  while (!reader.atEnd()) {
    if (reader.isStartElement()) {
      name = reader.name().toString();
    } else if (reader.isCharacters()) {
      QString value = reader.text().toString();
      if (name.isEmpty()) {
        // Ignore an empty name
      } else if (QString::compare(name, "CompareType", Qt::CaseInsensitive) == 0) {
        if (value.isEmpty()) {
          setCompareType(VariantComparer::Equal);
        } else {
          setCompareType(static_cast<VariantComparer::CompareType>(metaObj->enumerator(metaObj->indexOfEnumerator("CompareType")).keyToValue(qPrintable(value))));
        }
      }  else if (QString::compare(name, "FieldType", Qt::CaseInsensitive) == 0) {
        if (value.isEmpty()) {
          setFieldType(QMetaType::QString);
        } else {
          TypeMapper mapper;
          setFieldType(mapper.getMetaType(value));
        }
      } else if (QString::compare(name, "CompareField", Qt::CaseInsensitive) == 0) {
        if (value.isEmpty()) {
          setCompareField("");
        } else {
          setCompareField(value);
        }
      } else if (QString::compare(name, "Value", Qt::CaseInsensitive) == 0) {
        setValue(value);
      } else if (QString::compare(name, "CaseSensitive", Qt::CaseInsensitive) == 0) {
        if (value.isEmpty()) {
          setCaseSensitivityDefault();
        } else {
          setCaseSensitivity(QString::compare(value, "True") == 0 ? Qt::CaseSensitive : Qt::CaseInsensitive);
        }
      } else if (QString::compare(name, "Invert", Qt::CaseInsensitive) == 0) {
        if (value.isEmpty()) {
          setInvertFilterResultDefault();
        } else {
          setInvertFilterResult(QString::compare(value, "True") == 0);
        }
      } else if (QString::compare(name, "MultiValued", Qt::CaseInsensitive) == 0) {
        if (value.isEmpty()) {
          setMultiValuedDefault();
        } else {
          setMultiValued(QString::compare(value, "True") == 0);
        }
      } else if (QString::compare(name, "Accept", Qt::CaseInsensitive) == 0) {
        if (value.isEmpty()) {
          setFilterMeansAcceptDefault();
        } else {
          setFilterMeansAccept(QString::compare(value, "True") == 0);
        }
      } else if (QString::compare(name, "Filter", Qt::CaseInsensitive) == 0) {
      } else {
        reader.raiseError(QObject::tr("Unrecognized Filter Element %1").arg(name));
      }
    } else if (reader.isEndElement()) {
      if (QString::compare(reader.name().toString(), "Filter", Qt::CaseInsensitive) == 0)
      {
        return;
      }
      name = "";
    }
    reader.readNext();
  }
}
