#ifndef TABLESORTFIELD_H
#define TABLESORTFIELD_H

#include <QObject>
#include <QString>
#include "valuecomparer.h"

//**************************************************************************
/*! \class TableSortField
 * \brief Provide a method to sort columns in a field.
 *
 *  Identifies a field name, field index, order (ascending / descending), and a comparer to use.
 *  The comparer provides the actual comparison capability, which includes desired case sensitivity.
 *
 *  The field index assumes that the field names are in a particular order.
 *
 *  This class contains numerous helper methods that wrap calls to the comparer.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2013-2014
 ***************************************************************************/
class TableSortField : public QObject
{
    Q_OBJECT
    Q_ENUMS(SortOrder)
public:
    enum SortOrder {Ascending, Descending};

    //**************************************************************************
    /*! \brief Default constructor with the specified parent.
     *
     *  All objects owned by the parent are destroyed in the destructor.
     *  \param [in] parent The object's owner.
     ***************************************************************************/
    explicit TableSortField(QObject *parent = nullptr);

    //**************************************************************************
    /*! \brief Constructor copied from the specified filter object.
     *
     *  All objects owned by the parent are destroyed in the destructor.
     *  \param [in] filter This filter object is copied into this object.
     *  \param [in] parent The object's owner.
     ***************************************************************************/
    explicit TableSortField(const TableSortField& obj, QObject *parent = nullptr);

    //**************************************************************************
    /*! \brief Constructor
     *
     *  \param [in] name Field name.
     *  \param [in] index Field index. Defaults to -1.
     *  \param [in] order How to sort, Ascending or Descending. Defaults to Ascending.
     *  \param [in] sensitive Case sensitivity. Defaults to case insensitive.
     ***************************************************************************/
    TableSortField(const QString name, int index=-1, SortOrder order=Ascending, Qt::CaseSensitivity sensitive=Qt::CaseInsensitive);

    //**************************************************************************
    /*! \brief Constructor
     *
     *  \param [in] parent The object's owner.
     *  \param [in] name Field name.
     *  \param [in] index Field index. Defaults to -1.
     *  \param [in] order How to sort, Ascending or Descending. Defaults to Ascending.
     *  \param [in] sensitive Case sensitivity. Defaults to case insensitive.
     ***************************************************************************/
    TableSortField(QObject *parent, const QString name, int index=-1, SortOrder order=Ascending, Qt::CaseSensitivity sensitive=Qt::CaseInsensitive);

    //**************************************************************************
    /*! \brief Destructor to delete the comparer.
     ***************************************************************************/
    virtual ~TableSortField();

    void setFieldIndex(int index=-1);
    void setSortOrder(TableSortField::SortOrder order = TableSortField::Ascending);
    void setFieldName(const QString& name);

    int fieldIndex() const;
    TableSortField::SortOrder sortOrder() const;
    const QString& fieldName() const;

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
    void setCase(const Qt::CaseSensitivity caseSensitive = Qt::CaseInsensitive);

    //**************************************************************************
    /*! \brief Thin wrapper to call the corresponding function on the contained comparer.
     *
     * Comparer must not be null.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return true if v1 < v2
     ***************************************************************************/
    bool lessThan(const QVariant& v1, const QVariant& v2) const;

    //**************************************************************************
    /*! \brief Thin wrapper to call the corresponding function on the contained comparer.
     *
     * Comparer must not be null.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return true if v1 <= v2
     ***************************************************************************/
    bool lessThanEqual(const QVariant& v1, const QVariant& v2) const;

    //**************************************************************************
    /*! \brief Thin wrapper to call the corresponding function on the contained comparer.
     *
     * Comparer must not be null.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return true if v1 == v2
     ***************************************************************************/
    bool equal(const QVariant& v1, const QVariant& v2) const;

    //**************************************************************************
    /*! \brief Thin wrapper to call the corresponding function on the contained comparer.
     *
     * Comparer must not be null.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return true if v1 != v2
     ***************************************************************************/
    bool notEqual(const QVariant& v1, const QVariant& v2) const;

    //**************************************************************************
    /*! \brief Thin wrapper to call the corresponding function on the contained comparer.
     *
     * Comparer must not be null.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return true if v1 > v2
     ***************************************************************************/
    bool greaterThan(const QVariant& v1, const QVariant& v2) const;

    //**************************************************************************
    /*! \brief Thin wrapper to call the corresponding function on the contained comparer.
     *
     * Comparer must not be null.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return true if v1 >= v2
     ***************************************************************************/
    bool greaterThanEqual(const QVariant& v1, const QVariant& v2) const;

    //**************************************************************************
    /*! \brief Thin wrapper to call the corresponding function on the contained comparer.
     *
     * Comparer must not be null.
     *
     *  \param [in] v1 First object to compare
     *  \param [in] v2 Second object to compare
     *  \return -1 (v1 < v2), 0 (v1 == v2), or 1 (v1 > v2)
     ***************************************************************************/
    int valueCompare(const QVariant& v1, const QVariant& v2) const;

    bool isAscending() const { return m_sortOrder == Ascending; }
    bool isDescending() const { return m_sortOrder == Descending; }

    const TableSortField& operator=(const TableSortField& obj);

private:
    /*! \brief comparer is created and owned by this class. */
    ValueComparer* m_comparer;
    int m_fieldIndex;
    SortOrder m_sortOrder;
    QString m_fieldName;
};

inline void TableSortField::setFieldIndex(int index)
{
    m_fieldIndex = index;
}

inline void TableSortField::setCase(Qt::CaseSensitivity sensitive)
{
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

inline Qt::CaseSensitivity TableSortField::caseSensitivity() const
{
    return m_comparer->caseSensitivity();
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
