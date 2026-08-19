#include "scottnumber.h"
#include "imageutility.h"

ScottNumber::ScottNumber(const ScottNumber& s2) : m_country(s2.getCountry()), m_category(s2.getCategory()), m_number(s2.getNumber()), m_trailer(s2.getTrailer())
{
}

ScottNumber::ScottNumber(const QString& country, const QString& scott_number) : m_country(country), m_number(0)
{
    ImageUtility iu;
    QString num_str;
    iu.splitCatalogNumber(scott_number, m_category, num_str, m_trailer);
    if (!num_str.isEmpty()) {
        m_number = num_str.toInt();
    }
}


ScottNumber& ScottNumber::operator=(const ScottNumber& s2)
{
    if (this != &s2) {
        m_country = s2.getCountry();
        m_category = s2.getCategory();
        m_number = s2.getNumber();
        m_trailer = s2.getTrailer();
    }
    return *this;
}

int ScottNumber::compare(const ScottNumber& s1, const ScottNumber& s2, Qt::CaseSensitivity cs)
{
    int rc = QString::compare(s1.getCountry(), s2.getCountry(), cs);
    if (rc != 0)
        return rc;
    rc = QString::compare(s1.getCategory(), s2.getCategory(), cs);
    if (rc != 0)
        return rc;
    if (s1.getNumber() < s2.getNumber())
        return -1;
    if (s1.getNumber() > s2.getNumber())
        return 1;
    return QString::compare(s1.getTrailer(), s2.getTrailer(), cs);
}

