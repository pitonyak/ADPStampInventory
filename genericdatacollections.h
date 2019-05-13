#ifndef GENERICDATACOLLECTIONS_H
#define GENERICDATACOLLECTIONS_H

#include "genericdatacollection.h"

#include <QObject>
#include <QHash>
#include <QStringList>

//**************************************************************************
/*! \class GenericDataCollections
 * \brief Collection of generic collections. Think of this as a collection of tables.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2013-2014
 **************************************************************************/
class GenericDataCollections : public QObject
{
    Q_OBJECT
public:
    explicit GenericDataCollections(QObject *parent = nullptr);

    void addCollection(const QString& name, GenericDataCollection* collection);

    //**************************************************************************
    /*! \brief Does this collection contain a table as named. The name is not case sensitive.
     *
     *  \param [in] tableName Is the case insensitive table name.
     *  \return True if the tables collection contains a table with the specified name.
     ***************************************************************************/
    bool contains(const QString& tableName) const;

    //**************************************************************************
    /*! \brief Does this collection contain a table with the specified field name, both case insensitive.
     *
     *  \param [in] tableName Is the case insensitive table name.
     *  \param [in] fieldName Is the case insensitive field name.
     *  \return True if the tables collection contains the table and field.
     ***************************************************************************/
    bool contains(const QString& tableName, const QString& fieldName) const;

    QStringList getNames() const;

    GenericDataCollection* operator[](const QString& tableName);
    const GenericDataCollection* operator[](const QString& tableName) const;
    GenericDataCollection* getTable(const QString& tableName);
    const GenericDataCollection* getTable(const QString& tableName) const;

    GenericDataObject* getObject(const QString& tableName, const int id);

    //**************************************************************************
    /*! \brief Get the data value from a table if the ID (database key) exists, otherwise, return meta-data about the field.
     *
     *  \param [in] tableName Is the case insensitive table name.
     *  \param [in] id is the database key. If this exists, the data value from the table is returned. If not, column meta-data is returned.
     *  \param [in] fieldName Is the case insensitive field name.
     *  \return Data from the table, or, meta-data about the column if the ID is not present.
     ***************************************************************************/
    QVariant getValue(const QString& tableName, const int id, const QString& fieldName);

signals:

public slots:

private:
    QStringList m_names;
    QHash<QString, GenericDataCollection*> m_tables;
};

inline bool GenericDataCollections::contains(const QString& tableName) const
{
    return m_tables.contains(tableName.toLower());
}

inline bool GenericDataCollections::contains(const QString& tableName, const QString& fieldName) const
{
    return contains(tableName) && m_tables[tableName.toLower()]->containsProperty(fieldName);
}

inline GenericDataCollection* GenericDataCollections::operator[](const QString& tableName)
{
    return getTable(tableName);
}

inline GenericDataCollection* GenericDataCollections::getTable(const QString& tableName)
{
  return contains(tableName) ? m_tables.value(tableName.toLower()) : nullptr;
}

inline const GenericDataCollection* GenericDataCollections::getTable(const QString& tableName) const
{
  return contains(tableName) ? m_tables.value(tableName.toLower()) : nullptr;
}


inline const GenericDataCollection* GenericDataCollections::operator[](const QString& tableName) const
{
    return getTable(tableName);
}

inline QStringList GenericDataCollections::getNames() const
{
    return m_names;
}
#endif // GENERICDATACOLLECTIONS_H
