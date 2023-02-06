#include "sqlfieldtype.h"


Q_LOGGING_CATEGORY(sqlFieldTypeCategory, "andy.sqlfieldtype")

SqlFieldType::SqlFieldType(QMetaType::Type qType, bool supportsLen, bool supportsPrec) :
    m_supportsLength(supportsLen),
    m_supportsPrecision(supportsPrec),
    m_qtMetaType(qType)
{
}


SqlFieldType::SqlFieldType(const QString& firstName, QMetaType::Type qType, bool supportsLen, bool supportsPrec) :
    m_supportsLength(supportsLen),
    m_supportsPrecision(supportsPrec),
    m_qtMetaType(qType)
{
    addName(firstName);
}


SqlFieldType::SqlFieldType(const QString& firstName, const QString& secondName, QMetaType::Type qType, bool supportsLen, bool supportsPrec) :
    m_supportsLength(supportsLen),
    m_supportsPrecision(supportsPrec),
    m_qtMetaType(qType)
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
    QScopedPointer<QRegularExpression> regExp(firstMatchingRegExp(ddl));
    return regExp ? true : false;
}

QString SqlFieldType::name(const QString& ddl) const
{
    QScopedPointer<QRegularExpression> regExp(firstMatchingRegExp(ddl));
    if (!regExp)
    {
        return "";
    }

    return regExp->match(ddl).captured(1);
}
QString SqlFieldType::declaration(const QString& ddl) const
{
    QScopedPointer<QRegularExpression> regExp(firstMatchingRegExp(ddl));
    if (!regExp)
    {
        return "";
    }
    return regExp->match(ddl).captured(0);
}

int SqlFieldType::length(const QString& ddl) const
{
    QScopedPointer<QRegularExpression> regExp(firstMatchingRegExp(ddl));
    if (!regExp)
    {
        return -1;
    }

    return regExp->captureCount() > 1 ? regExp->match(ddl).captured(2).toInt() : -1;
}

QString SqlFieldType::stripNameAndLength(const QString& ddl) const
{
    QScopedPointer<QRegularExpression> regExp(firstMatchingRegExp(ddl));
    if (!regExp)
    {
        return ddl;
    }
    QString temp = ddl;
    return temp.replace(*regExp, "");
}

QMetaType::Type SqlFieldType::qtType() const
{
    return m_qtMetaType;
}

void SqlFieldType::qtType(QMetaType::Type qType)
{
    m_qtMetaType = qType;
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

QRegularExpression* SqlFieldType::firstMatchingRegExp(const QString& aName) const
{
    // Three regular expressions are used / tested.
    // Each regular expression is some variant on white spaces and numbers.
    // Initial testing shows that I am only testing against things such as "VARCHAR"
    const char* bases[] = {"\\(\\s*(-?\\d+)\\s*,\\s*(-?\\d+)\\s*\\)\\s*", "\\(\\s*(-?\\d+)\\s*\\)\\s*", ""};
    for (int iName=0; iName<m_supportedNames.size(); ++iName)
    {
        QString aSupportedName = m_supportedNames.at(iName);
        QString regExpStr_base = QString("^\\s*(%1)\\s*").arg(aSupportedName.replace(' ', "\\s+"));
        for (int iBase=0; iBase<3; ++iBase)
        {
            QRegularExpression regExp(regExpStr_base + bases[iBase]);
            //qDebug(sqlFieldTypeCategory) << "Testing Regular Expression (2) " << regExp.pattern();
            regExp.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
            if (!regExp.isValid()) {
              // TODO: Do something
              qWarning(sqlFieldTypeCategory) << "Invalid Regular Expression " << regExp.pattern();
            }
            QRegularExpressionMatch match = regExp.match(aName);
            if (match.hasMatch()) {
              //qDebug() << "Matched (" << aName << ") to " << "(" << match.capturedTexts().join(")(") << ")";
              return new QRegularExpression(regExpStr_base + bases[iBase]);
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
    // Remove leading and trailing white space.
    // Runs of white space replaced with a single space.
    QString simplifiedName = aName.simplified();
    return !simplifiedName.isEmpty() && !simplifiedName.isNull() && m_supportedNames.contains(simplifiedName, Qt::CaseInsensitive);
}


bool SqlFieldType::isValid()
{
    return m_qtMetaType != QMetaType::Void;
}
