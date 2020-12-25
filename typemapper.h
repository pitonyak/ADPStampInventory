#ifndef TYPEMAPPER_H
#define TYPEMAPPER_H

#include <QObject>
#include <QMetaType>
#include <QMap>
#include <QVariant>

//**************************************************************************
/*! \class TypeMapper
 *  \brief Guess at a string's type and provide intelligence around how to convert between types.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2013-2020
 *
 **************************************************************************/


class TypeMapper : public QObject
{
  Q_OBJECT
  Q_ENUMS(ColumnConversionPreference)

public:

  /*! \brief Flags used during conversion to help understand what types to use while guessing a column type. */
  enum ColumnConversionPreference {
    PreferNone = 0x0,
    PreferSigned = 0x1,
    PreferUnsigned = 0x2,
    PreferInt = 0x10,
    PreferLong = 0x20
  };
  Q_DECLARE_FLAGS(ColumnConversionPreferences, ColumnConversionPreference)

  /*! \brief Constructor
   *
   *  \param [in, out] parent Object owner.
   *  \return
   */
    explicit TypeMapper(QObject *parent = 0);

    /*! \brief Determine if this class knows the type.
     *
     *  \param [in] metaType Type of interest.
     *  \return True if the type is known by this class, false otherwise.
     */
    bool containsMetaTypeEntry(const QMetaType::Type metaType) const;

    /*! \brief Try to parse the string and guess the type.
     *
     *  \param [in] s String to parse.
     *  \return Guessed type.
     */
    QMetaType::Type guessType(const QString& s, const ColumnConversionPreferences preferences);

    /*! \brief Given two types, which type allows more data to be represented.
     *
     *  Expected use is primarily things like Double is more generic than Float, and long is more generic than int.
     *  A signed type is assumed to be more generic than an unsigned type.
     *
     *  \param [in] metaType1
     *  \param [in] metaType2
     *  \return Type able to hold the most data.
     */
    QMetaType::Type mostGenericType(const QMetaType::Type metaType1, const QMetaType::Type metaType2) const;

    /*! \brief Convert an unsigned integer to a signed integer type.
     *
     *  \param [in] metaType1
     *  \return The signed equivalent if avialable, otherwise return the passed type.
     */
    QMetaType::Type toSignedInteger(const QMetaType::Type metaType1) const;

    /*! \brief Convert the meta type to a variant type, and return a null version of the specified type as a variant.
     *
     *  \param [in] metaType Type of interest.
     *  \return A variant of the specified type that is considered NULL.
     */
    QVariant getNullVariant(QMetaType::Type metaType) const;

    QString getMetaName(QMetaType::Type aType) const;

    QMetaType::Type getMetaType(const QString& name) const;

    QVariant forceToType(const QVariant& x, const QMetaType::Type aType, bool* ok = nullptr) const;

    static QVariant getDefaultValue(QMetaType::Type aType);


signals:

public slots:

private:

    /*! \brief Initialize the maps between the two types. */
    void initialize();

    /*! \brief List of signed numeric types from smallest (short) to largest (double) for use with conversions. */
    QList<QMetaType::Type> m_NumList;

    /*! \brief List of unsigned signed numeric types from smallest (unsigned short) to largest (unsigned long long) for use with conversions. */
    QList<QMetaType::Type> m_UNumList;

    QMap<QMetaType::Type, QString> m_MetaToName;
    QMap<QString, QMetaType::Type> m_NameToMetaTo;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TypeMapper::ColumnConversionPreferences)

inline QString TypeMapper::getMetaName(QMetaType::Type aType) const
{
  return QMetaType(aType).name();
}

inline QMetaType::Type TypeMapper::getMetaType(const QString& name) const
{
    return m_NameToMetaTo.contains(name.toLower()) ? m_NameToMetaTo.value(name.toLower()) : QMetaType::UnknownType;
}


inline QVariant TypeMapper::getNullVariant(QMetaType::Type metaType) const
{
  return QVariant(QMetaType(metaType));
}

#endif // TYPEMAPPER_H
