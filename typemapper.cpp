#include "typemapper.h"
#include <QDate>
#include <QDateTime>

TypeMapper::TypeMapper(QObject *parent) :
  QObject(parent)
{
    initialize();
}

void TypeMapper::initialize()
{
    m_typeMapMetaToVariant.insert(QMetaType::Void, QVariant::Invalid);
    m_typeMapMetaToVariant.insert(QMetaType::Bool, QVariant::Bool);
    m_typeMapMetaToVariant.insert(QMetaType::Int, QVariant::Int);
    m_typeMapMetaToVariant.insert(QMetaType::UInt, QVariant::UInt);
    m_typeMapMetaToVariant.insert(QMetaType::Double, QVariant::Double);
    m_typeMapMetaToVariant.insert(QMetaType::QChar, QVariant::Char);
    m_typeMapMetaToVariant.insert(QMetaType::QString, QVariant::String);
    m_typeMapMetaToVariant.insert(QMetaType::QByteArray, QVariant::ByteArray);

    m_typeMapMetaToVariant.insert(QMetaType::Long, QVariant::LongLong);
    m_typeMapMetaToVariant.insert(QMetaType::LongLong, QVariant::LongLong);
    m_typeMapMetaToVariant.insert(QMetaType::Short, QVariant::Int);
    m_typeMapMetaToVariant.insert(QMetaType::Char, QVariant::Char);
    m_typeMapMetaToVariant.insert(QMetaType::ULong, QVariant::ULongLong);
    m_typeMapMetaToVariant.insert(QMetaType::ULongLong, QVariant::ULongLong);
    m_typeMapMetaToVariant.insert(QMetaType::UShort, QVariant::UInt);
    m_typeMapMetaToVariant.insert(QMetaType::UChar, QVariant::UInt);
    m_typeMapMetaToVariant.insert(QMetaType::Float, QVariant::Double);

    m_typeMapMetaToVariant.insert(QMetaType::QDate, QVariant::Date);
    m_typeMapMetaToVariant.insert(QMetaType::QTime, QVariant::Time);
    m_typeMapMetaToVariant.insert(QMetaType::QStringList, QVariant::StringList);
    m_typeMapMetaToVariant.insert(QMetaType::QUrl, QVariant::Url);
    m_typeMapMetaToVariant.insert(QMetaType::QDateTime, QVariant::DateTime);
    m_typeMapMetaToVariant.insert(QMetaType::QImage, QVariant::Image);

    m_typeMapVariantToMeta.insert(QVariant::Invalid, QMetaType::Void);
    m_typeMapVariantToMeta.insert(QVariant::Bool, QMetaType::Bool);
    m_typeMapVariantToMeta.insert(QVariant::Int, QMetaType::Int);
    m_typeMapVariantToMeta.insert(QVariant::UInt, QMetaType::UInt);
    m_typeMapVariantToMeta.insert(QVariant::Double, QMetaType::Double);
    m_typeMapVariantToMeta.insert(QVariant::Char, QMetaType::QChar);
    m_typeMapVariantToMeta.insert(QVariant::String, QMetaType::QString);
    m_typeMapVariantToMeta.insert(QVariant::ByteArray, QMetaType::QByteArray);

    //m_typeMapVariantToMeta.insert(QVariant::LongLong, QMetaType::Long);
    m_typeMapVariantToMeta.insert(QVariant::LongLong, QMetaType::LongLong);
    m_typeMapVariantToMeta.insert(QVariant::Int, QMetaType::Short);
    m_typeMapVariantToMeta.insert(QVariant::Char, QMetaType::Char);
    //m_typeMapVariantToMeta.insert(QVariant::ULongLong, QMetaType::ULong);
    m_typeMapVariantToMeta.insert(QVariant::ULongLong, QMetaType::ULongLong);
    m_typeMapVariantToMeta.insert(QVariant::UInt, QMetaType::UShort);
    //m_typeMapVariantToMeta.insert(QVariant::UInt, QMetaType::UChar);
    m_typeMapVariantToMeta.insert(QVariant::Double, QMetaType::Float);

    m_typeMapVariantToMeta.insert(QVariant::Date, QMetaType::QDate);
    m_typeMapVariantToMeta.insert(QVariant::Time, QMetaType::QTime);
    m_typeMapVariantToMeta.insert(QVariant::StringList, QMetaType::QStringList);
    m_typeMapVariantToMeta.insert(QVariant::Url, QMetaType::QUrl);
    m_typeMapVariantToMeta.insert(QVariant::DateTime, QMetaType::QDateTime);
    m_typeMapVariantToMeta.insert(QVariant::Image, QMetaType::QImage);


    m_NumList.append(QMetaType::Short);
    m_UNumList.append(QMetaType::UShort);

    m_NumList.append(QMetaType::Int);
    m_UNumList.append(QMetaType::UInt);

    m_NumList.append(QMetaType::Long);
    m_UNumList.append(QMetaType::ULong);

    m_NumList.append(QMetaType::LongLong);
    m_UNumList.append(QMetaType::ULongLong);

    m_NumList.append(QMetaType::Float);
    m_NumList.append(QMetaType::Double);
}

QMetaType::Type TypeMapper::guessType(const QString& s, const ColumnConversionPreferences preferences)
{
    QString simple = s.simplified();
    if (simple.length() == 0)
    {
        return QMetaType::Void;
    }
    QVariant x(simple);
    bool hasMinus = s.contains('-');
    if (hasMinus || ((preferences & (PreferSigned | PreferUnsigned)) == PreferSigned))
    {
        // This is NOT a non-negative integer.
        // There is not type QVariant::Long
        if (QVariant::fromValue(x).convert(QVariant::Int))
        {
          return ((preferences & (PreferLong | PreferInt)) == PreferLong) ? QMetaType::LongLong : QMetaType::Int;
        }
        else if (QVariant::fromValue(x).convert(QVariant::LongLong))
        {
            return QMetaType::LongLong;
        }
    }

    if (!hasMinus)
    {
        if (QVariant::fromValue(x).convert(QVariant::UInt))
        {
            return ((preferences & (PreferLong | PreferInt)) == PreferLong) ? QMetaType::ULongLong : QMetaType::UInt;
        }
        else if (QVariant::fromValue(x).convert(QVariant::ULongLong))
        {
            return QMetaType::ULongLong;
        }
    }

    if (QVariant::fromValue(x).convert(QVariant::Double))
    {
        return QMetaType::Double;
    }

    // This fails for "01/01/12 01:01:01" and I expected it to pass.
    if (QVariant::fromValue(x).convert(QVariant::DateTime))
    {
        return QMetaType::QDateTime;
    }

    if (QDateTime::fromString(simple, "MM/dd/yyyy hh:mm:ss").isValid())
    {
        return QMetaType::QDateTime;
    }

    if (QDateTime::fromString(simple, "yyyy-MM-ddThh:mm:ss").isValid())
    {
        return QMetaType::QDateTime;
    }

    if (QVariant::fromValue(x).convert(QVariant::Date))
    {
        return QMetaType::QDate;
    }
    if (QDate::fromString(simple, "MM/dd/yyyy").isValid())
    {
        return QMetaType::QDate;
    }

    if (QVariant::fromValue(x).convert(QVariant::Time))
    {
        return QMetaType::QTime;
    }
    if (QVariant::fromValue(x).convert(QVariant::Bool))
    {
        return QMetaType::Bool;
    }
    return QMetaType::QString;
}

QMetaType::Type TypeMapper::mostGenericType(const QMetaType::Type metaType1, const QMetaType::Type metaType2) const
{
  // qDebug(qPrintable(QString("mostGenericType(%1, %2)").arg(QMetaType::typeName(metaType1)).arg(QMetaType::typeName(metaType2))));
  if (metaType1 == metaType2)
  {
    return metaType1;
  }
  if (metaType1 == QMetaType::Void)
  {
    return metaType2;
  }
  if (metaType2 == QMetaType::Void)
  {
    return metaType1;
  }

  // Handle numbers
  int pos1InNumList = m_NumList.indexOf(metaType1);
  int pos1InUNumList = (pos1InNumList < 0) ? m_UNumList.indexOf(metaType1) : -1;

  int pos2InNumList = m_NumList.indexOf(metaType2);
  int pos2InUNumList = (pos2InNumList < 0) ? m_UNumList.indexOf(metaType2) : -1;

  if (pos1InNumList >= 0)
  {
    if (pos2InUNumList >= 0)
    {
      pos2InNumList = pos2InUNumList;
    }
    if (pos2InNumList >= 0)
    {
      return (pos2InNumList > pos1InNumList) ? m_NumList[pos2InNumList] : m_NumList[pos1InNumList];
    }
    return QMetaType::QString;
  }

  if (pos1InUNumList >= 0)
  {
    if (pos2InUNumList >= 0)
    {
      return (pos2InUNumList > pos1InUNumList) ? m_UNumList[pos2InUNumList] : m_UNumList[pos1InUNumList];
    }
    if (pos2InNumList >= 0)
    {
      return (pos2InNumList > pos1InUNumList) ? m_NumList[pos2InNumList] : m_NumList[pos1InUNumList];
    }
    return QMetaType::QString;
  }

  // TODO: Be smarter than this if needed.

  return QMetaType::QString;;
}

QMetaType::Type TypeMapper::toSignedInteger(const QMetaType::Type metaType1) const
{
  int pos1InUNumList = m_UNumList.indexOf(metaType1);
  return (pos1InUNumList >= 0) ? m_NumList[pos1InUNumList] : metaType1;
}

