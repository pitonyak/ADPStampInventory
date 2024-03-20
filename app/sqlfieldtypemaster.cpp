#include "sqlfieldtypemaster.h"

SqlFieldTypeMaster::SqlFieldTypeMaster()
{
    // Order is crucial here because the list is searched in order.
    // If CHAR is first, it will match even if the type is CHARACTER VARYING.

    // A BIT Varying field may have a length greater than 1, in which case, a string should be used rather than a boolean value.
    // This assumes that a BIT is not varying, and uses a boolean instead.

    m_fieldTypes.append(SqlFieldType("NVARCHAR", "NATIONAL CHARACTER VARYING", QMetaType::QString, true, false));
    m_fieldTypes.append(SqlFieldType("NCHAR", "NATIONAL CHARACTER", QMetaType::QString, true, false));
    m_fieldTypes.append(SqlFieldType("VARCHAR", "CHARACTER VARYING", QMetaType::QString, true, false));
    m_fieldTypes.append(SqlFieldType("CHARACTER", "CHAR", QMetaType::QChar, true, false));
    m_fieldTypes.append(SqlFieldType("BIT VARYING", "BIT", QMetaType::Bool, true, false));
    //m_fieldTypes.append(SqlFieldType("BIT VARYING", "BIT", QMetaType::QString, true, false));
    m_fieldTypes.append(SqlFieldType("TIMESTAMPTZ", "TIMESTAMP WITH TIME ZONE", QMetaType::QDateTime, false, false));
    m_fieldTypes.append(SqlFieldType("TIMESTAMP", QMetaType::QDateTime, false, false));
    m_fieldTypes.append(SqlFieldType("TIMETZ", "TIME WITH TIME ZONE", QMetaType::QTime, false, false));
    m_fieldTypes.append(SqlFieldType("TIME", QMetaType::QTime, false, false));
    m_fieldTypes.append(SqlFieldType("DATE", QMetaType::QDate, false, false));
    m_fieldTypes.append(SqlFieldType("INTEGER", QMetaType::Int, false, false));
    m_fieldTypes.append(SqlFieldType("SMALLINT",  QMetaType::Int, false, false));
    m_fieldTypes.append(SqlFieldType("FLOAT", "REAL", QMetaType::QString, false, false));
    m_fieldTypes.append(SqlFieldType("DOUBLE PRECISION", "DOUBLE", QMetaType::QString, false, false));
    m_fieldTypes.append(SqlFieldType("NUMERIC", "DECIMAL", QMetaType::QString, false, true));
}

const SqlFieldType& SqlFieldTypeMaster::findMatch(const QString& ddl) const
{
    QString simplified = ddl.simplified();
    for (int i=0; i<m_fieldTypes.size(); ++i)
    {
        if (m_fieldTypes.at(i).ddlMatches(simplified))
        {
            return m_fieldTypes.at(i);
        }
    }
    return m_invalidField;
}

const SqlFieldType& SqlFieldTypeMaster::findByName(const QString& name) const
{
    QString simplified = name.simplified();
    for (int i=0; i<m_fieldTypes.size(); ++i)
    {
        if (m_fieldTypes.at(i).ddlMatches(simplified))
        {
            return m_fieldTypes.at(i);
        }
    }
    return m_invalidField;
}
