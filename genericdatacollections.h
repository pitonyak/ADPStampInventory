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
    bool contains(const QString& tableName) const;
    bool contains(const QString& tableName, const QString& fieldName) const;

    QStringList getNames() const;

    GenericDataCollection* operator[](const QString& tableName);
    const GenericDataCollection* operator[](const QString& tableName) const;
    GenericDataCollection* getTable(const QString& tableName);
    const GenericDataCollection* getTable(const QString& tableName) const;

    GenericDataObject* getObject(const QString& tableName, const int id);

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
    return contains(tableName) && m_tables[tableName]->containsProperty(fieldName);
}

inline GenericDataCollection* GenericDataCollections::operator[](const QString& tableName)
{
    return getTable(tableName);
}

inline GenericDataCollection* GenericDataCollections::getTable(const QString& tableName)
{
  return contains(tableName) ? m_tables.value(tableName) : nullptr;
}

inline const GenericDataCollection* GenericDataCollections::getTable(const QString& tableName) const
{
  return contains(tableName) ? m_tables.value(tableName) : nullptr;
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
