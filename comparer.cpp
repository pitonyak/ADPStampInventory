#include "comparer.h"
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QUuid>
#include <QDebug>

TypeMapper Comparer::mapper;

Comparer::Comparer(QObject *parent) :
  QObject(parent)
{
}

bool Comparer::isEqual(const QString& x1, const QString& x2, Qt::CaseSensitivity cs)
{
  return (x1.compare(x2, cs) == 0);
}

bool Comparer::isEqual(const QVariant& x1, const QVariant& x2, Qt::CaseSensitivity cs)
{
  return compare(x1, x2, cs) == 0;
}

bool Comparer::isLess(const QString& x1, const QString& x2, Qt::CaseSensitivity cs)
{
  return compare(x1, x2, cs) < 0;
}

bool Comparer::isLess(const QVariant& x1, const QVariant& x2, Qt::CaseSensitivity cs)
{
  return compare(x1, x2, cs) < 0;
}

int Comparer::compare(const QString& x1, const QString& x2, Qt::CaseSensitivity cs)
{
  return x1.compare(x2, cs);
}

int Comparer::compare(const QVariant& x1, const QVariant& x2, Qt::CaseSensitivity cs)
{
  QMetaType::Type x1Type = (QMetaType::Type) x1.metaType().id();
  QMetaType::Type x2Type = (QMetaType::Type) x2.metaType().id();

  if (x1.isNull() || !x1.isValid())
  {
    return (x2.isNull() || !x2.isValid()) ? 0 : -1;
  }
  else if (x2.isNull() || !x2.isValid())
  {
    return 1;
  }
  else if (x1.metaType().id() == x2.metaType().id())
  {
    if (x1Type == QMetaType::QString || x1Type == QMetaType::QUrl || x1Type == QMetaType::QUuid)
    {
      return compare(x1.toString(), x2.toString(), cs);
    }
    // Types are the same, so make a cheat.
    if (x1 == x2)
    {
      return 0;
    }
    switch (x1Type)
    {
    case QMetaType::Char :
    case QMetaType::QChar :
    case QMetaType::SChar :
    case QMetaType::UChar :
      if (cs == Qt::CaseInsensitive)
      {
        QChar c1 = x1.toChar().toLower();
        QChar c2 = x2.toChar().toLower();
        return (c1 == c2) ? 0 : ((c1 < c2) ? -1 : 1);
      }
      else
      {
        QChar c1 = x1.toChar();
        QChar c2 = x2.toChar();
        return (c1 == c2) ? 0 : ((c1 < c2) ? -1 : 1);
      }
      //break;
    case QMetaType::Bool :
      return (x1.toBool() < x2.toBool()) ? -1 : 1;
      //break;
    case QMetaType::QDate :
      return (x1.toDate() < x2.toDate()) ? -1 : 1;
      //break;
    case QMetaType::QDateTime :
      return (x1.toDateTime() < x2.toDateTime()) ? -1 : 1;
      //break;
    case QMetaType::QTime :
      return (x1.toTime() < x2.toTime()) ? -1 : 1;
      //break;
    case QMetaType::Double :
    case QMetaType::Float :
      return (x1.toDouble() < x2.toDouble()) ? -1 : 1;
      //break;
    case QMetaType::Int :
    case QMetaType::Short :
      return (x1.toInt() < x2.toInt()) ? -1 : 1;
      //break;
    case QMetaType::LongLong :
    case QMetaType::Long :
      return (x1.toLongLong() < x2.toLongLong()) ? -1 : 1;
      //break;
    case QMetaType::UInt :
    case QMetaType::UShort :
      return (x1.toUInt() < x2.toUInt()) ? -1 : 1;
      //break;
    case QMetaType::ULongLong :
    case QMetaType::ULong :
      return (x1.toULongLong() < x2.toULongLong()) ? -1 : 1;
      //break;
    case QMetaType::QUuid :
      return (x1.toUuid() < x2.toUuid()) ? -1 : 1;
      //break;
    default:
      break;
    }
  }
  else if (x1Type == QMetaType::QString || x2Type == QMetaType::QString ||
           x1Type == QMetaType::Char || x2Type == QMetaType::Char ||
           x1Type == QMetaType::QChar || x2Type == QMetaType::QChar ||
           x1Type == QMetaType::SChar || x2Type == QMetaType::SChar ||
           x1Type == QMetaType::UChar || x2Type == QMetaType::UChar ||
           x1Type == QMetaType::QUuid || x2Type == QMetaType::QUuid)
  {
    return compare(x1.toString(), x2.toString(), cs);
  }
  else if (x1Type == QMetaType::Double ||
           x1Type == QMetaType::Float ||
           x1Type == QMetaType::Int ||
           x1Type == QMetaType::Short ||
           x1Type == QMetaType::UShort ||
           x1Type == QMetaType::Long ||
           x1Type == QMetaType::ULong ||
           x1Type == QMetaType::LongLong ||
           x1Type == QMetaType::UInt ||
           x1Type == QMetaType::ULongLong)
  {
    if (x2Type == QMetaType::Double ||
        x2Type == QMetaType::Float ||
        x2Type == QMetaType::Int ||
        x2Type == QMetaType::Short ||
        x2Type == QMetaType::UShort ||
        x2Type == QMetaType::Long ||
        x2Type == QMetaType::ULong ||
        x2Type == QMetaType::LongLong ||
        x2Type == QMetaType::UInt ||
        x2Type == QMetaType::ULongLong)
    {
      double d1 = x1.toDouble();
      double d2 = x2.toDouble();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
  }
  else if (x1Type == QMetaType::QDateTime)
  {
    if (x2Type == QMetaType::QDate)
    {
      QDateTime d2(x2.toDate(), QTime(0, 0, 0, 0));
      QDateTime d1 = x1.toDateTime();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
    else if (x2Type == QMetaType::QTime)
    {
      QTime d2 = x2.toTime();
      QTime d1 = x1.toDateTime().time();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
  }
  else if (x1Type == QMetaType::QDate)
  {
    if (x2Type == QMetaType::QDateTime)
    {
      QDateTime d1(x1.toDate(), QTime(0, 0, 0, 0));
      QDateTime d2 = x2.toDateTime();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
    else if (x2Type == QMetaType::QTime)
    {
      QTime d2(0, 0, 0, 0);
      QTime d1 = x1.toTime();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
  }
  else if (x1Type == QMetaType::QTime)
  {
    if (x2Type == QMetaType::QDateTime)
    {
      QTime d2 = x2.toDateTime().time();
      QTime d1 = x1.toTime();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
    else if (x2Type == QMetaType::QDate)
    {
      QTime d2(0, 0, 0, 0);
      QTime d1 = x1.toTime();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
  }

  qDebug() << qPrintable(QString(tr("Unexpected type in Comparer::compare: (%1)(%2)")).arg(x1.typeName()).arg(x2.typeName()));
  return compare(x1.toString(), x2.toString(), cs);
}

void Comparer::setInvertResult(const bool b)
{
  if (b)
  {
    m_flags = static_cast<Comparer::FilterFlags>(m_flags | Comparer::InvertResult);
  }
  else
  {
    m_flags = static_cast<Comparer::FilterFlags>(m_flags & ~Comparer::InvertResult);
  }
}

void Comparer::setMultiValued(const bool b)
{
  if (b)
  {
    m_flags = static_cast<Comparer::FilterFlags>(m_flags | Comparer::MultiValued);
  }
  else
  {
    m_flags = static_cast<Comparer::FilterFlags>(m_flags & ~Comparer::MultiValued);
  }
}

void Comparer::setMatchMeansReject(const bool b)
{
  if (b)
  {
    m_flags = static_cast<Comparer::FilterFlags>(m_flags | Comparer::MatchMeansReject);
  }
  else
  {
    m_flags = static_cast<Comparer::FilterFlags>(m_flags & ~Comparer::MatchMeansReject);
  }
}
