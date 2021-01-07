#include "genericdataobject.h"
#include "genericdatacollection.h"
#include "sqlfieldtype.h"
#include "typemapper.h"

#include <QUuid>
#include <QSqlQuery>

GenericDataObject::GenericDataObject(QObject *parent) :
  QObject(parent)
{
}

GenericDataObject::GenericDataObject(const GenericDataObject& obj, QObject *parent) : QObject(parent)
{
  GenericDataObject::operator=(obj);
}

QString GenericDataObject::getString(const QString& name) const
{
  QString lower = name.toLower();
  return containsValueNoCase(lower) ? m_properties.value(lower).toString() : "";
}

QString GenericDataObject::getString(const QString& name, const QString& defaultValue) const
{
  QString lower = name.toLower();
  return containsValueNoCase(lower) ? m_properties.value(lower).toString() : defaultValue;
}


int GenericDataObject::getInt(const QString& name, const int defaultValue) const
{
  QString lower = name.toLower();
  if (containsValueNoCase(lower))
  {
    bool ok = false;
    int i = m_properties.value(lower).toInt(&ok);
    if (ok)
    {
      return i;
    }
  }
  return defaultValue;
}

double GenericDataObject::getDouble(const QString& name, const double defaultValue) const
{
  QString lower = name.toLower();
  if (containsValueNoCase(lower))
  {
    bool ok = false;
    double i = m_properties.value(lower).toDouble(&ok);
    if (ok)
    {
      return i;
    }
  }
  return defaultValue;
}

QDate GenericDataObject::getDate(const QString& name) const
{
  QString lower = name.toLower();
  return containsValueNoCase(lower) ? m_properties.value(lower).toDate() : QDate::currentDate();
}

QDate GenericDataObject::getDate(const QString& name, const QDate& defaultValue) const
{
  QString lower = name.toLower();
  return containsValueNoCase(lower) ? m_properties.value(lower).toDate() : defaultValue;
}


QDateTime GenericDataObject::getDateTime(const QString& name) const
{
  QString lower = name.toLower();
  return containsValueNoCase(lower) ? m_properties.value(lower).toDateTime() : QDateTime::currentDateTime();
}

QDateTime GenericDataObject::getDateTime(const QString& name, const QDateTime& defaultValue) const
{
  QString lower = name.toLower();
  return containsValueNoCase(lower) ? m_properties.value(lower).toDateTime() : defaultValue;
}


QTime GenericDataObject::getTime(const QString& name) const
{
  QString lower = name.toLower();
  return containsValueNoCase(lower) ? m_properties.value(lower).toTime() : QTime::currentTime();
}

QTime GenericDataObject::getTime(const QString& name, const QTime& defaultValue) const
{
  QString lower = name.toLower();
  return containsValueNoCase(lower) ? m_properties.value(lower).toTime() : defaultValue;
}

const GenericDataObject& GenericDataObject::operator=(const GenericDataObject& obj)
{
  // Note that the parent object is NOT copied.
  if (this != &obj)
  {
    m_properties.clear();
    QHashIterator<QString, QVariant> i(obj.m_properties);
    while (i.hasNext())
    {
      i.next();
      m_properties.insert(i.key(), i.value());
    }
  }
  return *this;
}

bool GenericDataObject::lessThan(const GenericDataObject& obj, const QList<TableSortField *> &sortFields) const
{
    return compare(obj, sortFields) < 0;
}

int GenericDataObject::compare(const GenericDataObject& obj, const QList<TableSortField *> &sortFields) const
{
    int rc = 0;
    QListIterator<TableSortField*> i(sortFields);
    while (rc == 0 && i.hasNext())
    {
        const TableSortField* sortField = i.next();
        if (sortField != nullptr)
        {
            if (!obj.containsValueNoCase(sortField->fieldName()))
            {
                if (containsValueNoCase(sortField->fieldName()))
                {
                    return sortField->isAscending() ? 1 : -1;
                }
            }
            else if (!containsValueNoCase(sortField->fieldName()))
            {
                return sortField->isAscending() ? -1 : 1;
            }
            else
            {
                QVariant v1 = getValue(sortField->fieldName());
                QVariant v2 = obj.getValue(sortField->fieldName());
                rc = sortField->valueCompare(v1, v2);
            }
        }
    }
    return rc;
}

int GenericDataObject::compare(const GenericDataObject& obj, const QStringList& fields, Qt::CaseSensitivity sensitive) const
{
    int rc = 0;
    for (int i=0; rc==0 && i<fields.size(); ++i)
    {
        if (!obj.containsValueNoCase(fields.at(i)))
        {
            if (containsValueNoCase(fields.at(i)))
            {
                return 1;
            }
        }
        else if (!containsValueNoCase(fields.at(i)))
        {
            return -1;
        }
        else
        {
            QVariant v1 = getValue(fields.at(i));
            QVariant v2 = obj.getValue(fields.at(i));
            if (v1.metaType().id() != v2.metaType().id())
            {
                // This should never happen.
                qDebug() << "Comparing two variants of different types in column : " << fields.at(i);
                rc = v1.toString().compare(v2.toString(), sensitive);
            }
            else if (v1 != v2)
            {
                switch (v1.metaType().id())
                {
                case QMetaType::Int:
                    rc = v1.toInt() < v2.toInt() ? -1 : 1;
                    break;
                case QMetaType::QString:
                    rc = v1.toString().compare(v2.toString(), sensitive);
                    break;
                case QMetaType::Double:
                    rc = v1.toDouble() < v2.toDouble() ? -1 : 1;
                    break;
                case QMetaType::QDate:
                    rc = v1.toDate() < v2.toDate() ? -1 : 1;
                    break;
                case QMetaType::QDateTime:
                    rc = v1.toDateTime() < v2.toDateTime() ? -1 : 1;
                    break;
                case QMetaType::LongLong:
                    rc = v1.toLongLong() < v2.toLongLong() ? -1 : 1;
                    break;
                case QMetaType::ULongLong:
                    rc = v1.toULongLong() < v2.toULongLong() ? -1 : 1;
                    break;
                case QMetaType::UInt:
                    rc = v1.toUInt() < v2.toUInt() ? -1 : 1;
                    break;
                case QMetaType::QTime:
                    rc = v1.toTime() < v2.toTime() ? -1 : 1;
                    break;
                case QMetaType::QUuid:
                    rc = v1.toUuid() < v2.toUuid() ? -1 : 1;
                    break;
                case QMetaType::Char:
                    rc = v1.toChar() < v2.toChar() ? -1 : 1;
                    break;
                case QMetaType::Bool:
                    rc = v1.toBool() < v2.toBool() ? -1 : 1;
                    break;
                default:
                    qDebug() << "Comparing two unsupported types in column : " << fields.at(i);
                    rc = v1.toString().compare(v2.toString(), sensitive);
                    break;
                }
            }
        }
    }
    return rc;
}

bool GenericDataObject::isDateTime(const QString& name) const
{
  const QVariant v = m_properties.value(name.toLower());
  return (QMetaType::QDateTime == v.metaType().id());
}

bool GenericDataObject::isDate(const QString& name) const
{
  const QVariant v = m_properties.value(name.toLower());
  return (QMetaType::QDate == v.metaType().id());
}

bool GenericDataObject::isTime(const QString& name) const
{
  const QVariant v = m_properties.value(name.toLower());
  return (QMetaType::QTime == v.metaType().id());
}

bool GenericDataObject::increment(const QString& name, const double incValue, QVariant& variantValue)
{
  const QVariant v = m_properties.value(name.toLower());
  //qDebug() << "GenericDataObject::increment name:" << name << " value:" << v << " add " << incValue;
  if (incValue == 0.0) {
    variantValue = v;
  } else if (QMetaType::QDateTime == v.metaType().id()) {
    variantValue = QVariant(v.toDateTime().addDays(incValue));
  } else if (QMetaType::QDate == v.metaType().id()) {
    variantValue = QVariant(v.toDate().addDays(incValue));
  } else if (QMetaType::QTime == v.metaType().id()) {
    variantValue = QVariant(v.toTime().addSecs(incValue));
  } else if (QMetaType::Int == v.metaType().id() ||
             QMetaType::Short == v.metaType().id()) {
    variantValue.setValue(v.toInt() + static_cast<int>(incValue));
  } else if (QMetaType::UInt == v.metaType().id() ||
             QMetaType::UShort == v.metaType().id()) {
    variantValue = QVariant(v.toUInt() + static_cast<uint>(incValue));
  } else if (QMetaType::Long == v.metaType().id() ||
             QMetaType::LongLong == v.metaType().id()) {
    variantValue = QVariant(v.toLongLong() + static_cast<qlonglong>(incValue));
  } else if (QMetaType::ULong == v.metaType().id() ||
             QMetaType::ULongLong == v.metaType().id()) {
    variantValue = QVariant(v.toULongLong() + static_cast<qulonglong>(incValue));
  } else if (QMetaType::Double == v.metaType().id()) {
    variantValue = QVariant(v.toDouble() + incValue);
  } else if (QMetaType::Float == v.metaType().id()) {
    variantValue = QVariant(v.toFloat() + incValue);
  } else {
    qDebug() << "GenericDataObject::increment unsupported type:" << v.metaType().id();
    return false;
  }
  //qDebug() << "Before leaving v = " << v << " and variantValue = " << variantValue;
  return true;
}

bool GenericDataObject::increment(const QString& name, const int incValue, QVariant& variantValue)
{
  return increment(name, static_cast<double>(incValue), variantValue);
}

const QVariant GenericDataObject::getValue(const QString& name) const
{
  return m_properties.value(name.toLower());
}

bool GenericDataObject::fieldNameMeansDate(const QString &name)
{
  return (name.compare("purchasedate", Qt::CaseInsensitive) == 0 ||
      name.compare("releasedate", Qt::CaseInsensitive) == 0);
}

bool GenericDataObject::fieldNameMeansDateTime(const QString &name)
{
  return (name.compare("updated", Qt::CaseInsensitive) == 0);
}

bool GenericDataObject::setBindValue(QSqlQuery& query, const QString& paramName, const QString& fieldName, const SqlFieldType& fieldType, bool missingMeansNull) const
{
    QString lower = fieldName.toLower();
    if (containsValueNoCase(lower)) {
        query.bindValue(paramName, m_properties.value(lower));
    } else {
        if (missingMeansNull) {
            TypeMapper mapper;
            query.bindValue(paramName, QVariant(QMetaType(fieldType.qtType())));
        } else {
            // Go for default values
            switch (fieldType.qtType()) {
            case QMetaType::Bool :
                query.bindValue(paramName, false);
                break;

            case QMetaType::Int :
            case QMetaType::UInt :
            case QMetaType::Double :
            case QMetaType::Long :
            case QMetaType::LongLong :
            case QMetaType::Short :
            case QMetaType::ULong :
            case QMetaType::ULongLong :
            case QMetaType::UShort :
            case QMetaType::Float :
                query.bindValue(paramName, 0);
                break;

            case QMetaType::QUuid :
            case QMetaType::QUrl :
            case QMetaType::QString :
                query.bindValue(paramName, "");
                break;

            case QMetaType::QChar :
            case QMetaType::Char :
            case QMetaType::SChar :
            case QMetaType::UChar :
                query.bindValue(paramName, " ");
                break;

            case QMetaType::QDate :
                query.bindValue(paramName, QDate::currentDate());
                break;

            case QMetaType::QTime :
                query.bindValue(paramName, QTime::currentTime());
                break;

            case QMetaType::QDateTime :
                query.bindValue(paramName, QDateTime::currentDateTime());
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
                return false;
                break;

            default :
                return false;
                break;
            }
        }
    }
    return true;
}


