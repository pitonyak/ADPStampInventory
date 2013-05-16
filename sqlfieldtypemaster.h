#ifndef SQLFIELDTYPEMASTER_H
#define SQLFIELDTYPEMASTER_H

#include "sqlfieldtype.h"

//**************************************************************************
//! Find the SqlFieldType that matches a specific DDL string.
/*!
 * The DDL string should begin with the type name. In other words,
 * the field name should have already been removed.
 *
 **************************************************************************/

class SqlFieldTypeMaster
{
public:
    //**************************************************************************
    //! Default constructor.
    /*!
     *
     ***************************************************************************/
    SqlFieldTypeMaster();

    //**************************************************************************
    //! Find the SqlFieldType that matches a specific DDL string.
    /*!
     * \param DDL beginning with typename.
     * \returns The SqlFieldType that matches. If no match is found, return an empty type.
     *
     ***************************************************************************/
    const SqlFieldType& findMatch(const QString& ddl) const;

private:
    QList<SqlFieldType> m_fieldTypes;
    SqlFieldType m_invalidField;
};

#endif // SQLFIELDTYPEMASTER_H
