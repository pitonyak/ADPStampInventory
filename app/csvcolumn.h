#ifndef CSVCOLUMN_H
#define CSVCOLUMN_H

#include <QObject>
#include <QVariant>
#include "typemapper.h"

//**************************************************************************
/*! \class CSVColumn
 * \brief Represent a single column in a single row.
 * This class stores the value as a string and remembers if it is "qualified" (was surrounded by quotes).
 * This object is also able to "guess" the type of this particular cell.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2011-2020
 **************************************************************************/

class CSVColumn : public QObject
{
    Q_OBJECT
public:
    //**************************************************************************
    //! Constructor
    /*!
     * \param parent The object's owner. The parent's destructor destroys all child objects.
     *
     ***************************************************************************/
    explicit CSVColumn(QObject *parent = nullptr);

    //**************************************************************************
    //! Constructor
    /*!
     * \param value Set the value as a string for this entry.
     * \param parent The object's owner. The parent's destructor destroys all child objects.
     *
     ***************************************************************************/
    explicit CSVColumn(const QString& value, QObject *parent = nullptr);

    //**************************************************************************
    //! Constructor
    /*!
     * \param value Set the value as a string for this entry.
     * \param qualified If true, the entry was (or will be) surrounded by qualifiers (double quotes).
     * \param colType Column type.
     * \param parent The object's owner. The parent's destructor destroys all child objects.
     *
     ***************************************************************************/
    explicit CSVColumn(const QString& value, bool qualified=true, QMetaType::Type colType=QMetaType::Void, QObject *parent = nullptr);

    //**************************************************************************
    //! Constructor
    /*!
     * \param x Object to copy into the newly constructed object.
     * \param parent The object's owner. The parent's destructor destroys all child objects.
     *
     ***************************************************************************/
    CSVColumn(const CSVColumn& x, QObject *parent = nullptr);

    //**************************************************************************
    //! If true, the entry was (or will be) surrounded by qualifiers (double quotes).
    /*!
     * \returns True if the entry was (or will be) surrounded by qualifiers (double quotes).
     *
     ***************************************************************************/
    bool isQualified() const;

    //**************************************************************************
    //! Set if the object is (or will be) surrounded by qualifiers.
    /*!
     * \param qualified
     *
     ***************************************************************************/
    void setQualified(bool qualified=true);

    //**************************************************************************
    //! Get the column type.
    /*!
     * \returns Column type, or Invalid if not known or set.
     *
     ***************************************************************************/
    QMetaType::Type getType() const;

    QString getTypeAsString() const;
    QString getQualifiedAsString() const;

    //**************************************************************************
    //! Set the column type.
    /*!
     * \param colType
     *
     ***************************************************************************/
    void setType(QMetaType::Type colType=QMetaType::Void);

    //**************************************************************************
    //! Get the column value as a string.
    /*!
     * \returns Column value as a string.
     *
     ***************************************************************************/
    QString getValue() const;

    //**************************************************************************
    //! Set the column value as a string.
    /*!
     * \param value
     *
     ***************************************************************************/
    void setValue(const QString& value);

    const CSVColumn& operator=(const CSVColumn& x);
    const CSVColumn& copyFrom(const CSVColumn& x);

    // Support more types for set value; this used to be marked as a "to do task".
    // The only reason to support setting by more types is so that code can
    // set a value as say a float without converting it to a string. It might also be possible
    // to then keep type information for that object. Until I have need, however, do not bother.

    QString toString(bool brief=true) const;
    QVariant toVariant() const;

    static QMetaType::Type guessType(const QString& s, const TypeMapper::ColumnConversionPreferences preference);

signals:

public slots:

private:
    void setDefaults();

    bool m_qualified;
    QMetaType::Type m_type;
    QString m_value;
    static TypeMapper m_typeMap;
};

inline bool CSVColumn::isQualified() const
{
    return m_qualified;
}

inline void CSVColumn::setQualified(bool qualified)
{
    m_qualified = qualified;
}

inline QMetaType::Type CSVColumn::getType() const
{
    return m_type;
}

inline void CSVColumn::setType(QMetaType::Type colType)
{
    m_type = colType;
}

inline QString CSVColumn::getValue() const
{
    return m_value;
}

inline void CSVColumn::setValue(const QString& value)
{
    m_value = value;
}

#endif // CSVCOLUMN_H
