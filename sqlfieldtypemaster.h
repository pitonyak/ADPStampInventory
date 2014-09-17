#ifndef SQLFIELDTYPEMASTER_H
#define SQLFIELDTYPEMASTER_H

#include "sqlfieldtype.h"

//**************************************************************************
//! Find the SqlFieldType that matches a specific DDL string.
/*!
 * The DDL string should begin with the type name. In other words,
 * the field name should have already been removed.
 *
 * This object contains a list of field types such as INTEGER and VARCHAR.
 * Each type is appropriately set for information such as the associated
 * QT MetaType, if it supports a length, and if it supports a precision.
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

    const SqlFieldType& findByName(const QString& name) const;

private:
    /*! \brief Initialized to contain all supported field types. */
    QList<SqlFieldType> m_fieldTypes;

    /*! \brief An instance of an invalid field that can be returned if nothing valid is found while searching. */
    SqlFieldType m_invalidField;
};

#endif // SQLFIELDTYPEMASTER_H
