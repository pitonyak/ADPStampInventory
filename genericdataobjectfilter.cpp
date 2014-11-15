#include "genericdataobjectfilter.h"
#include "typemapper.h"
#include "genericdataobject.h"

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

bool GenericDataObjectFilter::objectMatchesFilter(const GenericDataObject& obj) const
{
  // Perhaps I should match against an empty variant object if this does not match.
  return obj.containsValue(m_compareField) ? variantMatchesFilter(obj.getValue(m_compareField)) : false;
}

bool GenericDataObjectFilter::variantMatchesFilter(const QVariant& obj) const
{
  bool filterPass = false;
  switch (m_fieldType)
  {
  case QMetaType::Bool :
    filterPass = compareValues(obj.toBool());
    break;

  // I should be more precise on this, but, unless there
  // is a performance problem, this is less code.
  case QMetaType::Int :
  case QMetaType::UInt :
  case QMetaType::Long :
  case QMetaType::LongLong :
  case QMetaType::Short :
  case QMetaType::ULong :
  case QMetaType::ULongLong :
  case QMetaType::UShort :
    filterPass = compareValues(obj.toLongLong());
    break;

  case QMetaType::Double :
  case QMetaType::Float :
    filterPass = compareValues(obj.toDouble());
    break;

  case QMetaType::QUuid :
  case QMetaType::QUrl :
  case QMetaType::QString :
  case QMetaType::QChar :
  case QMetaType::Char :
  case QMetaType::SChar :
  case QMetaType::UChar :
    filterPass = compareValues(obj.toString());
    break;

  case QMetaType::QDate :
    filterPass = compareValues(obj.toDate());
    break;

  case QMetaType::QTime :
    filterPass = compareValues(obj.toTime());
    break;

  case QMetaType::QDateTime :
    filterPass = compareValues(obj.toDateTime());
    break;

  case QMetaType::Void :
  case QMetaType::QByteArray :
  case QMetaType::VoidStar :
  case QMetaType::QObjectStar :
  case QMetaType::QVariant :
  case QMetaType::QCursor :
  case QMetaType::QSize :
  case QMetaType::QVariantList :
  case QMetaType::QPolygon :
  case QMetaType::QPolygonF :
  case QMetaType::QColor :
  case QMetaType::QSizeF :
  case QMetaType::QRectF :
  case QMetaType::QLine :
  case QMetaType::QTextLength :
  case QMetaType::QStringList :
  case QMetaType::QVariantMap :
  case QMetaType::QVariantHash :
  case QMetaType::QIcon :
  case QMetaType::QPen :
  case QMetaType::QLineF :
  case QMetaType::QTextFormat :
  case QMetaType::QRect :
  case QMetaType::QPoint :
  case QMetaType::QRegExp :
  case QMetaType::QRegularExpression :
  case QMetaType::QPointF :
  case QMetaType::QPalette :
  case QMetaType::QFont :
  case QMetaType::QBrush :
  case QMetaType::QRegion :
  case QMetaType::QBitArray :
  case QMetaType::QImage :
  case QMetaType::QKeySequence :
  case QMetaType::QSizePolicy :
  case QMetaType::QPixmap :
  case QMetaType::QLocale :
  case QMetaType::QBitmap :
  case QMetaType::QMatrix :
  case QMetaType::QTransform :
  case QMetaType::QMatrix4x4 :
  case QMetaType::QVector2D :
  case QMetaType::QVector3D :
  case QMetaType::QVector4D :
  case QMetaType::QQuaternion :
  case QMetaType::QEasingCurve :
  case QMetaType::QJsonValue :
  case QMetaType::QJsonObject :
  case QMetaType::QJsonArray :
  case QMetaType::QJsonDocument :
  case QMetaType::QModelIndex :
  case QMetaType::User :
  case QMetaType::UnknownType :
      // TODO: not supported
      return false;
      break;

  default :
      return false;
      break;
  }

  return m_invertFilterResult ? !filterPass : filterPass;
}

bool GenericDataObjectFilter::compareValues(const bool x) const
{
  switch (m_compareType)
  {
  case GenericDataObjectFilter::RegExpFull:
  case GenericDataObjectFilter::RegularExpression:
  case GenericDataObjectFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch( x ? "TRUE" : "FALSE"))
      {
        return true;
      }
    }
    break;
  case GenericDataObjectFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(x ? "TRUE" : "FALSE") >= 0))
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
        if (x < v.toBool())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::LessEqual:
        if (x <= v.toBool())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Equal:
        if (x == v.toBool())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::GreaterEqual:
        if (x >= v.toBool())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Greater:
        if (x > v.toBool())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::NotEqual:
        if (x != v.toBool())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Contains:
        {
          QString b = x ? "TRUE" : "FALSE";
          if (b.contains(v.toString(), m_caseSensitivity))
          {
            return true;
          }
        }
        break;
      case GenericDataObjectFilter::EndsWith:
        {
          QString b = x ? "TRUE" : "FALSE";
          if (b.startsWith(v.toString(), m_caseSensitivity))
          {
            return true;
          }
        }
        break;
      case GenericDataObjectFilter::StartsWith:
        {
          QString b = x ? "TRUE" : "FALSE";
          if (b.endsWith(v.toString(), m_caseSensitivity))
          {
            return true;
          }
        }
        break;
      default:
        break;
      }
    }
  }
  return false;
}

bool GenericDataObjectFilter::compareValues(const double x) const
{
  switch (m_compareType)
  {
  case GenericDataObjectFilter::RegExpFull:
  case GenericDataObjectFilter::RegularExpression:
  case GenericDataObjectFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(QString::number(x)))
      {
        return true;
      }
    }
    break;
  case GenericDataObjectFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(QString::number(x)) >= 0))
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
        if (x < v.toDouble())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::LessEqual:
        if (x <= v.toDouble())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Equal:
        if (x == v.toDouble())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::GreaterEqual:
        if (x >= v.toDouble())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Greater:
        if (x > v.toDouble())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::NotEqual:
        if (x != v.toDouble())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Contains:
        if (QString::number(x).contains(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::EndsWith:
        if (QString::number(x).startsWith(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::StartsWith:
        if (QString::number(x).endsWith(v.toString(), m_caseSensitivity))
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


bool GenericDataObjectFilter::compareValues(const qlonglong x) const
{
  switch (m_compareType)
  {
  case GenericDataObjectFilter::RegExpFull:
  case GenericDataObjectFilter::RegularExpression:
  case GenericDataObjectFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(QString::number(x)))
      {
        return true;
      }
    }
    break;
  case GenericDataObjectFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(QString::number(x)) >= 0))
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
        if (x < v.toLongLong())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::LessEqual:
        if (x <= v.toLongLong())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Equal:
        if (x == v.toLongLong())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::GreaterEqual:
        if (x >= v.toLongLong())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Greater:
        if (x > v.toLongLong())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::NotEqual:
        if (x != v.toLongLong())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Contains:
        if (QString::number(x).contains(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::EndsWith:
        if (QString::number(x).startsWith(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::StartsWith:
        if (QString::number(x).endsWith(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      default:
        qDebug("Unexpected unsupported compare type");
        break;
      }
    }
  }

  return false;
}


bool GenericDataObjectFilter::compareValues(const QTime& x) const
{
  switch (m_compareType)
  {
  case GenericDataObjectFilter::RegExpFull:
  case GenericDataObjectFilter::RegularExpression:
  case GenericDataObjectFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(x.toString("hh:mm:ss A")))
      {
        return true;
      }
    }
    break;
  case GenericDataObjectFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(x.toString("hh:mm:ss A")) >= 0))
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
        if (x < v.toTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::LessEqual:
        if (x <= v.toTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Equal:
        if (x == v.toTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::GreaterEqual:
        if (x >= v.toTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Greater:
        if (x > v.toTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::NotEqual:
        if (x != v.toTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Contains:
        if (x.toString("hh:mm:ss A").contains(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::EndsWith:
        if (x.toString("hh:mm:ss A").startsWith(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::StartsWith:
        if (x.toString("hh:mm:ss A").endsWith(v.toString(), m_caseSensitivity))
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

bool GenericDataObjectFilter::compareValues(const QDate& x) const
{
  switch (m_compareType)
  {
  case GenericDataObjectFilter::RegExpFull:
  case GenericDataObjectFilter::RegularExpression:
  case GenericDataObjectFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(x.toString("MM/dd/yyyy")))
      {
        return true;
      }
    }
    break;
  case GenericDataObjectFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(x.toString("MM/dd/yyyy")) >= 0))
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
        if (x < v.toDate())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::LessEqual:
        if (x <= v.toDate())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Equal:
        if (x == v.toDate())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::GreaterEqual:
        if (x >= v.toDate())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Greater:
        if (x > v.toDate())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::NotEqual:
        if (x != v.toDate())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Contains:
        if (x.toString("MM/dd/yyyy").contains(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::EndsWith:
        if (x.toString("MM/dd/yyyy").startsWith(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::StartsWith:
        if (x.toString("MM/dd/yyyy").endsWith(v.toString(), m_caseSensitivity))
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

bool GenericDataObjectFilter::compareValues(const QDateTime& x) const
{
  switch (m_compareType)
  {
  case GenericDataObjectFilter::RegExpFull:
  case GenericDataObjectFilter::RegularExpression:
  case GenericDataObjectFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(x.toString("MM/dd/yyyy hh:mm:ss A")))
      {
        return true;
      }
    }
    break;
  case GenericDataObjectFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(x.toString("MM/dd/yyyy hh:mm:ss A")) >= 0))
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
        if (x < v.toDateTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::LessEqual:
        if (x <= v.toDateTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Equal:
        if (x == v.toDateTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::GreaterEqual:
        if (x >= v.toDateTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Greater:
        if (x > v.toDateTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::NotEqual:
        if (x != v.toDateTime())
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Contains:
        if (x.toString("MM/dd/yyyy hh:mm:ss A").contains(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::EndsWith:
        if (x.toString("MM/dd/yyyy hh:mm:ss A").startsWith(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::StartsWith:
        if (x.toString("MM/dd/yyyy hh:mm:ss A").endsWith(v.toString(), m_caseSensitivity))
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

bool GenericDataObjectFilter::compareValues(const QString& x) const
{
  switch (m_compareType)
  {
  case GenericDataObjectFilter::RegExpFull:
  case GenericDataObjectFilter::RegularExpression:
  case GenericDataObjectFilter::FileSpec:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && expression->exactMatch(x))
      {
        return true;
      }
    }
    break;
  case GenericDataObjectFilter::RegExpPartial:
    foreach (QRegExp* expression, *m_expressions)
    {
      if ((expression != nullptr) && (expression->indexIn(x) >= 0))
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
        if (x.compare(v.toString(), m_caseSensitivity) < 0)
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::LessEqual:
        if (x.compare(v.toString(), m_caseSensitivity) <= 0)
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Equal:
        if (x.compare(v.toString(), m_caseSensitivity) == 0)
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::GreaterEqual:
        if (x.compare(v.toString(), m_caseSensitivity) >= 0)
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Greater:
        if (x.compare(v.toString(), m_caseSensitivity) > 0)
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::NotEqual:
        if (x.compare(v.toString(), m_caseSensitivity) != 0)
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::Contains:
        if (x.contains(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::EndsWith:
        if (x.startsWith(v.toString(), m_caseSensitivity))
        {
          return true;
        }
        break;
      case GenericDataObjectFilter::StartsWith:
        if (x.endsWith(v.toString(), m_caseSensitivity))
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
          setCompareType(GenericDataObjectFilter::Equal);
        } else {
          setCompareType(static_cast<GenericDataObjectFilter::CompareType>(metaObj->enumerator(metaObj->indexOfEnumerator("CompareType")).keyToValue(qPrintable(value))));
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
