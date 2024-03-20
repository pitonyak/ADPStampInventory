#ifndef VALUECOMPARER_H
#define VALUECOMPARER_H

#include <QString>
#include <QVariant>

//**************************************************************************
/*! \class ValueComparer
 * \brief Provide a means of quickly comparing two "values" stored in a QVariant.
 *
 * Only a few types are supported, but, new types can be supported as the need is found.
 *
 * The QVariant class supports the equal and not equal operators already. The other methods
 * in this class rely on the valueCompare method.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2013-2014
 ***************************************************************************/
class ValueComparer
{
public:
  //**************************************************************************
  /*! \brief Default constructor with the specified case sensitivity.
   *
   *  \param [in] caseSensitive Sets the case sensitivity; defaults to CaseInsensitive.
   ***************************************************************************/
    ValueComparer(Qt::CaseSensitivity caseSensitive = Qt::CaseInsensitive);

    //**************************************************************************
    /*! \brief Copy constructor.
     *
     *  \param [in] obj Object to copy.
     ***************************************************************************/
    ValueComparer(const ValueComparer& obj);

    //**************************************************************************
    /*! \brief Destructor; currently this does nothing.
     *
     ***************************************************************************/
    virtual ~ValueComparer();

    //**************************************************************************
    /*! \brief Compare v1 to v2 and return -1, 0, or 1.
     *
     * First, QVariant::operator== is used. If equal, return 0.
     * If the types are not the same, convert to a string and compare as a string.
     *
     * For the same type, use a string compare for type string, Url, and Uuid.
     *
     * Compare bool, char, Date, DateTime, Time, double, int, uint, ulonglong, and long long.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return -1 if v1 < v2, 1 if v1 > v2, and 0 otherwise.
     ***************************************************************************/
    int valueCompare(const QVariant& v1, const QVariant& v2) const;

    //**************************************************************************
    /*! \brief Compare each string in v1 to each string in v2. Return -1, 0, or 1.
     *
     * Compares the strings in v1 to v2 in order (v1[0].compare(v2[0]), v1[1].compare(v2[1]), ...)
     * by calling QString::compare. Check each string until one is found that is not equal (0).
     * If they are all equal, then return -1 if v1 is shorter than v2, 1 if v2 is shorter than v1,
     * and 0 if they are the same length.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return -1 if v1 < v2, 1 if v1 > v2, and 0 otherwise.
     ***************************************************************************/
    int valueCompare(const QStringList& v1, const QStringList& v2) const;

    //**************************************************************************
    /*! \brief Thin wrapper to the valueCompare method to determine if v1 < v2.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return True if v1 < v2
     ***************************************************************************/
    bool lessThan(const QVariant& v1, const QVariant& v2) const;

    //**************************************************************************
    /*! \brief Thin wrapper to the valueCompare method to determine if v1 <= v2.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return True if v1 <= v2
     ***************************************************************************/
    bool lessThanEqual(const QVariant& v1, const QVariant& v2) const;

    //**************************************************************************
    /*! \brief Thin wrapper to the Qt provide QVariant::operator==.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return True if v1 == v2
     ***************************************************************************/
    bool equal(const QVariant& v1, const QVariant& v2) const;

    //**************************************************************************
    /*! \brief Thin wrapper to the Qt provide QVariant::operator!=.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return True if v1 != v2
     ***************************************************************************/
    bool notEqual(const QVariant& v1, const QVariant& v2) const;

    //**************************************************************************
    /*! \brief Thin wrapper to the valueCompare method to determine if v1 > v2.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return True if v1 > v2
     ***************************************************************************/
    bool greaterThan(const QVariant& v1, const QVariant& v2) const;

    //**************************************************************************
    /*! \brief Thin wrapper to the valueCompare method to determine if v1 >= v2.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return True if v1 >= v2
     ***************************************************************************/
    bool greaterThanEqual(const QVariant& v1, const QVariant& v2) const;

    //**************************************************************************
    /*! \brief Get the case sensitivity.
     *
     *  \return The case sensitivity.
     ***************************************************************************/
    Qt::CaseSensitivity caseSensitivity() const;

    //**************************************************************************
    /*! \brief Set the case sensitivity.
     *
     *  \param [in] caseSensitive Sets the case sensitivity; defaults to CaseInsensitive.
     ***************************************************************************/
    void setSensitivity(const Qt::CaseSensitivity caseSensitive = Qt::CaseInsensitive);

    //**************************************************************************
    /*! \brief Assignment operator, copies the case sensitivity.
     *
     *  \param [in] obj Object to copy.
     *  \return Reference to this object.
     ***************************************************************************/
    const ValueComparer& operator=(const ValueComparer& obj);

private:
    Qt::CaseSensitivity m_caseSensitivity;
};

inline bool ValueComparer::lessThan(const QVariant& v1, const QVariant& v2) const
{
    return ValueComparer::valueCompare(v1, v2) < 0;
}

inline bool ValueComparer::lessThanEqual(const QVariant& v1, const QVariant& v2) const
{
    return ValueComparer::valueCompare(v1, v2) <= 0;
}

inline bool ValueComparer::equal(const QVariant& v1, const QVariant& v2) const
{
    return caseSensitivity() == Qt::CaseSensitive ? v1 == v2 : (ValueComparer::valueCompare(v1, v2) == 0);
}

inline bool ValueComparer::notEqual(const QVariant& v1, const QVariant& v2) const
{
  return caseSensitivity() == Qt::CaseSensitive ? v1 != v2 : (ValueComparer::valueCompare(v1, v2) != 0);
}

inline bool ValueComparer::greaterThan(const QVariant& v1, const QVariant& v2) const
{
    return ValueComparer::valueCompare(v1, v2) > 0;
}

inline bool ValueComparer::greaterThanEqual(const QVariant& v1, const QVariant& v2) const
{
    return ValueComparer::valueCompare(v1, v2) >= 0;
}

inline void ValueComparer::setSensitivity(const Qt::CaseSensitivity caseSensitive)
{
  m_caseSensitivity = caseSensitive;
}

inline Qt::CaseSensitivity ValueComparer::caseSensitivity() const
{
  return m_caseSensitivity;
}


#endif // VALUECOMPARER_H
