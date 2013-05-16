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
    m_typeMap.insert(QMetaType::Void, QVariant::Invalid);
    m_typeMap.insert(QMetaType::Bool, QVariant::Bool);
    m_typeMap.insert(QMetaType::Int, QVariant::Int);
    m_typeMap.insert(QMetaType::UInt, QVariant::UInt);
    m_typeMap.insert(QMetaType::Double, QVariant::Double);
    m_typeMap.insert(QMetaType::QChar, QVariant::Char);
    m_typeMap.insert(QMetaType::QString, QVariant::String);
    m_typeMap.insert(QMetaType::QByteArray, QVariant::ByteArray);

    m_typeMap.insert(QMetaType::Long, QVariant::LongLong);
    m_typeMap.insert(QMetaType::LongLong, QVariant::LongLong);
    m_typeMap.insert(QMetaType::Short, QVariant::Int);
    m_typeMap.insert(QMetaType::Char, QVariant::Char);
    m_typeMap.insert(QMetaType::ULong, QVariant::ULongLong);
    m_typeMap.insert(QMetaType::ULongLong, QVariant::ULongLong);
    m_typeMap.insert(QMetaType::UShort, QVariant::UInt);
    m_typeMap.insert(QMetaType::UChar, QVariant::UInt);
    m_typeMap.insert(QMetaType::Float, QVariant::Double);

    m_typeMap.insert(QMetaType::QDate, QVariant::Date);
    m_typeMap.insert(QMetaType::QTime, QVariant::Time);
    m_typeMap.insert(QMetaType::QStringList, QVariant::StringList);
    m_typeMap.insert(QMetaType::QUrl, QVariant::Url);
    m_typeMap.insert(QMetaType::QDateTime, QVariant::DateTime);
    m_typeMap.insert(QMetaType::QImage, QVariant::Image);

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

QMetaType::Type TypeMapper::guessType(const QString& s)
{
    QString simple = s.simplified();
    if (simple.length() == 0)
    {
        return QMetaType::Void;
    }
    QVariant x(simple);
    if (s.contains('-'))
    {
        // This is NOT a non-negative integer.
        if (QVariant::fromValue(x).convert(QVariant::Int))
        {
            return QMetaType::Int;
        }
        else if (QVariant::fromValue(x).convert(QVariant::LongLong))
        {
            return QMetaType::LongLong;
        }
    }
    else
    {
        if (QVariant::fromValue(x).convert(QVariant::UInt))
        {
            return QMetaType::UInt;
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
    // TODO: This fails for "01/01/12 01:01:01" and I expected it to pass.
    if (QVariant::fromValue(x).convert(QVariant::DateTime))
    {
        return QMetaType::QDateTime;
    }
    if (QDateTime::fromString(simple, "MM/dd/yyyy hh:mm:ss").isValid())
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

