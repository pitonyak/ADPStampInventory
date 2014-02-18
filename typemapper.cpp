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
    m_typeMapMetaToVariant.insert(QMetaType::UnknownType, QVariant::Invalid);
    m_typeMapMetaToVariant.insert(QMetaType::Bool, QVariant::Bool);
    m_typeMapMetaToVariant.insert(QMetaType::Int, QVariant::Int);
    m_typeMapMetaToVariant.insert(QMetaType::UInt, QVariant::UInt);
    m_typeMapMetaToVariant.insert(QMetaType::Double, QVariant::Double);
    m_typeMapMetaToVariant.insert(QMetaType::QChar, QVariant::Char);
    m_typeMapMetaToVariant.insert(QMetaType::QString, QVariant::String);
    m_typeMapMetaToVariant.insert(QMetaType::QByteArray, QVariant::ByteArray);

    m_typeMapMetaToVariant.insert(QMetaType::QBitArray, QVariant::BitArray);
    m_typeMapMetaToVariant.insert(QMetaType::QBitmap, QVariant::Bitmap);
    m_typeMapMetaToVariant.insert(QMetaType::QBrush, QVariant::Brush);
    m_typeMapMetaToVariant.insert(QMetaType::QColor, QVariant::Color);
    m_typeMapMetaToVariant.insert(QMetaType::QCursor, QVariant::Cursor);
    m_typeMapMetaToVariant.insert(QMetaType::QEasingCurve, QVariant::EasingCurve);
    m_typeMapMetaToVariant.insert(QMetaType::QUuid, QVariant::Uuid);
    m_typeMapMetaToVariant.insert(QMetaType::QModelIndex, QVariant::ModelIndex);
    m_typeMapMetaToVariant.insert(QMetaType::QFont, QVariant::Font);
    m_typeMapMetaToVariant.insert(QMetaType::QVariantHash, QVariant::Hash);
    m_typeMapMetaToVariant.insert(QMetaType::QIcon, QVariant::Icon);
    m_typeMapMetaToVariant.insert(QMetaType::QKeySequence, QVariant::KeySequence);
    m_typeMapMetaToVariant.insert(QMetaType::QLine, QVariant::Line);
    m_typeMapMetaToVariant.insert(QMetaType::QLineF, QVariant::LineF);
    m_typeMapMetaToVariant.insert(QMetaType::QVariantList, QVariant::List);
    m_typeMapMetaToVariant.insert(QMetaType::QLocale, QVariant::Locale);
    m_typeMapMetaToVariant.insert(QMetaType::QVariantMap, QVariant::Map);
    m_typeMapMetaToVariant.insert(QMetaType::QMatrix, QVariant::Matrix);
    m_typeMapMetaToVariant.insert(QMetaType::QTransform, QVariant::Transform);
    m_typeMapMetaToVariant.insert(QMetaType::QMatrix4x4, QVariant::Matrix4x4);
    m_typeMapMetaToVariant.insert(QMetaType::QPalette, QVariant::Palette);
    m_typeMapMetaToVariant.insert(QMetaType::QPen, QVariant::Pen);
    m_typeMapMetaToVariant.insert(QMetaType::QPixmap, QVariant::Pixmap);
    m_typeMapMetaToVariant.insert(QMetaType::QPoint, QVariant::Point);
    m_typeMapMetaToVariant.insert(QMetaType::QPointF, QVariant::PointF);
    m_typeMapMetaToVariant.insert(QMetaType::QPolygon, QVariant::Polygon);
    m_typeMapMetaToVariant.insert(QMetaType::QPolygonF, QVariant::PolygonF);
    m_typeMapMetaToVariant.insert(QMetaType::QQuaternion, QVariant::Quaternion);
    m_typeMapMetaToVariant.insert(QMetaType::QRect, QVariant::Rect);
    m_typeMapMetaToVariant.insert(QMetaType::QRectF, QVariant::RectF);
    m_typeMapMetaToVariant.insert(QMetaType::QRegExp, QVariant::RegExp);
    m_typeMapMetaToVariant.insert(QMetaType::QRegularExpression, QVariant::RegularExpression);
    m_typeMapMetaToVariant.insert(QMetaType::QRegion, QVariant::Region);
    m_typeMapMetaToVariant.insert(QMetaType::QSize, QVariant::Size);
    m_typeMapMetaToVariant.insert(QMetaType::QSizeF, QVariant::SizeF);
    m_typeMapMetaToVariant.insert(QMetaType::QSizePolicy, QVariant::SizePolicy);
    m_typeMapMetaToVariant.insert(QMetaType::QTextFormat, QVariant::TextFormat);
    m_typeMapMetaToVariant.insert(QMetaType::QTextLength, QVariant::TextLength);
    m_typeMapMetaToVariant.insert(QMetaType::QVector2D, QVariant::Vector2D);
    m_typeMapMetaToVariant.insert(QMetaType::QVector3D, QVariant::Vector3D);
    m_typeMapMetaToVariant.insert(QMetaType::QVector4D, QVariant::Vector4D);
    m_typeMapMetaToVariant.insert(QMetaType::User, QVariant::UserType);

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

    m_typeMapVariantToMeta.insert(QVariant::Invalid, QMetaType::UnknownType);
    m_typeMapVariantToMeta.insert(QVariant::Bool, QMetaType::Bool);
    m_typeMapVariantToMeta.insert(QVariant::Int, QMetaType::Int);
    m_typeMapVariantToMeta.insert(QVariant::UInt, QMetaType::UInt);
    m_typeMapVariantToMeta.insert(QVariant::Double, QMetaType::Double);
    m_typeMapVariantToMeta.insert(QVariant::Char, QMetaType::QChar);
    m_typeMapVariantToMeta.insert(QVariant::String, QMetaType::QString);
    m_typeMapVariantToMeta.insert(QVariant::ByteArray, QMetaType::QByteArray);

    //m_typeMapVariantToMeta.insert(QVariant::BitArray, QMetaType::QBitArray);
    //m_typeMapVariantToMeta.insert(QVariant::Bitmap, QMetaType::QBitmap);
    //m_typeMapVariantToMeta.insert(QVariant::Brush, QMetaType::QBrush);
    //m_typeMapVariantToMeta.insert(QVariant::Color, QMetaType::QColor);
    //m_typeMapVariantToMeta.insert(QVariant::Cursor, QMetaType::QCursor);

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
    m_MetaToName.insert(QMetaType::QRegExp,"RegExp");
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
    m_MetaToName.insert(QMetaType::QMatrix,"Matrix");
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

    m_VariantToName.insert(QVariant::Invalid, "Unknown");
    m_VariantToName.insert(QVariant::BitArray, "BitArray");
    m_VariantToName.insert(QVariant::Bitmap, "Bitmap");
    m_VariantToName.insert(QVariant::Bool, "Boolean");
    m_VariantToName.insert(QVariant::Brush, "Brush");
    m_VariantToName.insert(QVariant::ByteArray, "ByteArray");
    m_VariantToName.insert(QVariant::Char, "Char");
    m_VariantToName.insert(QVariant::Color, "Color");
    m_VariantToName.insert(QVariant::Cursor, "Cursor");
    m_VariantToName.insert(QVariant::Date, "Date");
    m_VariantToName.insert(QVariant::DateTime, "DateTime");
    m_VariantToName.insert(QVariant::Double, "ouble");
    m_VariantToName.insert(QVariant::EasingCurve, "EasingCurve");
    m_VariantToName.insert(QVariant::Uuid, "Uuid");
    m_VariantToName.insert(QVariant::ModelIndex, "ModelIndex");
    m_VariantToName.insert(QVariant::Font, "Font");
    m_VariantToName.insert(QVariant::Hash, "VariantHash");
    m_VariantToName.insert(QVariant::Icon, "Icon");
    m_VariantToName.insert(QVariant::Image, "Image");
    m_VariantToName.insert(QVariant::Int, "Int");
    m_VariantToName.insert(QVariant::KeySequence, "KeySequence");
    m_VariantToName.insert(QVariant::Line, "Line");
    m_VariantToName.insert(QVariant::LineF, "LineF");
    m_VariantToName.insert(QVariant::List, "VariantList");
    m_VariantToName.insert(QVariant::Locale, "Locale");
    m_VariantToName.insert(QVariant::LongLong, "longlong");
    m_VariantToName.insert(QVariant::Map, "VariantMap");
    m_VariantToName.insert(QVariant::Matrix, "Matrix");
    m_VariantToName.insert(QVariant::Transform, "Transform");
    m_VariantToName.insert(QVariant::Matrix4x4, "Matrix4x4");
    m_VariantToName.insert(QVariant::Palette, "Palette");
    m_VariantToName.insert(QVariant::Pen, "Pen");
    m_VariantToName.insert(QVariant::Pixmap, "Pixmap");
    m_VariantToName.insert(QVariant::Point, "Point");
    m_VariantToName.insert(QVariant::PointF, "PointF");
    m_VariantToName.insert(QVariant::Polygon, "Polygon");
    m_VariantToName.insert(QVariant::PolygonF, "PolygonF");
    m_VariantToName.insert(QVariant::Quaternion, "Quaternion");
    m_VariantToName.insert(QVariant::Rect, "Rect");
    m_VariantToName.insert(QVariant::RectF, "RectF");
    m_VariantToName.insert(QVariant::RegExp, "RegExp");
    m_VariantToName.insert(QVariant::RegularExpression, "RegularExpression");
    m_VariantToName.insert(QVariant::Region, "Region");
    m_VariantToName.insert(QVariant::Size, "Size");
    m_VariantToName.insert(QVariant::SizeF, "SizeF");
    m_VariantToName.insert(QVariant::SizePolicy, "SizePolicy");
    m_VariantToName.insert(QVariant::String, "String");
    m_VariantToName.insert(QVariant::StringList, "StringList");
    m_VariantToName.insert(QVariant::TextFormat, "TextFormat");
    m_VariantToName.insert(QVariant::TextLength, "TextLength");
    m_VariantToName.insert(QVariant::Time, "Time");
    m_VariantToName.insert(QVariant::UInt, "Unsigned Int");
    m_VariantToName.insert(QVariant::ULongLong, "Unsigned LongLong");
    m_VariantToName.insert(QVariant::Url, "Url");
    m_VariantToName.insert(QVariant::Vector2D, "Vector2D");
    m_VariantToName.insert(QVariant::Vector3D, "Vector3D");
    m_VariantToName.insert(QVariant::Vector4D, "Vector4D");
    m_VariantToName.insert(QVariant::UserType, "User");

    QMapIterator<QMetaType::Type, QString> metaNameIterator(m_MetaToName);
    while (metaNameIterator.hasNext()) {
        metaNameIterator.next();
        m_NameToMetaTo.insert(metaNameIterator.value().toLower(), metaNameIterator.key());
    }

    QMapIterator<QVariant::Type, QString> variantNameIterator(m_VariantToName);
    while (variantNameIterator.hasNext()) {
        variantNameIterator.next();
        m_NameToVariant.insert(variantNameIterator.value().toLower(), variantNameIterator.key());
    }

    // Fill in the variant to meta map where the variant type has no values.
    QMapIterator<QMetaType::Type, QVariant::Type> typeMapMetaToVariantIterator(m_typeMapMetaToVariant);
    while (typeMapMetaToVariantIterator.hasNext()) {
        typeMapMetaToVariantIterator.next();
        if (!m_typeMapVariantToMeta.contains(typeMapMetaToVariantIterator.value())) {
            m_typeMapVariantToMeta.insert(typeMapMetaToVariantIterator.value(), typeMapMetaToVariantIterator.key());
        }
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

