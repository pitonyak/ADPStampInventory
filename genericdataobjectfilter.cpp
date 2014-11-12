#include "genericdataobjectfilter.h"
#include "typemapper.h"

#include <QFileInfo>
#include <QDir>
#include <QRegExp>
#include <QRegExp>
#include <QMetaObject>
#include <QMetaEnum>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QTime>
#include <QDateTime>

GenericDataObjectFilter::GenericDataObjectFilter(QObject *parent) :
    QObject(parent), m_compareType(GenericDataObjectFilter::Equal), m_caseSensitivity(Qt::CaseInsensitive), m_invertFilterResult(false), m_filterMeansAccept(true), m_multiValued(false), m_values(nullptr), m_expressions(nullptr)
{
  // After this, the lists are guaranteed to exist.
  clearLists(false, true);
}


GenericDataObjectFilter::GenericDataObjectFilter(const GenericDataObjectFilter& filter, QObject *parent) :
  QObject(parent), m_compareType(GenericDataObjectFilter::Equal), m_caseSensitivity(Qt::CaseInsensitive), m_invertFilterResult(false), m_filterMeansAccept(true), m_multiValued(false), m_values(nullptr), m_expressions(nullptr)
{
  // After this, the lists are guaranteed to exist.
  operator=(filter);
}

GenericDataObjectFilter::GenericDataObjectFilter(const GenericDataObjectFilter& filter) :
  QObject(nullptr), m_compareType(GenericDataObjectFilter::Equal), m_caseSensitivity(Qt::CaseInsensitive), m_invertFilterResult(false), m_filterMeansAccept(true), m_multiValued(false), m_values(nullptr), m_expressions(nullptr)
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
      m_expressions = new QList<QRegExp*>();
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


void GenericDataObjectFilter::setCompareType(CompareType compareType)
{
    if (compareType == GenericDataObjectFilter::RegularExpression) {
        compareType = GenericDataObjectFilter::RegExpFull;
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
      m_expressions->at(i)->setCaseSensitivity(m_caseSensitivity);
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
  if (!isMultiValued() || m_value.type() != QVariant::String)
  {
    m_values->append(m_value);
  }
  else
  {
    QStringList list = m_value.toString().split(',', QString::SkipEmptyParts);
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
      if (value.type() == QVariant::RegExp)
      {
        m_expressions->append(new QRegExp(value.toRegExp()));
      }
      else if (m_compareType == GenericDataObjectFilter::RegularExpression || m_compareType == GenericDataObjectFilter::RegExpFull || m_compareType == GenericDataObjectFilter::RegExpPartial)
      {
        m_expressions->append(new QRegExp(value.toString(), m_caseSensitivity, QRegExp::RegExp2));
      }
      else if (m_compareType == GenericDataObjectFilter::FileSpec)
      {
        m_expressions->append(new QRegExp(value.toString(), m_caseSensitivity, QRegExp::WildcardUnix));
      }
      else
      {
        // Ignore because the type is neither a regular expression, nor a file spec.
      }
    }
  }
}


void GenericDataObjectFilter::setValueDefault(const QVariant::Type aType)
{
    // TODO: Consider this
    setValue(TypeMapper::getDefaultValue((const QMetaType::Type)aType));
}

void GenericDataObjectFilter::setValueDefault(const QMetaType::Type aType)
{
    setValue(TypeMapper::getDefaultValue(aType));
}


/**
bool GenericDataObjectFilter::passes(const QFileInfo& fileInfo) const
{
  if (!applicable(fileInfo))
  {
    TRACE_MSG(QString("Filter (%1) not applicable").arg(getMainValueAsString()), 5);
    return false;
  }

  bool filterPass = false;

  switch (m_compareField)
  {
  case GenericDataObjectFilter::Date:
    filterPass = compareValues(fileInfo.lastModified().date());
    break;
  case GenericDataObjectFilter::DateTime:
    filterPass = compareValues(fileInfo.lastModified());
    break;
  case GenericDataObjectFilter::Name:
    filterPass = compareValues(fileInfo.fileName());
    break;
  case GenericDataObjectFilter::FullPath:
    filterPass = compareValues(fileInfo.canonicalFilePath());
    break;
  case GenericDataObjectFilter::PathOnly:
    filterPass = compareValues(fileInfo.canonicalPath());
    break;
  case GenericDataObjectFilter::Size:
    filterPass = compareValues(fileInfo.size());
    break;
  case GenericDataObjectFilter::Time:
    filterPass = compareValues(fileInfo.lastModified().time());
    break;
  default:
    // An invalid compare will never pass, even for an inverted filter.
    qDebug("Invalid compare field");
    return false;
    break;
  }

  return isInvertFilterResult() ? !filterPass : filterPass;
}
**/
bool GenericDataObjectFilter::compareValues(const qlonglong aSize) const
{
  switch (m_compareType)
  {
  case GenericDataObjectFilter::RegExpFull:
  case GenericDataObjectFilter::RegularExpression:
  case GenericDataObjectFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(QString::number(aSize)))
      {
        return true;
      }
    }
    break;
  case GenericDataObjectFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(QString::number(aSize)) >= 0))
      {
        return true;
      }
    }
    break;
  default:
    foreach (QVariant v, *m_values)
    {
      switch (m_compareType)
      {
      case GenericDataObjectFilter::Less:
        if (aSize < v.toLongLong())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::LessEqual:
        if (aSize <= v.toLongLong())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Equal:
        if (aSize == v.toLongLong())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::GreaterEqual:
        if (aSize >= v.toLongLong())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Greater:
        if (aSize > v.toLongLong())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::NotEqual:
        if (aSize != v.toLongLong())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Contains:
        if (QString::number(aSize).contains(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      default:
        break;
      }
    }
  }

  return false;
}


bool GenericDataObjectFilter::compareValues(const QTime& aTime) const
{
  switch (m_compareType)
  {
  case GenericDataObjectFilter::RegExpFull:
  case GenericDataObjectFilter::RegularExpression:
  case GenericDataObjectFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(aTime.toString(Qt::TextDate)))
      {
        return true;
      }
    }
    break;
  case GenericDataObjectFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(aTime.toString(Qt::TextDate)) >= 0))
      {
        return true;
      }
    }
    break;
  default:
    foreach (QVariant v, *m_values)
    {
      switch (m_compareType)
      {
      case GenericDataObjectFilter::Less:
        if (aTime < v.toTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::LessEqual:
        if (aTime <= v.toTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Equal:
        if (aTime == v.toTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::GreaterEqual:
        if (aTime >= v.toTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Greater:
        if (aTime > v.toTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::NotEqual:
        if (aTime != v.toTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Contains:
        if (aTime.toString(Qt::TextDate).contains(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      default:
        break;
      }
    }
  }
  return false;
}

bool GenericDataObjectFilter::compareValues(const QDate& aDate) const
{
  switch (m_compareType)
  {
  case GenericDataObjectFilter::RegExpFull:
  case GenericDataObjectFilter::RegularExpression:
  case GenericDataObjectFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(aDate.toString(Qt::TextDate)))
      {
        return true;
      }
    }
    break;
  case GenericDataObjectFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(aDate.toString(Qt::TextDate)) >= 0))
      {
        return true;
      }
    }
    break;
  default:
    foreach (QVariant v, *m_values)
    {
      switch (m_compareType)
      {
      case GenericDataObjectFilter::Less:
        if (aDate < v.toDate())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::LessEqual:
        if (aDate <= v.toDate())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Equal:
        if (aDate == v.toDate())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::GreaterEqual:
        if (aDate >= v.toDate())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Greater:
        if (aDate > v.toDate())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::NotEqual:
        if (aDate != v.toDate())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Contains:
        if (aDate.toString(Qt::TextDate).contains(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      default:
        break;
      }
    }
  }
  return false;
}

bool GenericDataObjectFilter::compareValues(const QDateTime& aDateTime) const
{
  switch (m_compareType)
  {
  case GenericDataObjectFilter::RegExpFull:
  case GenericDataObjectFilter::RegularExpression:
  case GenericDataObjectFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(aDateTime.toString(Qt::TextDate)))
      {
        return true;
      }
    }
    break;
  case GenericDataObjectFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(aDateTime.toString(Qt::TextDate)) >= 0))
      {
        return true;
      }
    }
    break;
  default:
    foreach (QVariant v, *m_values)
    {
      switch (m_compareType)
      {
      case GenericDataObjectFilter::Less:
        if (aDateTime < v.toDateTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::LessEqual:
        if (aDateTime <= v.toDateTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Equal:
        if (aDateTime == v.toDateTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::GreaterEqual:
        if (aDateTime >= v.toDateTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Greater:
        if (aDateTime > v.toDateTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::NotEqual:
        if (aDateTime != v.toDateTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Contains:
        if (aDateTime.toString(Qt::TextDate).contains(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      default:
        break;
      }
    }
  }
  return false;
}

bool GenericDataObjectFilter::compareValues(const QString& filePortion) const
{
  int i;
  switch (m_compareType)
  {
  case GenericDataObjectFilter::RegExpFull:
  case GenericDataObjectFilter::RegularExpression:
  case GenericDataObjectFilter::FileSpec:
    for (i=0; i<m_expressions->size(); ++i)
    {
      QRegExp* expression = m_expressions->at(i);
      if ((expression != nullptr) && expression->exactMatch(filePortion))
      {
        return true;
      }
      else
      {
      }
    }
    break;
  case GenericDataObjectFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(filePortion) >= 0))
      {
        return true;
      }
    }
    break;
  default:
    foreach (QVariant v, *m_values)
    {
      switch (m_compareType)
      {
      case GenericDataObjectFilter::Less:
        if (QString::compare(filePortion, v.toString(), m_caseSensitivity) < 0)
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::LessEqual:
        if (QString::compare(filePortion, v.toString(), m_caseSensitivity) <= 0)
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Equal:
        if (QString::compare(filePortion, v.toString(), m_caseSensitivity) == 0)
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::GreaterEqual:
        if (QString::compare(filePortion, v.toString(), m_caseSensitivity) >= 0)
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Greater:
        if (QString::compare(filePortion, v.toString(), m_caseSensitivity) > 0)
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::NotEqual:
        if (QString::compare(filePortion, v.toString(), m_caseSensitivity) != 0)
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Contains:
        if (filePortion.contains(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      default:
        break;
      }
    }
  }
  return false;
}

QString GenericDataObjectFilter::getCompareTypeAsString() const
{
  const QMetaObject* metaObj = metaObject();
  return  metaObj->enumerator(metaObj->indexOfEnumerator("CompareType")).valueToKey(getCompareType());
}

QString GenericDataObjectFilter::getCompareFieldAsString() const
{
    return m_compareField;
}

QXmlStreamWriter& GenericDataObjectFilter::operator<<(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("Filter");
  writer.writeTextElement("CompareType", getCompareTypeAsString());
  writer.writeTextElement("CompareField", getCompareFieldAsString());
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
  // setAllDefault();
  // TODO: Clear the object first.
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
          setCompareTypeDefault();
        } else {
          //TRACE_MSG(QString("Setting CompareType (%1) for filter").arg(value), 1);
          // ??? indexIn >= 0 is a partial match.
          setCompareType(static_cast<GenericDataObjectFilter::CompareType>(metaObj->enumerator(metaObj->indexOfEnumerator("CompareType")).keyToValue(qPrintable(value))));
        }
      } else if (QString::compare(name, "CompareField", Qt::CaseInsensitive) == 0) {
        if (value.isEmpty()) {
          setCompareField("");
        } else {
          //TRACE_MSG(QString("Setting CompareField (%1) for filter").arg(value), 1);
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
