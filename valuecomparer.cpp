#include "valuecomparer.h"
#include <QVariant>
#include <QTime>
#include <QDateTime>
#include <QDate>
#include <QString>
#include <QStringList>

ValueComparer::ValueComparer(Qt::CaseSensitivity caseSensitive) : m_caseSensitivity(caseSensitive)
{
}

ValueComparer::ValueComparer(const ValueComparer& obj) : m_caseSensitivity(obj.m_caseSensitivity)
{
}

ValueComparer::~ValueComparer()
{

}

const ValueComparer& ValueComparer::operator=(const ValueComparer& obj)
{
    if (this != &obj)
    {
        setSensitivity(obj.caseSensitivity());
    }
    return *this;
}

int ValueComparer::valueCompare(const QStringList& v1, const QStringList& v2) const
{
    int rc = 0;
    for (int i=0; rc == 0 && i<v1.count() && i<v2.count(); ++i)
    {
        rc = v1.at(i).compare(v2.at(i), m_caseSensitivity);
    }
    if (rc == 0 && v1.count() != v2.count())
    {
        rc = v1.count() < v2.count() ? -1 : 1;
    }
    return rc;
}

int ValueComparer::valueCompare(const QVariant& v1, const QVariant& v2) const
{
    if (v1 == v2)
    {
        return 0;
    }
    if (v1.type() == v2.type())
    {
        switch ((QMetaType::Type)(v1.type()))
        {
        case QMetaType::QString :
        case QMetaType::QUrl :
        case QMetaType::QUuid :
            return v1.toString().compare(v2.toString(), m_caseSensitivity);
            break;
        case QMetaType::Bool :
            return v1.toBool() < v2.toBool() ? -1 : 1;
            break;
        case QMetaType::Char :
        case QMetaType::QChar :
        case QMetaType::UChar :
        case QMetaType::SChar :
            if (m_caseSensitivity == Qt::CaseInsensitive)
            {
                return v1.toString().compare(v2.toString(), m_caseSensitivity);
            }
            else
            {
                return v1.toChar() < v2.toChar() ? -1 : 1;
            }
            break;
        case QMetaType::QDate :
            return v1.toDate() < v2.toDate() ? -1 : 1;
            break;
        case QMetaType::QDateTime :
            return v1.toDateTime() < v2.toDateTime() ? -1 : 1;
            break;
        case QMetaType::QTime :
            return v1.toDateTime() < v2.toDateTime() ? -1 : 1;
            break;
        case QMetaType::Double :
        case QMetaType::Float :
            return v1.toDouble() < v2.toDouble() ? -1 : 1;
            break;
        case QMetaType::Int :
        case QMetaType::Short :
            return v1.toInt() < v2.toInt() ? -1 : 1;
            break;
        case QMetaType::UInt :
        case QMetaType::UShort :
            return v1.toUInt() < v2.toUInt() ? -1 : 1;
            break;
        case QMetaType::ULongLong :
        case QMetaType::ULong :
            return v1.toULongLong() < v2.toULongLong() ? -1 : 1;
            break;
        case QMetaType::LongLong :
        case QMetaType::Long :
            return v1.toLongLong() < v2.toLongLong() ? -1 : 1;
            break;
        case QMetaType::QStringList :
            return valueCompare(v1.toStringList(), v2.toStringList());
            break;
        default:
            return 0;
            break;

        }
    }
    // Different types, so this is dangerous!
    return v1.toString().compare(v2.toString(), m_caseSensitivity);
}
