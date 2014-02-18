#include "tablesortfield.h"

const QString s_Ascending = "Ascending";
const QString s_Descending = "Descending";


TableSortField::TableSortField(QObject *parent) : QObject(parent),
    m_comparer(nullptr), m_fieldIndex(-1), m_sortOrder(Ascending), m_MetaType(QMetaType::UnknownType)
{
    m_comparer = new ValueComparer(Qt::CaseInsensitive);
}

TableSortField::TableSortField(const TableSortField& obj, QObject *parent) : QObject(parent)
{
    m_comparer = new ValueComparer(obj.caseSensitivity());
    operator=(obj);
}

TableSortField::TableSortField(const QString name, int index, SortOrder order, Qt::CaseSensitivity sensitive) : QObject(nullptr),
    m_comparer(nullptr), m_fieldIndex(index), m_sortOrder(order), m_fieldName(name)
{
    m_comparer = new ValueComparer(sensitive);
}

TableSortField::TableSortField(QObject *parent, const QString name, int index, SortOrder order, Qt::CaseSensitivity sensitive) : QObject(parent),
    m_comparer(nullptr), m_fieldIndex(index), m_sortOrder(order), m_fieldName(name)
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
        setCase(obj.caseSensitivity());
        setSortOrder(obj.sortOrder());
        setFieldName(obj.fieldName());
        setFieldType(obj.fieldType());
    }
    return *this;
}


QString TableSortField::sortOrderToName(const SortOrder sortOrder)
{
    return sortOrder == TableSortField::Ascending ? "Ascending" : "Descending";
}

TableSortField::SortOrder TableSortField::sortOrderFromName(const QString& name)
{
    return name.compare("Descending", Qt::CaseInsensitive) == 0 ? TableSortField::Descending : TableSortField::Ascending;
}

QStringList TableSortField::sortOrderNames()
{
    QStringList list;
    list << "Ascending";
    list << "Descending";
    return list;
}
