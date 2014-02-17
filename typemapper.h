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
 * \date 2013-2014
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

  /*! \brief Map from a meta-type to a variant-type.
   *
   *  Meta-types are usually used to associate a type name to a type so that it can be created and destructed dynamically at run-time.
   *  The QMetaType class manages named types and is used as a helper to marshall types in QVariant, queued signals, and slots.
   *  This method attempts to equate types in the two systems. I believe that the QVariant::Type is (or will be) deprecated.
   *
   *  \param [in] metaType Type of interest.
   *  \return Equivalent variant type if known, or QVariant::Invalid otherwise.
   */
    QVariant::Type metaToVariantType(const QMetaType::Type metaType) const;

    /*! \brief Map from a variant-type to a meta-type.
     *
     *  Meta-types are usually used to associate a type name to a type so that it can be created and destructed dynamically at run-time.
     *  The QMetaType class manages named types and is used as a helper to marshall types in QVariant, queued signals, and slots.
     *  This method attempts to equate types in the two systems. I believe that the QVariant::Type is (or will be) deprecated.
     *
     *  \param [in] variantType Type of interest.
     *  \return Equivalent meta type if known, or QMetaType::Void otherwise.
     */
    QMetaType::Type variantTypeToMetaType(const QVariant::Type variantType) const;

    /*! \brief Determine if this class knows the type.
     *
     *  \param [in] metaType Type of interest.
     *  \return True if the type is known by this class, false otherwise.
     */
    bool hasMetaTypeEntry(const QMetaType::Type metaType) const;

    /*! \brief Determine if this class knows the type.
     *
     *  \param [in] variantType Type of interest.
     *  \return True if the type is known by this class, false otherwise.
     */
    bool hasVariantTypeEntry(const QVariant::Type variantType) const;

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

    /*! \brief Get a null version of the specified variant type.
     *
     *  \param [in] variantType Variant type of interest.
     *  \return A variant of the specified type that is considered NULL.
     */
    QVariant getNullVariant(QVariant::Type variantType) const;

    /*! \brief Convert the meta type to a variant type, and return a null version of the specified type as a variant.
     *
     *  \param [in] metaType Type of interest.
     *  \return A variant of the specified type that is considered NULL.
     */
    QVariant getNullVariant(QMetaType::Type metaType) const;

signals:

public slots:

private:

    /*! \brief Initialize the maps between the two types. */
    void initialize();

    /*! \brief Map from Meta types to Variant types for fast conversion. */
    QMap<QMetaType::Type, QVariant::Type> m_typeMapMetaToVariant;

    /*! \brief Map from Variant types to Meta types for fast conversion. */
    QMap<QVariant::Type, QMetaType::Type> m_typeMapVariantToMeta;

    /*! \brief List of signed numeric types from smallest (short) to largest (double) for use with conversions. */
    QList<QMetaType::Type> m_NumList;

    /*! \brief List of unsigned signed numeric types from smallest (unsigned short) to largest (unsigned long long) for use with conversions. */
    QList<QMetaType::Type> m_UNumList;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(TypeMapper::ColumnConversionPreferences)

inline QVariant::Type TypeMapper::metaToVariantType(const QMetaType::Type metaType) const
{
    return hasMetaTypeEntry(metaType) ? m_typeMapMetaToVariant[metaType] : QVariant::Invalid;
}

inline QMetaType::Type TypeMapper::variantTypeToMetaType(const QVariant::Type variantType) const
{
  return hasVariantTypeEntry(variantType) ? m_typeMapVariantToMeta[variantType] : QMetaType::Void;
}

inline bool TypeMapper::hasMetaTypeEntry(const QMetaType::Type metaType) const
{
    return m_typeMapMetaToVariant.contains(metaType);
}

inline bool TypeMapper::hasVariantTypeEntry(const QVariant::Type variantType) const
{
  return m_typeMapVariantToMeta.contains(variantType);
}


inline QVariant TypeMapper::getNullVariant(QVariant::Type variantType) const
{
  return QVariant(variantType);
}

inline QVariant TypeMapper::getNullVariant(QMetaType::Type metaType) const
{
  return getNullVariant(metaToVariantType(metaType));
}


#endif // TYPEMAPPER_H
