#include "tablesortfield.h"

TableSortField::TableSortField(QObject *parent) : QObject(parent),
    m_comparer(nullptr), m_fieldIndex(-1), m_caseSensitive(Qt::CaseInsensitive), m_sortOrder(Ascending)
{
    m_comparer = new ValueComparer(m_caseSensitive);
}

TableSortField::TableSortField(const TableSortField& obj, QObject *parent) : QObject(parent)
{
    m_comparer = new ValueComparer(obj.caseSensitive());
    operator=(obj);
}

TableSortField::TableSortField(const QString name, int index, SortOrder order, Qt::CaseSensitivity sensitive) : QObject(nullptr),
    m_comparer(nullptr), m_fieldIndex(index), m_caseSensitive(sensitive), m_sortOrder(order), m_fieldName(name)
{
    m_comparer = new ValueComparer(m_caseSensitive);
}

TableSortField::TableSortField(QObject *parent, const QString name, int index, SortOrder order, Qt::CaseSensitivity sensitive) : QObject(parent),
    m_comparer(nullptr), m_fieldIndex(index), m_caseSensitive(sensitive), m_sortOrder(order), m_fieldName(name)
{
    m_comparer = new ValueComparer(sensitive);
}

TableSortField::~TableSortField()
{
    if (m_comparer != nullptr)
    {
        delete m_comparer;
        // wasted cycles...
        m_comparer = nullptr;
    }
}

const TableSortField& TableSortField::operator=(const TableSortField& obj)
{
    if (this != &obj)
    {
        *m_comparer = *obj.m_comparer;
        setFieldIndex(obj.fieldIndex());
        setCase(obj.caseSensitive());
        setSortOrder(obj.sortOrder());
        setFieldName(obj.fieldName());
    }
    return *this;
}
