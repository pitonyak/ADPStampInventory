#include "valuefilter.h"

//#include "linkbackupglobals.h"

#include <QFileInfo>
#include <QDir>
#include <QRegExp>
#include <QRegExp>
#include <QMetaObject>
#include <QMetaEnum>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QUuid>
#include <QUrl>

//#include "stringhelper.h"

ValueFilter::ValueFilter(QObject *parent) :
  QObject(parent), m_compareType(ValueFilter::Equal), m_compareField(ValueFilter::FullPath), m_caseSensitivity(Qt::CaseInsensitive), m_invertFilterResult(false), m_filterMeansAccept(true), m_filterFiles(true), m_filterDirs(true), m_multiValued(false), m_values(nullptr), m_expressions(nullptr)
{
  // After this, the lists are guaranteed to exist.
  clearLists(false, true);
}


ValueFilter::ValueFilter(const ValueFilter& filter, QObject *parent) :
  QObject(parent), m_compareType(ValueFilter::Equal), m_compareField(ValueFilter::FullPath), m_caseSensitivity(Qt::CaseInsensitive), m_invertFilterResult(false), m_filterMeansAccept(true), m_filterFiles(true), m_filterDirs(true), m_multiValued(false), m_values(nullptr), m_expressions(nullptr)
{
  // After this, the lists are guaranteed to exist.
  operator=(filter);
}

ValueFilter::ValueFilter(const ValueFilter& filter) :
  QObject(nullptr), m_compareType(ValueFilter::Equal), m_compareField(ValueFilter::FullPath), m_caseSensitivity(Qt::CaseInsensitive), m_invertFilterResult(false), m_filterMeansAccept(true), m_filterFiles(true), m_filterDirs(true), m_multiValued(false), m_values(nullptr), m_expressions(nullptr)
{
  // After this, the lists are guaranteed to exist.
  operator=(filter);
}


ValueFilter::~ValueFilter()
{
  clearLists(true, false);
}

void ValueFilter::clearLists(bool deleteLists, bool createIfDoNotExist)
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

ValueFilter& ValueFilter::operator=(const ValueFilter& filter)
{
  if (this != & filter)
  {
    // set in this order so that multiple values will be cleared as needed.
    clearLists(false, true);
    m_multiValued = false;
    setValueDefault();
    setCaseSensitivity(filter.getCaseSensitivity());
    setInvertFilterResult(filter.isInvertFilterResult());
    setFilterMeansAccept(filter.isFilterMeansAccept());
    setCompareField(filter.getCompareField());
    setCompareType(filter.getCompareType());
    setFilterFiles(filter.isFilterFiles());
    setFilterDirs(filter.isFilterDirs());
    setMultiValued(filter.isMultiValued());
    setValue(filter.getValue());
  }
  return *this;
}

ValueFilter* ValueFilter::clone(QObject *parent) const
{
  return new ValueFilter(*this, parent);
}


void ValueFilter::setCompareType(CompareType compareType)
{
    if (compareType == ValueFilter::RegularExpression) {
        compareType = ValueFilter::RegExpFull;
    }
  if (compareType != m_compareType)
  {
    m_compareType = compareType;
    emit compareTypeChanged(compareType);
  }
}

void ValueFilter::setCompareField(CompareField compareField)
{
  if (compareField != m_compareField)
  {
    m_compareField = compareField;
    emit compareFieldChanged(compareField);
  }
}

void ValueFilter::setCaseSensitivity(Qt::CaseSensitivity caseSensitivity)
{
  if (caseSensitivity != m_caseSensitivity)
  {
    m_caseSensitivity = caseSensitivity;
    for (int i=0; i<m_expressions->size(); ++i)
    {
      m_expressions->at(i)->setCaseSensitivity(m_caseSensitivity);
    }
    emit caseSensitivityChanged(caseSensitivity);
  }
}

void ValueFilter::setInvertFilterResult(bool invertFilterResult)
{
  if (invertFilterResult != m_invertFilterResult)
  {
    m_invertFilterResult = invertFilterResult;
    emit invertFilterResultChanged(invertFilterResult);
  }
}

bool ValueFilter::isMultiValued() const
{
  return m_multiValued;
}

void ValueFilter::setMultiValuedDefault()
{
  setMultiValued(false);
}

void ValueFilter::setMultiValued(bool multiValued)
{
  if (multiValued != m_multiValued)
  {
    m_multiValued = multiValued;
    clearLists(false, true);
    emit multiValuedChanged(multiValued);
  }
}

void ValueFilter::setFilterMeansAccept(bool filterMeansAccept)
{
  if (filterMeansAccept != m_filterMeansAccept)
  {
    m_filterMeansAccept = filterMeansAccept;
    emit filterMeansAcceptChanged(filterMeansAccept);
  }
}

bool ValueFilter::isFilterDirs() const
{
  return m_filterDirs;
}

void ValueFilter::setFilterDirs(bool filterDirs)
{
  if (m_filterDirs != filterDirs)
  {
    m_filterDirs = filterDirs;
    emit filterDirsChanged(m_filterDirs);
  }
}

void ValueFilter::setFilterDirsDefault()
{
  setFilterDirs();
}


bool ValueFilter::isFilterFiles() const
{
  return m_filterFiles;
}

void ValueFilter::setFilterFiles(bool filterFiles)
{
  if (m_filterFiles != filterFiles)
  {
    m_filterFiles = filterFiles;
    emit filterFilesChanged(m_filterFiles);
  }
}

void ValueFilter::setFilterFilesDefault()
{
  setFilterFiles();
}

void ValueFilter::setValue(const QVariant& value)
{
  if (value != m_value)
  {
    m_value = value;
    createLists();
    emit valueChanged(m_value);
  }
}

void ValueFilter::createLists()
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

void ValueFilter::createRegularExpressions()
{
  foreach (QVariant value, *m_values)
  {
    if (value.isValid() && !value.isNull())
    {
      if (value.type() == QVariant::RegExp)
      {
        m_expressions->append(new QRegExp(value.toRegExp()));
      }
      else if (m_compareType == ValueFilter::RegularExpression || m_compareType == ValueFilter::RegExpFull || m_compareType == ValueFilter::RegExpPartial)
      {
        m_expressions->append(new QRegExp(value.toString(), m_caseSensitivity, QRegExp::RegExp2));
      }
      else if (m_compareType == ValueFilter::FileSpec)
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

void ValueFilter::setValueDefault()
{
  switch (m_compareField)
  {
  case ValueFilter::Date:
    setValue(QDate::currentDate());
    break;
  case ValueFilter::DateTime:
    setValue(QDateTime::currentDateTime());
    break;
  case ValueFilter::Name:
  case ValueFilter::FullPath:
  case ValueFilter::PathOnly:
    setValue(QString());
    break;
  case ValueFilter::Size:
    setValue(0);
    break;
  case ValueFilter::Time:
    setValue(QTime::currentTime());
    break;
  default:
    setValue(QString());
    break;
  }
}

bool ValueFilter::applicable(const QFileInfo& fileInfo) const
{
  if (isFilterFiles() && fileInfo.isFile())
  {
    return true;
  }
  if (isFilterDirs() && fileInfo.isDir())
  {
    return true;
  }
  return false;
}

bool ValueFilter::passes(const QStringList& value) const
{
    QStringListIterator i(value);
    while (i.hasNext())
    {
        if (compareValues(i.next()))
        {
            return invertIfNeeded(true);
        }
    }
    return invertIfNeeded(false);
}

bool ValueFilter::passes(const QVariant& value) const
{
    switch (value.type())
    {
    case QVariant::Bool :
        return passes(value.toBool());
        break;
    case QVariant::Char :
        return passes(value.toChar());
        break;
    case QVariant::Date :
        return passes(value.toDate());
        break;
    case QVariant::DateTime :
        return passes(value.toDateTime());
        break;
    case QVariant::Time :
        return passes(value.toTime());
        break;
    case QVariant::Double :
        return passes(value.toDouble());
        break;
    case QVariant::Uuid :
        return passes(value.toUuid());
        break;
    case QVariant::Int :
        return passes(value.toInt());
        break;
    case QVariant::String :
        return passes(value.toString());
        break;
    case QVariant::UInt :
        //return passes(value.toUInt());
        break;
    case QVariant::ULongLong :
        return passes(value.toULongLong());
        break;
    case QVariant::Url :
        return passes(value.toUrl());
        break;
    case QVariant::StringList :
        return passes(value.toStringList());
        break;
    default:
        return false;
        break;
    }
    return false;
}


bool ValueFilter::passes(const QFileInfo& fileInfo) const
{
  if (!applicable(fileInfo))
  {
    //TRACE_MSG(QString("Filter (%1) not applicable").arg(getMainValueAsString()), 5);
    return false;
  }

  bool filterPass = false;

  switch (m_compareField)
  {
  case ValueFilter::Date:
    filterPass = compareValues(fileInfo.lastModified().date());
    break;
  case ValueFilter::DateTime:
    filterPass = compareValues(fileInfo.lastModified());
    break;
  case ValueFilter::Name:
    filterPass = compareValues(fileInfo.fileName());
    break;
  case ValueFilter::FullPath:
    filterPass = compareValues(fileInfo.canonicalFilePath());
    break;
  case ValueFilter::PathOnly:
    filterPass = compareValues(fileInfo.canonicalPath());
    break;
  case ValueFilter::Size:
    filterPass = compareValues(fileInfo.size());
    break;
  case ValueFilter::Time:
    filterPass = compareValues(fileInfo.lastModified().time());
    break;
  default:
    // An invalid compare will never pass, even for an inverted filter.
    qDebug("Invalid compare field");
    return false;
    break;
  }

  return invertIfNeeded(filterPass);
}

bool ValueFilter::compareValues(const double value) const
{
  switch (m_compareType)
  {
  case ValueFilter::RegExpFull:
  case ValueFilter::RegularExpression:
  case ValueFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(QString::number(value)))
      {
        return true;
      }
    }
    break;
  case ValueFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(QString::number(value)) >= 0))
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
      case ValueFilter::Less:
        if (value < v.toDouble())
        {
          return true;
        }
        break;
      case ValueFilter::LessEqual:
        if (value <= v.toDouble())
        {
          return true;
        }
        break;
      case ValueFilter::Equal:
        if (value == v.toDouble())
        {
          return true;
        }
        break;
      case ValueFilter::GreaterEqual:
        if (value >= v.toDouble())
        {
          return true;
        }
        break;
      case ValueFilter::Greater:
        if (value > v.toDouble())
        {
          return true;
        }
        break;
      case ValueFilter::NotEqual:
        if (value != v.toDouble())
        {
          return true;
        }
        break;
      case ValueFilter::Contains:
        if (QString::number(value).contains(v.toString(), m_caseSensitivity))
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


bool ValueFilter::compareValues(const qlonglong value) const
{
  switch (m_compareType)
  {
  case ValueFilter::RegExpFull:
  case ValueFilter::RegularExpression:
  case ValueFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(QString::number(value)))
      {
        return true;
      }
    }
    break;
  case ValueFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(QString::number(value)) >= 0))
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
      case ValueFilter::Less:
        if (value < v.toLongLong())
        {
          return true;
        }
        break;
      case ValueFilter::LessEqual:
        if (value <= v.toLongLong())
        {
          return true;
        }
        break;
      case ValueFilter::Equal:
        if (value == v.toLongLong())
        {
          return true;
        }
        break;
      case ValueFilter::GreaterEqual:
        if (value >= v.toLongLong())
        {
          return true;
        }
        break;
      case ValueFilter::Greater:
        if (value > v.toLongLong())
        {
          return true;
        }
        break;
      case ValueFilter::NotEqual:
        if (value != v.toLongLong())
        {
          return true;
        }
        break;
      case ValueFilter::Contains:
        if (QString::number(value).contains(v.toString(), m_caseSensitivity))
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

bool ValueFilter::compareValues(const qulonglong value) const
{
  switch (m_compareType)
  {
  case ValueFilter::RegExpFull:
  case ValueFilter::RegularExpression:
  case ValueFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(QString::number(value)))
      {
        return true;
      }
    }
    break;
  case ValueFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(QString::number(value)) >= 0))
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
      case ValueFilter::Less:
        if (value < v.toULongLong())
        {
          return true;
        }
        break;
      case ValueFilter::LessEqual:
        if (value <= v.toULongLong())
        {
          return true;
        }
        break;
      case ValueFilter::Equal:
        if (value == v.toULongLong())
        {
          return true;
        }
        break;
      case ValueFilter::GreaterEqual:
        if (value >= v.toULongLong())
        {
          return true;
        }
        break;
      case ValueFilter::Greater:
        if (value > v.toULongLong())
        {
          return true;
        }
        break;
      case ValueFilter::NotEqual:
        if (value != v.toULongLong())
        {
          return true;
        }
        break;
      case ValueFilter::Contains:
        if (QString::number(value).contains(v.toString(), m_caseSensitivity))
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



bool ValueFilter::compareValues(const QTime& aTime) const
{
  switch (m_compareType)
  {
  case ValueFilter::RegExpFull:
  case ValueFilter::RegularExpression:
  case ValueFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(aTime.toString(Qt::TextDate)))
      {
        return true;
      }
    }
    break;
  case ValueFilter::RegExpPartial:
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
      case ValueFilter::Less:
        if (aTime < v.toTime())
        {
          return true;
        }
        break;
      case ValueFilter::LessEqual:
        if (aTime <= v.toTime())
        {
          return true;
        }
        break;
      case ValueFilter::Equal:
        if (aTime == v.toTime())
        {
          return true;
        }
        break;
      case ValueFilter::GreaterEqual:
        if (aTime >= v.toTime())
        {
          return true;
        }
        break;
      case ValueFilter::Greater:
        if (aTime > v.toTime())
        {
          return true;
        }
        break;
      case ValueFilter::NotEqual:
        if (aTime != v.toTime())
        {
          return true;
        }
        break;
      case ValueFilter::Contains:
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

bool ValueFilter::compareValues(const QDate& aDate) const
{
  switch (m_compareType)
  {
  case ValueFilter::RegExpFull:
  case ValueFilter::RegularExpression:
  case ValueFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(aDate.toString(Qt::TextDate)))
      {
        return true;
      }
    }
    break;
  case ValueFilter::RegExpPartial:
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
      case ValueFilter::Less:
        if (aDate < v.toDate())
        {
          return true;
        }
        break;
      case ValueFilter::LessEqual:
        if (aDate <= v.toDate())
        {
          return true;
        }
        break;
      case ValueFilter::Equal:
        if (aDate == v.toDate())
        {
          return true;
        }
        break;
      case ValueFilter::GreaterEqual:
        if (aDate >= v.toDate())
        {
          return true;
        }
        break;
      case ValueFilter::Greater:
        if (aDate > v.toDate())
        {
          return true;
        }
        break;
      case ValueFilter::NotEqual:
        if (aDate != v.toDate())
        {
          return true;
        }
        break;
      case ValueFilter::Contains:
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

bool ValueFilter::compareValues(const QDateTime& aDateTime) const
{
  switch (m_compareType)
  {
  case ValueFilter::RegExpFull:
  case ValueFilter::RegularExpression:
  case ValueFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(aDateTime.toString(Qt::TextDate)))
      {
        return true;
      }
    }
    break;
  case ValueFilter::RegExpPartial:
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
      case ValueFilter::Less:
        if (aDateTime < v.toDateTime())
        {
          return true;
        }
        break;
      case ValueFilter::LessEqual:
        if (aDateTime <= v.toDateTime())
        {
          return true;
        }
        break;
      case ValueFilter::Equal:
        if (aDateTime == v.toDateTime())
        {
          return true;
        }
        break;
      case ValueFilter::GreaterEqual:
        if (aDateTime >= v.toDateTime())
        {
          return true;
        }
        break;
      case ValueFilter::Greater:
        if (aDateTime > v.toDateTime())
        {
          return true;
        }
        break;
      case ValueFilter::NotEqual:
        if (aDateTime != v.toDateTime())
        {
          return true;
        }
        break;
      case ValueFilter::Contains:
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

bool ValueFilter::compareValues(const QString& value) const
{
  int i;
  switch (m_compareType)
  {
  case ValueFilter::RegExpFull:
  case ValueFilter::RegularExpression:
  case ValueFilter::FileSpec:
    for (i=0; i<m_expressions->size(); ++i)
    {
      QRegExp* expression = m_expressions->at(i);
      //TRACE_MSG(QString("Checking fileportion (%1) against (%2)").arg(value, m_values->at(i).toString()), 6);
      if ((expression != nullptr) && expression->exactMatch(value))
      {
        //TRACE_MSG(QString("PASSED fileportion (%1) against (%2)").arg(value, m_values->at(i).toString()), 5);
        return true;
      }
      else
      {
        //TRACE_MSG(QString("FAILED fileportion (%1) against (%2)").arg(value, m_values->at(i).toString()), 5);
      }
    }
    break;
  case ValueFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(value) >= 0))
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
      case ValueFilter::Less:
        if (QString::compare(value, v.toString(), m_caseSensitivity) < 0)
        {
          return true;
        }
        break;
      case ValueFilter::LessEqual:
        if (QString::compare(value, v.toString(), m_caseSensitivity) <= 0)
        {
          return true;
        }
        break;
      case ValueFilter::Equal:
        if (QString::compare(value, v.toString(), m_caseSensitivity) == 0)
        {
          return true;
        }
        break;
      case ValueFilter::GreaterEqual:
        if (QString::compare(value, v.toString(), m_caseSensitivity) >= 0)
        {
          return true;
        }
        break;
      case ValueFilter::Greater:
        if (QString::compare(value, v.toString(), m_caseSensitivity) > 0)
        {
          return true;
        }
        break;
      case ValueFilter::NotEqual:
        if (QString::compare(value, v.toString(), m_caseSensitivity) != 0)
        {
          return true;
        }
        break;
      case ValueFilter::Contains:
        if (value.contains(v.toString(), m_caseSensitivity))
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

QString ValueFilter::getCompareTypeAsString() const
{
  const QMetaObject* metaObj = metaObject();
  return  metaObj->enumerator(metaObj->indexOfEnumerator("CompareType")).valueToKey(getCompareType());
}

QString ValueFilter::getCompareFieldAsString() const
{
  const QMetaObject* metaObj = metaObject();
  return  metaObj->enumerator(metaObj->indexOfEnumerator("CompareField")).valueToKey(getCompareField());
}

QXmlStreamWriter& ValueFilter::operator<<(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("Filter");
  writer.writeTextElement("CompareType", getCompareTypeAsString());
  writer.writeTextElement("CompareField", getCompareFieldAsString());
  writer.writeTextElement("Value", m_value.toString());
  writer.writeTextElement("CaseSensitive", (m_caseSensitivity==Qt::CaseSensitive) ? "True" : "False");
  writer.writeTextElement("Invert", m_invertFilterResult ? "True" : "False");
  writer.writeTextElement("Accept", m_filterMeansAccept ? "True" : "False");
  writer.writeTextElement("Files", m_filterFiles ? "True" : "False");
  writer.writeTextElement("MultiValued", m_multiValued ? "True" : "False");
  writer.writeTextElement("Directories", m_filterDirs ? "True" : "False");
  writer.writeEndElement();
  return writer;
}

void ValueFilter::setAllDefault()
{
  setValueDefault();
  setFilterFilesDefault();
  setFilterDirsDefault();
  setFilterMeansAcceptDefault();
  setInvertFilterResultDefault();
  setCaseSensitivityDefault();
  setCompareFieldDefault();
  setCompareTypeDefault();
  setMultiValuedDefault();
}

QXmlStreamReader& ValueFilter::operator>>(QXmlStreamReader& reader)
{
  return readFilter(reader);
}

QXmlStreamReader&  ValueFilter::readFilter(QXmlStreamReader& reader)
{
  setAllDefault();
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

void ValueFilter::readInternals(QXmlStreamReader& reader, const QString&)
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
          setCompareType(static_cast<ValueFilter::CompareType>(metaObj->enumerator(metaObj->indexOfEnumerator("CompareType")).keyToValue(qPrintable(value))));
        }
      } else if (QString::compare(name, "CompareField", Qt::CaseInsensitive) == 0) {
        if (value.isEmpty()) {
          setCompareFieldDefault();
        } else {
          //TRACE_MSG(QString("Setting CompareField (%1) for filter").arg(value), 1);
          setCompareField(static_cast<ValueFilter::CompareField>(metaObj->enumerator(metaObj->indexOfEnumerator("CompareField")).keyToValue(qPrintable(value))));
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
          //TRACE_MSG(QString("Setting Multi-Valued (%1) for filter").arg(QString::compare(value, "True") == 0), 5);
          setMultiValued(QString::compare(value, "True") == 0);
        }
      } else if (QString::compare(name, "Accept", Qt::CaseInsensitive) == 0) {
        if (value.isEmpty()) {
          setFilterMeansAcceptDefault();
        } else {
          setFilterMeansAccept(QString::compare(value, "True") == 0);
        }
      } else if (QString::compare(name, "Files", Qt::CaseInsensitive) == 0) {
        if (value.isEmpty()) {
          setFilterFilesDefault();
        } else {
          setFilterFiles(QString::compare(value, "True") == 0);
        }
      } else if (QString::compare(name, "Directories", Qt::CaseInsensitive) == 0) {
        if (value.isEmpty()) {
          setFilterDirsDefault();
        } else {
          setFilterDirs(QString::compare(value, "True") == 0);
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
