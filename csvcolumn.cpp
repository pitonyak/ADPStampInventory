#include "csvcolumn.h"
#include <QVariant>
#include <QDateTime>
#include <QDate>
#include <QMetaType>

TypeMapper CSVColumn::m_typeMap;

CSVColumn::CSVColumn(QObject *parent) :
  QObject(parent),
  m_qualified(true),
  m_type(QMetaType::Void),
  m_value("")
{
}

CSVColumn::CSVColumn(const QString& value, QObject *parent) :
  QObject(parent),
  m_qualified(true),
  m_type(QMetaType::Void),
  m_value(value)
{
}

CSVColumn::CSVColumn(const QString& value, bool qualified, QMetaType::Type colType, QObject *parent) :
  QObject(parent),
  m_qualified(qualified),
  m_type(colType),
  m_value(value)
{
}

CSVColumn::CSVColumn(const CSVColumn& x, QObject *parent) :
  QObject(parent)
{
  copyFrom(x);
}

const CSVColumn& CSVColumn::operator=(const CSVColumn& x)
{
  return copyFrom(x);
}

const CSVColumn& CSVColumn::copyFrom(const CSVColumn& x)
{
  if (this != &x)
  {
    m_qualified = x.isQualified();
    m_type = x.getType();
    m_value = x.getValue();
  }
  return *this;
}

QString CSVColumn::getQualifiedAsString() const
{
  return m_qualified ? "Qualified" : "Unqualified";
}

QString CSVColumn::getTypeAsString() const
{
  return QMetaType(m_type).name();
}

QString CSVColumn::toString(bool brief) const
{
  // TODO: get string delimiters from the controller
  QString value = m_qualified ? QString("\"%1\"").arg(m_value) : m_value;
  return brief ? value : QString(tr("(%1, %2)")).arg(value, getTypeAsString());
}

QMetaType::Type CSVColumn::guessType(const QString& s, const TypeMapper::ColumnConversionPreferences preference)
{
  return m_typeMap.guessType(s, preference);
}

// ?? TODO: bool CSVColumn::canConvertToVariant() const
// ?? TODO: {
  // ?? TODO: return QVariant::fromValue(m_value).canConvert(m_typeMap.metaToVariantType(m_type));
// ?? TODO: }

QVariant CSVColumn::toVariant() const
{
  QVariant v = QVariant::fromValue(m_value);
  if (v.convert(QMetaType(m_type)))
  {
    return v;
  }
  if (m_type == QMetaType::QDateTime)
  {
    // So it must be in the special format not recognized by the variant class
    QDateTime dateTime = QDateTime::fromString(m_value, "MM/dd/yyyy hh:mm:ss");
    if (dateTime.isValid())
    {
      return QVariant::fromValue(dateTime);
    }
  }
  if (m_type == QMetaType::QDate)
  {
    // So it must be in the special format not recognized by the variant class
    QDate date = QDate::fromString(m_value, "MM/dd/yyyy");
    if (date.isValid())
    {
      return QVariant::fromValue(date);
    }
  }
  // TODO: Error occurred here
  return v;
}
