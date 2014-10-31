#ifndef SQLFIELDTYPE_H
#define SQLFIELDTYPE_H

#include <QVariant>
#include <QStringList>

//**************************************************************************
//! Used to track types in DDL.
/*!
 * Names are tested in the order that they are added to the list.
 * A name will match as "name(int1, int2)", "name(int1)", or "name".
 * No attempt has been made to make this efficient. Ease of coding was
 * put in front of efficiency. That can be fixed if this is seen as
 * a choke point.
 **************************************************************************/

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
     * \param qType QVariant type used to represent this SQL type.
     * \param supportsLen If true, the type may be written with (n) to specify the length.
     * \param supportsPrec If true, the type may be written with (precision, scale).
     * \param firstName a name added for matching.
     *
     ***************************************************************************/
    SqlFieldType(const QString& firstName, QMetaType::Type qType=QMetaType::Void, bool supportsLen=false, bool supportsPrec=false);

    //**************************************************************************
    //! constructor.
    /*!
     * \param qType QVariant type used to represent this SQL type.
     * \param supportsLen If true, the type may be written with (n) to specify the length.
     * \param supportsPrec If true, the type may be written with (precision, scale).
     * \param firstName a name added for matching.
     * \param secondName a name added for matching.
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
     * \param aName Name that can match the DDL. Names are matched in the order that they are added.
     *
     * \returns True if the name (case insensitive) is supported by this type.
     *
     ***************************************************************************/
    bool containsName(const QString& name) const;

    QStringList getSupportedNames() const { return m_supportedNames; }

private:
    QRegExp* firstMatchingRegExp(const QString& aName) const;

    bool m_supportsLength;
    bool m_supportsPrecision;
    QMetaType::Type m_qtVariantType;
    QStringList m_supportedNames;
};

#endif // SQLFIELDTYPE_H
