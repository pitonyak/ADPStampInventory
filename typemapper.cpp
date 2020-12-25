#include "typemapper.h"
#include <QDate>
#include <QDateTime>
#include <QBitArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QEasingCurve>
#include <QLineF>
#include <QRect>
#include <QRectF>
#include <QModelIndex>
#include <QSize>
#include <QSizeF>
#include <QUrl>
#include <QUuid>
#include <QLocale>
#include <QtGlobal>
#include <QRegularExpression>


TypeMapper::TypeMapper(QObject *parent) :
  QObject(parent)
{
    initialize();
}

void TypeMapper::initialize()
{
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

    m_MetaToName.insert(QMetaType::Void,"Void");
    m_MetaToName.insert(QMetaType::Bool,"Boolean");
    m_MetaToName.insert(QMetaType::Int,"Int");
    m_MetaToName.insert(QMetaType::UInt,"Unsigned Int");
    m_MetaToName.insert(QMetaType::Double,"Double");
    m_MetaToName.insert(QMetaType::QChar,"Char");
    m_MetaToName.insert(QMetaType::QString,"Text");
    m_MetaToName.insert(QMetaType::QByteArray,"ByteArray");
    m_MetaToName.insert(QMetaType::VoidStar,"Void *");
    m_MetaToName.insert(QMetaType::Long,"Long");
    m_MetaToName.insert(QMetaType::LongLong,"LongLong");
    m_MetaToName.insert(QMetaType::Short,"Short");
    m_MetaToName.insert(QMetaType::Char,"Char");
    m_MetaToName.insert(QMetaType::ULong,"Unsigned Long");
    m_MetaToName.insert(QMetaType::ULongLong,"Unsigned LongLong");
    m_MetaToName.insert(QMetaType::UShort,"Unsigned Short");
    m_MetaToName.insert(QMetaType::SChar,"Signed Char");
    m_MetaToName.insert(QMetaType::UChar,"Unsigned Char");
    m_MetaToName.insert(QMetaType::Float,"Float");
    m_MetaToName.insert(QMetaType::QObjectStar,"Object *");
    m_MetaToName.insert(QMetaType::QVariant,"Variant");
    m_MetaToName.insert(QMetaType::QCursor,"Cursor");
    m_MetaToName.insert(QMetaType::QDate,"Date");
    m_MetaToName.insert(QMetaType::QSize,"Size");
    m_MetaToName.insert(QMetaType::QTime,"Time");
    m_MetaToName.insert(QMetaType::QVariantList,"VariantList");
    m_MetaToName.insert(QMetaType::QPolygon,"Polygon");
    m_MetaToName.insert(QMetaType::QPolygonF,"PolygonF");
    m_MetaToName.insert(QMetaType::QColor,"Color");
    m_MetaToName.insert(QMetaType::QSizeF,"SizeF");
    m_MetaToName.insert(QMetaType::QRectF,"RectF");
    m_MetaToName.insert(QMetaType::QLine,"Line");
    m_MetaToName.insert(QMetaType::QTextLength,"TextLength");
    m_MetaToName.insert(QMetaType::QStringList,"StringList");
    m_MetaToName.insert(QMetaType::QVariantMap,"VariantMap");
    m_MetaToName.insert(QMetaType::QVariantHash,"VariantHash");
    m_MetaToName.insert(QMetaType::QIcon,"Icon");
    m_MetaToName.insert(QMetaType::QPen,"Pen");
    m_MetaToName.insert(QMetaType::QLineF,"LineF");
    m_MetaToName.insert(QMetaType::QTextFormat,"TextFormat");
    m_MetaToName.insert(QMetaType::QRect,"Rect");
    m_MetaToName.insert(QMetaType::QPoint,"Point");
    m_MetaToName.insert(QMetaType::QUrl,"Url");
    m_MetaToName.insert(QMetaType::QRegularExpression,"RegularExpression");
    m_MetaToName.insert(QMetaType::QDateTime,"DateTime");
    m_MetaToName.insert(QMetaType::QPointF,"PointF");
    m_MetaToName.insert(QMetaType::QPalette,"Palette");
    m_MetaToName.insert(QMetaType::QFont,"Font");
    m_MetaToName.insert(QMetaType::QBrush,"Brush");
    m_MetaToName.insert(QMetaType::QRegion,"Region");
    m_MetaToName.insert(QMetaType::QBitArray,"BitArray");
    m_MetaToName.insert(QMetaType::QImage,"Image");
    m_MetaToName.insert(QMetaType::QKeySequence,"KeySequence");
    m_MetaToName.insert(QMetaType::QSizePolicy,"SizePolicy");
    m_MetaToName.insert(QMetaType::QPixmap,"Pixmap");
    m_MetaToName.insert(QMetaType::QLocale,"Locale");
    m_MetaToName.insert(QMetaType::QBitmap,"Bitmap");
    m_MetaToName.insert(QMetaType::QTransform,"Transform");
    m_MetaToName.insert(QMetaType::QMatrix4x4,"Matrix4x4");
    m_MetaToName.insert(QMetaType::QVector2D,"Vector2D");
    m_MetaToName.insert(QMetaType::QVector3D,"Vector3D");
    m_MetaToName.insert(QMetaType::QVector4D,"Vector4D");
    m_MetaToName.insert(QMetaType::QQuaternion,"Quaternion");
    m_MetaToName.insert(QMetaType::QEasingCurve,"EasingCurve");
    m_MetaToName.insert(QMetaType::QJsonValue,"JsonValue");
    m_MetaToName.insert(QMetaType::QJsonObject,"JsonObject");
    m_MetaToName.insert(QMetaType::QJsonArray,"JsonArray");
    m_MetaToName.insert(QMetaType::QJsonDocument,"JsonDocument");
    m_MetaToName.insert(QMetaType::QModelIndex,"ModelIndex");
    m_MetaToName.insert(QMetaType::QUuid,"Uuid");
    m_MetaToName.insert(QMetaType::User,"User type");
    m_MetaToName.insert(QMetaType::UnknownType,"Unknown");

    QMapIterator<QMetaType::Type, QString> metaNameIterator(m_MetaToName);
    while (metaNameIterator.hasNext()) {
        metaNameIterator.next();
        m_NameToMetaTo.insert(metaNameIterator.value().toLower(), metaNameIterator.key());
    }
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
        // There is not type Long
        if (QVariant::fromValue(x).canConvert(QMetaType(QMetaType::Int)))
        {
          return ((preferences & (PreferLong | PreferInt)) == PreferLong) ? QMetaType::LongLong : QMetaType::Int;
        }
        else if (QVariant::fromValue(x).canConvert(QMetaType(QMetaType::LongLong)))
        {
            return QMetaType::LongLong;
        }
    }

    if (!hasMinus)
    {
        if (QVariant::fromValue(x).canConvert(QMetaType(QMetaType::UInt)))
        {
            return ((preferences & (PreferLong | PreferInt)) == PreferLong) ? QMetaType::ULongLong : QMetaType::UInt;
        }
        else if (QVariant::fromValue(x).canConvert(QMetaType(QMetaType::ULongLong)))
        {
            return QMetaType::ULongLong;
        }
    }

    if (QVariant::fromValue(x).canConvert(QMetaType(QMetaType::Double)))
    {
        return QMetaType::Double;
    }

    // This fails for "01/01/12 01:01:01" and I expected it to pass.
    if (QVariant::fromValue(x).canConvert(QMetaType(QMetaType::QDateTime)))
    {
        return QMetaType::QDateTime;
    }

    if (QDateTime::fromString(simple, "MM/dd/yyyy hh:mm:ss P").isValid())
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

    if (QVariant::fromValue(x).canConvert(QMetaType(QMetaType::QDate)))
    {
        return QMetaType::QDate;
    }
    if (QDate::fromString(simple, "MM/dd/yyyy").isValid())
    {
        return QMetaType::QDate;
    }

    if (QVariant::fromValue(x).canConvert(QMetaType(QMetaType::QTime)))
    {
        return QMetaType::QTime;
    }
    if (QVariant::fromValue(x).canConvert(QMetaType(QMetaType::Bool)))
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


QVariant TypeMapper::forceToType(const QVariant& x, const QMetaType::Type aType, bool* ok) const
{
  QMetaType metaType(aType);
  QVariant v(x);

  if (ok != nullptr) {
    *ok = true;
  }

  if (v.convert(metaType)) {
    return v;
  }

  // The code below generally helps with the conversion, so, no need to provide a warning here.
  // Will have a better warning below.
  // qDebug() << "Cannot convert " << x.typeName() << " to " << metaType.name() << ".";

  // Types differ, try to make the conversion.
    switch (aType) {

    case QMetaType::QChar :
    case QMetaType::Char :
    case QMetaType::SChar :
    case QMetaType::UChar :
      v.setValue(x.toChar());
      break;

      // Cannot convert  QString  to  QDate
    case QMetaType::QDate :
      v.setValue(x.toDate());
      break;

    case QMetaType::QDateTime :
      v.setValue(x.toDateTime());
      break;

    case QMetaType::Double :
      v.setValue(x.toDouble(ok));
      break;

    case QMetaType::Float :
      v.setValue(x.toFloat(ok));
      break;

    case QMetaType::Int :
      v.setValue(x.toInt(ok));
      break;

    case QMetaType::LongLong :
      v.setValue(x.toLongLong(ok));
      break;

    case QMetaType::Long :
      v.setValue(x.toLongLong(ok));
      if (ok != nullptr && *ok) {
        qlonglong qll_temp = v.toLongLong();
        *ok = (std::numeric_limits<long>::min() <= qll_temp) && (qll_temp <= std::numeric_limits<long>::max());
      }
      break;

    case QMetaType::Short :
      v.setValue(x.toInt(ok));
      if (ok != nullptr && *ok) {
        qlonglong qll_temp = v.toLongLong();
        *ok = (std::numeric_limits<short>::min() <= qll_temp) && (qll_temp <= std::numeric_limits<short>::max());
      }
      break;

    case QMetaType::UInt :
      v.setValue(x.toUInt(ok));
      break;

    case QMetaType::ULongLong :
      v.setValue(x.toULongLong(ok));
      break;

    case QMetaType::ULong :
      v.setValue(x.toULongLong(ok));
      if (ok != nullptr && *ok) {
        qulonglong qll_temp = v.toULongLong();
        *ok = (std::numeric_limits<ulong>::min() <= qll_temp) && (qll_temp <= std::numeric_limits<ulong>::max());
      }
      break;

    case QMetaType::UShort :
      v.setValue(x.toUInt(ok));
      if (ok != nullptr && *ok) {
        qulonglong qll_temp = v.toULongLong();
        *ok = (std::numeric_limits<ushort>::min() <= qll_temp) && (qll_temp <= std::numeric_limits<ushort>::max());
      }
      break;

    case QMetaType::QUrl :
      v.setValue(x.toUrl());
      break;

    case QMetaType::QUuid :
      v.setValue(x.toUuid());
      break;

      v.setValue(x);
      break;

    case QMetaType::Void :
    case QMetaType::VoidStar :
    case QMetaType::QObjectStar :
    case QMetaType::QVariant :
    case QMetaType::QCursor :
    case QMetaType::QPolygon :
    case QMetaType::QPolygonF :
    case QMetaType::QColor :
    case QMetaType::QTextLength :
    case QMetaType::QIcon :
    case QMetaType::QPen :
    case QMetaType::QTextFormat :
    case QMetaType::QPalette :
    case QMetaType::QFont :
    case QMetaType::QBrush :
    case QMetaType::QImage :
    case QMetaType::QKeySequence :
    case QMetaType::QSizePolicy :
    case QMetaType::QPixmap :
    case QMetaType::QBitmap :
    case QMetaType::QTransform :
    case QMetaType::QMatrix4x4 :
    case QMetaType::QVector2D :
    case QMetaType::QVector3D :
    case QMetaType::QVector4D :
    case QMetaType::QQuaternion :
    case QMetaType::User :

      // I used to support these!
    case QMetaType::Bool :
    case QMetaType::QByteArray :
    case QMetaType::QVariantHash :
    case QMetaType::QEasingCurve :
    case QMetaType::QJsonArray :
    case QMetaType::QJsonDocument :
    case QMetaType::QJsonObject :
    case QMetaType::QJsonValue :
    case QMetaType::QLine :
    case QMetaType::QLineF :
    case QMetaType::QVariantList :
    case QMetaType::QLocale :
    case QMetaType::QVariantMap :
    case QMetaType::QModelIndex :
    case QMetaType::QPoint :
    case QMetaType::QPointF :
    case QMetaType::QRect :
    case QMetaType::QRectF :
    case QMetaType::QRegularExpression :
    case QMetaType::QSize :
    case QMetaType::QSizeF :
    case QMetaType::QString :
    case QMetaType::QStringList :
    case QMetaType::QTime :



      qDebug() << QString("Unsupported type %1").arg(metaType.name());
      if (ok != nullptr) {
        *ok = false;
      }
      break;

    default :
      qDebug() << QString("Unsupported type %1").arg(metaType.name());
      if (ok != nullptr) {
        *ok = false;
      }
      break;
    }

  if (!v.isValid())
  {
    if (ok != nullptr && *ok) {
      *ok = false;
    }
    qDebug() << QString("Cannot convert type %1 to type %2 with [%3].").arg(x.typeName(), metaType.name(), x.toString());
  }

  return v;
}


QVariant TypeMapper::getDefaultValue(QMetaType::Type aType)
{
    // Go for default values
    switch (aType) {
    case QMetaType::Bool :
        return QVariant((bool)false);
        break;

    case QMetaType::Int :
        return QVariant((int) 0);
        break;

    case QMetaType::UInt :
        return QVariant((unsigned int) 0);
        break;

    case QMetaType::Double :
        return QVariant((double) 0);
        break;

    case QMetaType::Long :
    case QMetaType::LongLong :
        return QVariant((qlonglong) 0);
        break;

    case QMetaType::Short :
        return QVariant((short) 0);
        break;

    case QMetaType::ULong :
    case QMetaType::ULongLong :
        return QVariant((qulonglong) 0);
        break;

    case QMetaType::UShort :
        return QVariant((unsigned short) 0);
        break;

    case QMetaType::Float :
        return QVariant((float) 0);
        break;

    case QMetaType::QUuid :
        return QUuid();
        break;

    case QMetaType::QUrl :
        return QUrl();
        break;

    case QMetaType::QString :
        return QString();
        break;

    case QMetaType::QChar :
        return QChar('X');
        break;

    case QMetaType::Char :
        return 'X';
        break;

    case QMetaType::SChar :
        return QVariant((signed char) 'X');
        break;

    case QMetaType::UChar :
        return QVariant((unsigned char) 'X');
        break;

    case QMetaType::QDate :
        return QDate::currentDate();
        break;

    case QMetaType::QTime :
        return QTime::currentTime();
        break;

    case QMetaType::QDateTime :
        return QDateTime::currentDateTime();
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
        qDebug() << "Returning an empty variant because the empty type is not supported for " << QMetaType(aType).name();
        return QVariant(QMetaType(aType));
        break;

    default :
        qDebug() << "Returning an empty variant because the default type was not expected for " << QMetaType(aType).name();
        return QVariant(QMetaType(aType));
        break;
    }
}
