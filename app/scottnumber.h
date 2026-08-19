#ifndef SCOTTNUMBER_H
#define SCOTTNUMBER_H

#include <QString>

//**************************************************************************
/*! \class ScottNumber
 * \brief Provide a Scott number with split values for easy sorting.
 *
 *
 *
 *
 *  The field index assumes that the field names are in a particular order.
 *
 *  This class contains numerous helper methods that wrap calls to the comparer.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2026
 ***************************************************************************/
class ScottNumber
{
public:
    ScottNumber();
    ScottNumber(const QString& country, const QString& category, int number, const QString& trailer);
    ScottNumber(const QString& country, const QString& scott_number);
    ScottNumber(const ScottNumber& sn);

    const QString& getCountry() const {return m_country;}
    const QString& getCategory() const {return m_category;}
    const QString& getTrailer() const {return m_trailer;}
    int getNumber() const {return m_number;}

    void setCountry(QString& country) {m_country = country;}
    void setCategory(QString& category) {m_category = category;}
    void setTrailer(QString& trailer) {m_trailer = trailer;}
    void setNumber(int number) {m_number = number;}

    //**************************************************************************
    /*! \brief Get the original scott number from this. Note that the Scott number does not include the country.
     *
     *  \return Scott number as a single string.
     ***************************************************************************/
    QString getScott() const;

    //**************************************************************************
    /*! \brief Compare this to another object. Returns -1, 0, 1 for this < s2, this == s2, this > s2
     *
     *  \param [in] s2 Object against which to compare.
     *  \param [in] cs Case Sensitivity for the compare.
     *  \return -1, 0, 1 for this < s2, this == s2, this > s2
     ***************************************************************************/
    int compare(const ScottNumber& s2, Qt::CaseSensitivity cs = Qt::CaseInsensitive) const { return ScottNumber::compare(*this, s2, cs);}

    //**************************************************************************
    /*! \brief Compare two object. Returns -1, 0, 1 for s1 < s2, s1 == s2, s1 > s2
     *
     *  \param [in] s1 Object against which to compare.
     *  \param [in] s2 Object against which to compare.
     *  \param [in] cs Case Sensitivity for the compare.
     *  \return -1, 0, 1 for s1 < s2, s1 == s2, s1 > s2
     ***************************************************************************/
    static int compare(const ScottNumber& s1, const ScottNumber& s2, Qt::CaseSensitivity cs = Qt::CaseInsensitive);

    //**************************************************************************
    /*! \brief Compare this to another object to see if it is less than.
     *
     *  \param [in] s2 Object against which to compare.
     *  \return true if (*this < s2)
     ***************************************************************************/
    bool operator<(const ScottNumber& s2) const {return ScottNumber::compare(*this, s2) < 0; }

    ScottNumber& operator=(const ScottNumber& s2);

private:
    QString m_country;
    QString m_category;
    // Should be long enough!
    int m_number;
    QString m_trailer;
};

inline ScottNumber::ScottNumber() : m_country(""), m_category(""), m_number(0), m_trailer("")
{
}

inline ScottNumber::ScottNumber(const QString& country, const QString& category, int number, const QString& trailer) : m_country(country), m_category(category), m_number(number), m_trailer(trailer)
{
}

inline QString ScottNumber::getScott() const
{
    return getCategory() % QString::number(getNumber()) % getTrailer();
}

#endif // SCOTTNUMBER_H
