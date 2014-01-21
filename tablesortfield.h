#ifndef TABLESORTFIELD_H
#define TABLESORTFIELD_H

#include <QObject>
#include <QString>
#include "valuecomparer.h"

class TableSortField : public QObject
{
    Q_OBJECT
    Q_ENUMS(SortOrder)
public:
    enum SortOrder {Ascending, Descending};

    explicit TableSortField(QObject *parent = nullptr);
    explicit TableSortField(const TableSortField& obj, QObject *parent = nullptr);
    TableSortField(const QString name, int index=-1, SortOrder order=Ascending, Qt::CaseSensitivity sensitive=Qt::CaseInsensitive);
    TableSortField(QObject *parent, const QString name, int index=-1, SortOrder order=Ascending, Qt::CaseSensitivity sensitive=Qt::CaseInsensitive);

    virtual ~TableSortField();

    void setFieldIndex(int index=-1);
    void setCase(Qt::CaseSensitivity sensitive=Qt::CaseInsensitive);
    void setSortOrder(TableSortField::SortOrder order = TableSortField::Ascending);
    void setFieldName(const QString& name);

    int fieldIndex() const;
    Qt::CaseSensitivity caseSensitive() const;
    TableSortField::SortOrder sortOrder() const;
    const QString& fieldName() const;

    bool lessThan(const QVariant& v1, const QVariant& v2) const;
    bool lessThanEqual(const QVariant& v1, const QVariant& v2) const;
    bool equal(const QVariant& v1, const QVariant& v2) const;
    bool notEqual(const QVariant& v1, const QVariant& v2) const;
    bool greaterThan(const QVariant& v1, const QVariant& v2) const;
    bool greaterThanEqual(const QVariant& v1, const QVariant& v2) const;
    int valueCompare(const QVariant& v1, const QVariant& v2) const;

    bool isAscending() const { return m_sortOrder == Ascending; }
    bool isDescending() const { return m_sortOrder == Descending; }

    const TableSortField& operator=(const TableSortField& obj);

private:
    ValueComparer* m_comparer;
    int m_fieldIndex;
    Qt::CaseSensitivity m_caseSensitive;
    SortOrder m_sortOrder;
    QString m_fieldName;
};

inline void TableSortField::setFieldIndex(int index)
{
    m_fieldIndex = index;
}

inline void TableSortField::setCase(Qt::CaseSensitivity sensitive)
{
    m_caseSensitive = sensitive;
    m_comparer->setSensitivity(sensitive);
}

inline void TableSortField::setSortOrder(TableSortField::SortOrder order)
{
    m_sortOrder = order;
}

inline void TableSortField::setFieldName(const QString& name)
{
    m_fieldName = name;
}

inline int TableSortField::fieldIndex() const
{
    return m_fieldIndex;
}

inline Qt::CaseSensitivity TableSortField::caseSensitive() const
{
    return m_caseSensitive;
}

inline TableSortField::SortOrder TableSortField::sortOrder() const
{
    return m_sortOrder;
}

inline const QString& TableSortField::fieldName() const
{
    return m_fieldName;
}

inline bool TableSortField::lessThan(const QVariant& v1, const QVariant& v2) const
{
    return m_sortOrder == TableSortField::Ascending ? m_comparer->lessThan(v1, v2) : m_comparer->greaterThanEqual(v1, v2) ;
}

inline bool TableSortField::lessThanEqual(const QVariant& v1, const QVariant& v2) const
{
    return m_sortOrder == TableSortField::Ascending ? m_comparer->lessThanEqual(v1, v2) : m_comparer->greaterThan(v1, v2) ;
}

inline bool TableSortField::equal(const QVariant& v1, const QVariant& v2) const
{
    return m_sortOrder == TableSortField::Ascending ? m_comparer->equal(v1, v2) : m_comparer->notEqual(v1, v2) ;
}

inline bool TableSortField::notEqual(const QVariant& v1, const QVariant& v2) const
{
    return m_sortOrder == TableSortField::Ascending ? m_comparer->notEqual(v1, v2) : m_comparer->equal(v1, v2) ;
}

inline bool TableSortField::greaterThan(const QVariant& v1, const QVariant& v2) const
{
    return m_sortOrder == TableSortField::Ascending ? m_comparer->greaterThan(v1, v2) : m_comparer->lessThanEqual(v1, v2) ;
}

inline bool TableSortField::greaterThanEqual(const QVariant& v1, const QVariant& v2) const
{
    return m_sortOrder == TableSortField::Ascending ? m_comparer->greaterThanEqual(v1, v2) : m_comparer->lessThan(v1, v2) ;
}

inline int TableSortField::valueCompare(const QVariant& v1, const QVariant& v2) const
{
    return m_sortOrder == TableSortField::Ascending ? m_comparer->valueCompare(v1, v2) : m_comparer->valueCompare(v1, v2) * -1 ;
}

#endif // TABLESORTFIELD_H
