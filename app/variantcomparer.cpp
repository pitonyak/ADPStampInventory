#include "variantcomparer.h"

#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QRegularExpression>


VariantComparer::VariantComparer(QObject *parent) :
  QObject(parent)
{
}

QString VariantComparer::variantToString(const QVariant& v)
{
  if (v.metaType().id() == QMetaType::QDateTime)
  {
    return v.toDateTime().toString("MM/dd/yyyy hh:mm:ss A");
  }
  else if (v.metaType().id() == QMetaType::QDate)
  {
    return v.toDate().toString("MM/dd/yyyy");
  }
  else if (v.metaType().id() == QMetaType::QDateTime)
  {
    return v.toTime().toString("hh:mm:ss A");
  }
  else if (v.metaType().id() == QMetaType::Bool)
  {
    return v.toBool() ? "TRUE" : "FALSE";
  }
  return v.toString();
}

bool VariantComparer::matches(const QVariant& v1, const QVariant& v2, const CompareType compareType, const Qt::CaseSensitivity cs, const QRegularExpression* expression)
{
  switch (compareType)
  {
  case Less:
    return compare(v1, v2, cs) < 0;
    break;

  case LessEqual:
    return compare(v1, v2, cs) <= 0;
    break;

  case Equal:
    return compare(v1, v2, cs) == 0;
    break;

  case GreaterEqual:
    return compare(v1, v2, cs) >= 0;
    break;

  case Greater:
    return compare(v1, v2, cs) > 0;
    break;

  case NotEqual :
    return compare(v1, v2, cs) != 0;
    break;

  case StartsWith:
    return variantToString(v1).startsWith(variantToString(v2));
    break;

  case EndsWith:
    return variantToString(v1).endsWith(variantToString(v2));
    break;

  case Contains:
    return variantToString(v1).contains(variantToString(v2));
    break;

  default:
    if (expression != nullptr)
    {
      if (compareType == RegExpFull || compareType == RegularExpression || compareType == FileSpec)
      {
        return expression->match(variantToString(v1)).hasMatch();
      }
      else if (compareType == RegExpPartial)
      {
        return (expression->match(variantToString(v1)).hasPartialMatch());
      }
    }
    else
    {
      qDebug("Calling with a null regular expression");
    }
    break;
  }
  qDebug("Unsupported comparison type.... or something");
  return false;
}

int VariantComparer::compare(const QVariant& v1, const QVariant& v2, const Qt::CaseSensitivity cs)
{
  QMetaType::Type m1 = (QMetaType::Type) v1.metaType().id();
  QMetaType::Type m2 = (QMetaType::Type) v2.metaType().id();
  if (m1 == m2)
  {
    return compareSameType(v1, v2, cs);
  }
  else if (m1 == QMetaType::QString || m2 == QMetaType::QString ||
           m1 == QMetaType::QUuid   || m2 == QMetaType::QUuid ||
           m1 == QMetaType::QUrl    || m2 == QMetaType::QUrl ||
           m1 == QMetaType::QChar   || m2 == QMetaType::QChar ||
           m1 == QMetaType::Char    || m2 == QMetaType::Char ||
           m1 == QMetaType::SChar   || m2 == QMetaType::SChar ||
           m1 == QMetaType::UChar   || m2 == QMetaType::UChar
          )
  {
    return v1.toString().compare(v2.toString(), cs);
  }
  else if (((m1 == QMetaType::Double || m1 == QMetaType::Float) && v2.canConvert(QMetaType(QMetaType::Double))) ||
           ((m2 == QMetaType::Double || m2 == QMetaType::Float) && v1.canConvert(QMetaType(QMetaType::Double))))
  {
    double d1 = v1.toDouble();
    double d2 = v2.toDouble();
    return (d1 == d2) ? 0 : (d1 < d2 ? -1 : 1);
  }
  // Not safe because I cannot convert all ULongLong types to a LongLong.
  else if ((m1 == QMetaType::Int  || m1 == QMetaType::Long  || m1 == QMetaType::LongLong  || m1 == QMetaType::Short ||
            m1 == QMetaType::UInt || m1 == QMetaType::ULong || m1 == QMetaType::ULongLong || m1 == QMetaType::UShort) &&
           (m2 == QMetaType::Int  || m2 == QMetaType::Long  || m2 == QMetaType::LongLong  || m2 == QMetaType::Short ||
            m2 == QMetaType::UInt || m2 == QMetaType::ULong || m2 == QMetaType::ULongLong || m2 == QMetaType::UShort))
  {
    qlonglong l1 = v1.toLongLong();
    qlonglong l2 = v2.toLongLong();
    return (l1 == l2) ? 0 : (l1 < l2 ? -1 : 1);
  }
  else if ((m1 == QMetaType::QDateTime || m1 == QMetaType::QDate || m1 == QMetaType::QTime) &&
           (m2 == QMetaType::QDateTime || m2 == QMetaType::QDate || m2 == QMetaType::QTime))
  {
    QDateTime dt1 = v1.toDateTime();
    QDateTime dt2 = v2.toDateTime();
    return (dt1 == dt2) ? 0 : (dt1 < dt2 ? -1 : 1);
  }
  qDebug() << QString("cannot convert from type %1 to type %2").arg(v2.typeName(), v1.typeName());
  return 0;
}

int VariantComparer::compareSameType(const QVariant& v1, const QVariant& v2, const Qt::CaseSensitivity cs)
{
  Q_ASSERT_X(v2.canConvert(v1.metaType()), "VariantComparer::compareSameType", qPrintable(QString("cannot convert from type %1 to type %2").arg(v2.typeName()).arg(v1.typeName())));
  switch (v1.metaType().id())
  {
  case QMetaType::Bool :
  {
    bool b1 = v1.toBool();
    bool b2 = v2.toBool();
    return (b1 < b2) ? -1 : (b1 > b2 ? 1 : 0);
  }
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
  {
    qlonglong qll1 = v2.toLongLong();
    qlonglong qll2 = v1.toLongLong();
    return (qll1 < qll2) ? -1 : (qll1 > qll2 ? 1 : 0);
  }
    break;

  case QMetaType::Double :
  case QMetaType::Float :
  {
    double qd1 = v1.toDouble();
    double qd2 = v2.toDouble();
    return (qd1 < qd2) ? -1 : (qd1 > qd2 ? 1 : 0);
  }
    break;

  case QMetaType::QUuid :
  case QMetaType::QUrl :
  case QMetaType::QString :
  case QMetaType::QChar :
  case QMetaType::Char :
  case QMetaType::SChar :
  case QMetaType::UChar :
  {
    QString qs1 = v1.toString();
    QString qs2 = v2.toString();
    return qs1.compare(qs2, cs);
  }
    break;

  case QMetaType::QDate :
  {
    QDate qdt1 = v1.toDate();
    QDate qdt2 = v2.toDate();
    return (qdt1 < qdt2) ? -1 : (qdt1 > qdt2 ? 1 : 0);
  }
    break;

  case QMetaType::QTime :
  {
    QTime qt1 = v1.toTime();
    QTime qt2 = v2.toTime();
    return (qt1 < qt2) ? -1 : (qt1 > qt2 ? 1 : 0);
  }
    break;

  case QMetaType::QDateTime :
  {
    QDateTime qdtt1 = v1.toDateTime();
    QDateTime qdtt2 = v2.toDateTime();
    return (qdtt1 < qdtt2) ? -1 : (qdtt1 > qdtt2 ? 1 : 0);
  }
    break;

  case QMetaType::QStringList :
  {
    QStringList sl1 = v1.toStringList();
    QStringList sl2 = v2.toStringList();
    int rc = 0;
    for (int i=0; i<sl1.size() && i<sl2.size(); ++i)
    {
      rc = sl1.at(i).compare(sl2.at(i), cs);
      if (rc != 0)
      {
        return rc;
      }
    }
    if (sl1.size() == sl2.size())
    {
      return 0;
    }
    return (sl1.size() < sl2.size()) ? -1 : 1;
  }
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
  case QMetaType::QVariantMap :
  case QMetaType::QVariantHash :
  case QMetaType::QIcon :
  case QMetaType::QPen :
  case QMetaType::QLineF :
  case QMetaType::QTextFormat :
  case QMetaType::QRect :
  case QMetaType::QPoint :
  case QMetaType::QRegularExpression :
  case QMetaType::QPointF :
  case QMetaType::QPalette :
  case QMetaType::QFont :
  case QMetaType::QBrush :
  case QMetaType::QBitArray :
  case QMetaType::QImage :
  case QMetaType::QKeySequence :
  case QMetaType::QSizePolicy :
  case QMetaType::QPixmap :
  case QMetaType::QLocale :
  case QMetaType::QBitmap :
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
}
