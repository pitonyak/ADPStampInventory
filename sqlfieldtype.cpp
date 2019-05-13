#include "sqlfieldtype.h"

SqlFieldType::SqlFieldType(QMetaType::Type qType, bool supportsLen, bool supportsPrec) :
    m_supportsLength(supportsLen),
    m_supportsPrecision(supportsPrec),
    m_qtVariantType(qType)
{
}


SqlFieldType::SqlFieldType(const QString& firstName, QMetaType::Type qType, bool supportsLen, bool supportsPrec) :
    m_supportsLength(supportsLen),
    m_supportsPrecision(supportsPrec),
    m_qtVariantType(qType)
{
    addName(firstName);
}


SqlFieldType::SqlFieldType(const QString& firstName, const QString& secondName, QMetaType::Type qType, bool supportsLen, bool supportsPrec) :
    m_supportsLength(supportsLen),
    m_supportsPrecision(supportsPrec),
    m_qtVariantType(qType)
{
    addName(firstName);
    addName(secondName);
}

SqlFieldType::SqlFieldType(const SqlFieldType& aType)
{
    operator=(aType);
}

SqlFieldType& SqlFieldType::operator=(const SqlFieldType& aType)
{
    if (this != &aType)
    {
        supportsLength(aType.supportsLength());
        supportsPrecision(aType.supportsPrecision());
        qtType(aType.qtType());
        m_supportedNames.clear();
        m_supportedNames.append(aType.m_supportedNames);
    }
    return *this;
}


SqlFieldType::~SqlFieldType()
{
}

bool SqlFieldType::ddlMatches(const QString& ddl) const
{
    QScopedPointer<QRegExp> regExp(firstMatchingRegExp(ddl));
    return regExp ? true : false;
}

QString SqlFieldType::name(const QString& ddl) const
{
    QScopedPointer<QRegExp> regExp(firstMatchingRegExp(ddl));
    if (!regExp)
    {
        return "";
    }
    return regExp->cap(1);
}
QString SqlFieldType::declaration(const QString& ddl) const
{
    QScopedPointer<QRegExp> regExp(firstMatchingRegExp(ddl));
    if (!regExp)
    {
        return "";
    }
    return regExp->cap(0);
}

int SqlFieldType::length(const QString& ddl) const
{
    QScopedPointer<QRegExp> regExp(firstMatchingRegExp(ddl));
    if (!regExp)
    {
        return -1;
    }
    return regExp->captureCount() > 1 ? regExp->cap(2).toInt() : -1;
}

QString SqlFieldType::stripNameAndLength(const QString& ddl) const
{
    QScopedPointer<QRegExp> regExp(firstMatchingRegExp(ddl));
    if (!regExp)
    {
        return ddl;
    }
    QString temp = ddl;
    return temp.replace(*regExp, "");
}

QMetaType::Type SqlFieldType::qtType() const
{
    return m_qtVariantType;
}

void SqlFieldType::qtType(QMetaType::Type qType)
{
    m_qtVariantType = qType;
}

bool SqlFieldType::supportsLength() const
{
    return m_supportsLength;
}

void SqlFieldType::supportsLength(bool supportsLen)
{
    m_supportsLength = supportsLen;
}

bool SqlFieldType::supportsPrecision() const
{
    return m_supportsPrecision;
}
void SqlFieldType::supportsPrecision(bool supportsPrec)
{
    m_supportsPrecision = supportsPrec;
}

QRegExp* SqlFieldType::firstMatchingRegExp(const QString& aName) const
{
    const char* bases[] = {"\\(\\s*(-?\\d+)\\s*,\\s*(-?\\d+)\\s*\\)\\s*", "\\(\\s*(-?\\d+)\\s*\\)\\s*", ""};
    for (int iName=0; iName<m_supportedNames.size(); ++iName)
    {
        QString temp = m_supportedNames.at(iName);
        QString regExpStr_base = QString("^\\s*(%1)\\s*").arg(temp.replace(' ', "\\s+"));
        for (int iBase=0; iBase<3; ++iBase)
        {
            QRegExp regExp(regExpStr_base + bases[iBase]);
            regExp.setCaseSensitivity(Qt::CaseInsensitive);
            if (regExp.indexIn(aName) >= 0)
            {
                return new QRegExp(regExpStr_base + bases[iBase]);
            }
        }
    }
    return nullptr;
}


void SqlFieldType::addName(const QString& aName)
{
    QString simplifiedName = aName.simplified();
    if (!simplifiedName.isEmpty() && !simplifiedName.isNull() && !m_supportedNames.contains(simplifiedName, Qt::CaseInsensitive))
    {
        m_supportedNames.append(simplifiedName);
    }
}

bool SqlFieldType::containsName(const QString& aName) const
{
    QString simplifiedName = aName.simplified();
    return !simplifiedName.isEmpty() && !simplifiedName.isNull() && m_supportedNames.contains(simplifiedName, Qt::CaseInsensitive);
}


bool SqlFieldType::isValid()
{
    return m_qtVariantType != QMetaType::Void;
}
