#include "comparer.h"
#include <QDate>
#include <QDateTime>
#include <QTime>
#include <QUuid>

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
  if (x1.isNull() || !x1.isValid())
  {
    return (x2.isNull() || !x2.isValid()) ? 0 : -1;
  }
  else if (x2.isNull() || !x2.isValid())
  {
    return 1;
  }
  else if (x1.type() == x2.type())
  {
    if (x1.type() == QVariant::String)
    {
      return compare(x1.toString(), x2.toString(), cs);
    }
    // Types are the same, so make a cheat.
    if (x1 == x2)
    {
      return 0;
    }
    switch (x1.type())
    {
    case QVariant::Char :
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
      break;
    case QVariant::Bool :
      return (x1.toBool() < x2.toBool()) ? -1 : 1;
      break;
    case QVariant::Date :
      return (x1.toDate() < x2.toDate()) ? -1 : 1;
      break;
    case QVariant::DateTime :
      return (x1.toDateTime() < x2.toDateTime()) ? -1 : 1;
      break;
    case QVariant::Time :
      return (x1.toTime() < x2.toTime()) ? -1 : 1;
      break;
    case QVariant::Double :
      return (x1.toDouble() < x2.toDouble()) ? -1 : 1;
      break;
    case QVariant::Int :
      return (x1.toInt() < x2.toInt()) ? -1 : 1;
      break;
    case QVariant::LongLong :
      return (x1.toLongLong() < x2.toLongLong()) ? -1 : 1;
      break;
    case QVariant::UInt :
      return (x1.toUInt() < x2.toUInt()) ? -1 : 1;
      break;
    case QVariant::ULongLong :
      return (x1.toULongLong() < x2.toULongLong()) ? -1 : 1;
      break;
    case QVariant::Uuid :
      return (x1.toUuid() < x2.toUuid()) ? -1 : 1;
      break;
    default:
      break;
    }
  }
  else if (x1.type() == QVariant::String || x2.type() == QVariant::String ||
           x1.type() == QVariant::Char || x2.type() == QVariant::Char ||
           x1.type() == QVariant::Uuid || x2.type() == QVariant::Uuid)
  {
    return compare(x1.toString(), x2.toString(), cs);
  }
  else if (x1.type() == QVariant::Double ||
           x1.type() == QVariant::Int ||
           x1.type() == QVariant::LongLong ||
           x1.type() == QVariant::UInt ||
           x1.type() == QVariant::ULongLong)
  {
    if (x2.type() == QVariant::Double ||
        x2.type() == QVariant::Int ||
        x2.type() == QVariant::LongLong ||
        x2.type() == QVariant::UInt ||
        x2.type() == QVariant::ULongLong)
    {
      double d1 = x1.toDouble();
      double d2 = x2.toDouble();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
  }
  else if (x1.type() == QVariant::DateTime)
  {
    if (x2.type() == QVariant::Date)
    {
      QDateTime d2(x2.toDate(), QTime(0, 0, 0, 0));
      QDateTime d1 = x1.toDateTime();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
    else if (x2.type() == QVariant::Time)
    {
      QTime d2 = x2.toTime();
      QTime d1 = x1.toDateTime().time();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
  }
  else if (x1.type() == QVariant::Date)
  {
    if (x2.type() == QVariant::DateTime)
    {
      QDateTime d1(x1.toDate(), QTime(0, 0, 0, 0));
      QDateTime d2 = x2.toDateTime();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
    else if (x2.type() == QVariant::Time)
    {
      QTime d2(0, 0, 0, 0);
      QTime d1 = x1.toTime();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
  }
  else if (x1.type() == QVariant::Time)
  {
    if (x2.type() == QVariant::DateTime)
    {
      QTime d2 = x2.toDateTime().time();
      QTime d1 = x1.toTime();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
    else if (x2.type() == QVariant::Date)
    {
      QTime d2(0, 0, 0, 0);
      QTime d1 = x1.toTime();
      return (d1 == d2) ? 0 : ((d1 < d2) ? -1 : 1);
    }
  }

  qDebug(qPrintable(QString(tr("Unexpected type2 in Comparer::compare: (%1)(%2)")).arg(x1.typeName()).arg(x2.typeName())));
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
