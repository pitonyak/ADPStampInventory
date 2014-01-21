#ifndef VALUECOMPARER_H
#define VALUECOMPARER_H

#include <QString>
#include <QVariant>

class ValueComparer
{
public:
    ValueComparer(Qt::CaseSensitivity caseSensitive = Qt::CaseInsensitive);
    ValueComparer(const ValueComparer& obj);
    virtual ~ValueComparer();

    int valueCompare(const QVariant& v1, const QVariant& v2) const;
    int valueCompare(const QStringList& v1, const QStringList& v2) const;

    bool lessThan(const QVariant& v1, const QVariant& v2) const;
    bool lessThanEqual(const QVariant& v1, const QVariant& v2) const;
    bool equal(const QVariant& v1, const QVariant& v2) const;
    bool notEqual(const QVariant& v1, const QVariant& v2) const;
    bool greaterThan(const QVariant& v1, const QVariant& v2) const;
    bool greaterThanEqual(const QVariant& v1, const QVariant& v2) const;

    Qt::CaseSensitivity caseSensitivity() const { return m_caseSensitivity; }
    void setSensitivity(const Qt::CaseSensitivity caseSensitive = Qt::CaseInsensitive) { m_caseSensitivity = caseSensitive; }

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
    return v1 == v2;
}

inline bool ValueComparer::notEqual(const QVariant& v1, const QVariant& v2) const
{
    return v1 != v2;
}

inline bool ValueComparer::greaterThan(const QVariant& v1, const QVariant& v2) const
{
    return ValueComparer::valueCompare(v1, v2) > 0;
}

inline bool ValueComparer::greaterThanEqual(const QVariant& v1, const QVariant& v2) const
{
    return ValueComparer::valueCompare(v1, v2) >= 0;
}


#endif // VALUECOMPARER_H
