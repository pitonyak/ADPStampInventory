#ifndef SQLFIELDTYPE_H
#define SQLFIELDTYPE_H

#include "genericdataobjectfilter.h"
#include <QVariant>
#include <QStringList>
#include <QRegularExpression>
#include <QLoggingCategory>

//**************************************************************************
//! Used to track types in DDL.
/*!
 * Field Names are tested in the order that they are added to the list.
 * A name will match as "name(int1, int2)", "name(int1)", or "name".
 * No attempt has been made to make this efficient. Ease of coding was
 * put in front of efficiency. That can be fixed if this is seen as
 * a choke point.
 *
 * When this object is created, it is created listing supporting
 * SQL types. The first and second names are used where this might be
 * something like:
 *
 * SqlFieldType("NVARCHAR", "NATIONAL CHARACTER VARYING", QMetaType::QString, true, false)
 *
 * This causes two names to be added, NVARCHAR and NATIONAL CHARACTER VARYING.
 * We then indicate that this maps to a QString, it supports a length, but it does not support a precision.
 * The full list is as follows:
 *
 * "NVARCHAR", "NATIONAL CHARACTER VARYING", QMetaType::QString, true, false
 * "NCHAR", "NATIONAL CHARACTER", QMetaType::QString, true, false
 * "VARCHAR", "CHARACTER VARYING", QMetaType::QString, true, false
 * "CHARACTER", "CHAR", QMetaType::QChar, true, false
 * "BIT VARYING", "BIT", QMetaType::Bool, true, false
 * "TIMESTAMPTZ", "TIMESTAMP WITH TIME ZONE", QMetaType::QDateTime, false, false
 * "TIMESTAMP", QMetaType::QDateTime, false, false
 * "TIMETZ", "TIME WITH TIME ZONE", QMetaType::QTime, false, false
 * "TIME", QMetaType::QTime, false, false
 * "DATE", QMetaType::QDate, false, false
 * "INTEGER", QMetaType::Int, false, false
 * "SMALLINT",  QMetaType::Int, false, false
 * "FLOAT", "REAL", QMetaType::QString, false, false
 * "DOUBLE PRECISION", "DOUBLE", QMetaType::QString, false, false
 * "NUMERIC", "DECIMAL", QMetaType::QString, false, true
 *
 * Later, I can search a list of these to determine the type as obtained from DDL.
 *
 * \date 2011-2023
 **************************************************************************/

Q_DECLARE_LOGGING_CATEGORY(sqlFieldTypeCategory)

class SqlFieldType
{
public:
    //**************************************************************************
    //! Default constructor.
    /*!
     * \param qType QVariant type used to represent this SQL type.
     * \param supportsLen If true, the type may be written with (n) to specify the length.
     * \param supportsPrec If true, the type may be written with (precision, scale).
     *
     ***************************************************************************/
    SqlFieldType(QMetaType::Type qType=QMetaType::Void, bool supportsLen=false, bool supportsPrec=false);

    //**************************************************************************
    //! constructor.
    /*!
     * \param firstName a name added for matching.
     * \param qType QVariant type used to represent this SQL type.
     * \param supportsLen If true, the type may be written with (n) to specify the length.
     * \param supportsPrec If true, the type may be written with (precision, scale).
     *
     ***************************************************************************/
    SqlFieldType(const QString& firstName, QMetaType::Type qType=QMetaType::Void, bool supportsLen=false, bool supportsPrec=false);

    //**************************************************************************
    //! constructor.
    /*!
     * \param firstName a name added for matching.
     * \param secondName a name added for matching.
     * \param qType QVariant type used to represent this SQL type.
     * \param supportsLen If true, the type may be written with (n) to specify the length.
     * \param supportsPrec If true, the type may be written with (precision, scale).
     *
     ***************************************************************************/
    SqlFieldType(const QString& firstName, const QString& secondName, QMetaType::Type qType=QMetaType::Void, bool supportsLen=false, bool supportsPrec=false);

    SqlFieldType(const SqlFieldType& aType);
    SqlFieldType& operator=(const SqlFieldType& aType);

    //**************************************************************************
    //! Virtual destructor, which allows RTTI to work if needed.
    /*!
     *
     ***************************************************************************/
    virtual ~SqlFieldType();

    //**************************************************************************
    //! See if the DDL begins with a name that matches.
    /*!
     * \param ddl Matching DDL that begins with the type.
     * \returns True if there is a match for the name.
     *
     ***************************************************************************/
    bool ddlMatches(const QString& ddl) const;

    //**************************************************************************
    //! Extract the type name from matching DDL that begins with the type.
    /*!
     * \param ddl Matching DDL that begins with the type.
     * \returns The type name from matching DDL that begins with the type.
     *
     ***************************************************************************/
    QString name(const QString& ddl) const;

    //**************************************************************************
    //! Extract the full declaration for type and length from matching DDL that begins with the type.
    /*!
     * \param ddl Matching DDL that begins with the type.
     * \returns The full declaration for type and length from matching DDL that begins with the type.
     *
     ***************************************************************************/
    QString declaration(const QString& ddl) const;

    QString stripNameAndLength(const QString& ddl) const;

    //**************************************************************************
    //! Extract the length from a matching DDL that begins with the type.
    /*!
     * \param ddl Matching DDL that begins with the type.
     * \returns The length from matching DDL that begins with the type or -1 if it is not specified.
     *
     ***************************************************************************/
    int length(const QString& ddl) const;

    //**************************************************************************
    //! Get the QMetaType::Type type used to represent this SQL type.
    /*!
     * \returns QMetaType::Type type used to represent this SQL type.
     *
     ***************************************************************************/
    QMetaType::Type qtType() const;

    //**************************************************************************
    //! Set the QMetaType::Type type used to represent this SQL type.
    /*!
     * \param qType QMetaType::Type type used to represent this SQL type.
     *
     ***************************************************************************/
    void qtType(QMetaType::Type qType);

    //**************************************************************************
    //! If true, the type may be written with (n) to specify the length.
    /*!
     * \returns
     *
     ***************************************************************************/
    bool supportsLength() const;

    //**************************************************************************
    //! Set if the type may be written with (n) to specify the length.
    /*!
     * \param supportsLen If true, the type may be written with (n) to specify the length.
     *
     ***************************************************************************/
    void supportsLength(bool supportsLen);

    //**************************************************************************
    //! If true, the DDL may be written with (precision, scale).
    /*!
     * \returns True if the DDL may be written with (precision, scale).
     *
     ***************************************************************************/
    bool supportsPrecision() const;

    //**************************************************************************
    //! Sets if the DDL may be written with (precision, scale).
    /*!
     * \param supportsPrec
     *
     ***************************************************************************/
    void supportsPrecision(bool supportsPrec);

    //**************************************************************************
    //! Add a name that can be used to match the DDL.
    /*!
     * The name is "simplified" before adding, which means that leading and trailing white space is removed.
     * Runs of white space are replaced by a single space.
     *
     * \param aName Name that can match the DDL. Names are matched in the order that they are added.
     *
     ***************************************************************************/
    void addName(const QString& aName);

    //**************************************************************************
    //! Check to see if the type is valid.
    /*!
     * \returns True if the type is a valid variant type.
     *
     ***************************************************************************/
    bool isValid();

    //**************************************************************************
    //! Determine if the provided name is supported by this type object.
    /*!
     * In other words, see if this contains the simplified name.
     *
     * \param aName Name that can match the DDL. Names are matched in the order that they are added.
     *
     * \returns True if the name (case insensitive) is supported by this type.
     *
     ***************************************************************************/
    bool containsName(const QString& name) const;

    QStringList getSupportedNames() const { return m_supportedNames; }

    QString getFirstSupportedName() const { return m_supportedNames.size() > 0 ? m_supportedNames.first() : "Unknown"; }

private:
    //**************************************************************************
    //! The argument is a DDL type and and the regex that provides a match is returned.
    /*!
     * Search the list of supported names. Supported names may contain a space, but the space is converted to a regular expression for one or more spaces.
     *
     * Check to see if the provided name is some variant of the supported name.
     *
     * \param aName Search the list of supported names for aName.
     *
     * \returns Regular expression
     *
     ***************************************************************************/
    QRegularExpression* firstMatchingRegExp(const QString& aName) const;

    bool m_supportsLength;
    bool m_supportsPrecision;
    QMetaType::Type m_qtMetaType;

    // List of supported field names
    QStringList m_supportedNames;
};

#endif // SQLFIELDTYPE_H
