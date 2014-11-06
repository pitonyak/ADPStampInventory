#ifndef LINKEDFIELDSELECTIONCACHE_H
#define LINKEDFIELDSELECTIONCACHE_H

#include <QObject>
#include <QHash>
#include <QStringList>

//**************************************************************************
/*! \class LinkedFieldSelectionCache
 *
 * \brief Quick link for a <Table, object ID, Field Name, Cached Value>
 *
 * When a new value is chosen for linked field, a drop down containing all supported values
 * is displayed. This class caches the linked values for a specific table / field combination.
 *
 * For a given table.field that is a link, the displayed values are a set of fields strung together.
 * A unique name is derived as "<tableName>|<field1Name>|<field2Name>|...<fieldKName>"
 * This unique name is called the "Cache Identifier Name".
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2014
 ***************************************************************************/
class LinkedFieldSelectionCache : public QObject
{
    Q_OBJECT
public:
    /*! \brief Constructor
     *
     *  \param [in, out] parent The object's owner. The parent's destructor destroys this object.
     */
    explicit LinkedFieldSelectionCache(QObject *parent = nullptr);


    /*! \brief Get a pointer to the cached value; returns nullptr if the value does not exist.
     *
     * This method is not const because it has side effects.
     * Specifically, an entry for the table and the object id will exist after the call.
     * This is done for speed considerations.
     *
     *  \param [in] tableName
     *  \param [in] id
     *  \param [in] fieldName
     *
     *  \return Pointer to the cached value; returns nullptr if the value does not exist.
     */

    QString buildCacheIdentifier(const QString& targetTableName, const QStringList fields) const;
    QString buildCacheIdentifier(const QString& targetTableName, const QString& field) const;
    QString setCacheIdentifier(const QString& tableName, const QString& fieldName, const QString& targetTableName, const QStringList fields);
    QString setCacheIdentifier(const QString& tableName, const QString& fieldName, const QString& cacheIdentifier);
    bool hasCacheIdentifier(const QString& tableName, const QString& fieldName) const;
    QString getCacheIdentifer(const QString& tableName, const QString& fieldName) const;

    void addCacheValue(const QString& cacheIdentifier, const int id, const QString& cacheValue);
    const QString* getCacheValue(const QString& cacheIdentifier, const int id) const;

    void clear();

    bool hasCachedList(const QString& cacheIdentifier) const;
    QStringList getCachedList(const QString& cacheIdentifier) const;
    void setCachedList(const QString& cacheIdentifier, QStringList list);

    const QString* getCacheValueBySourceTable(const QString& tableName, const QString& fieldName, const int id) const;
    bool hasCachedListBySource(const QString& tableName, const QString& fieldName) const;
    QStringList getCachedListBySource(const QString& tableName, const QString& fieldName) const;

    int getIdForCachedValue(const QString& cacheId, const QString& cachedValue) const;

  signals:

  public slots:

  private:
    /*! The ID is the ID value from linked table, so it is what should be stored in the source table
     * QHash<SourceTableName, QHash<SourceFieldName, QHash<cachedValue, link_table_id> > >
     * What we really want is to say that for a given table, this is a particular set of fields strung together.
     * If we don't care about memory, then we can settle on assuming that for a specific
     * table.field, the target table and fields do not change.
     */

    /*! Associate a cache identifier name to a list of valid values. */
    QHash<QString, QStringList> m_cachedLists;

    /*! Associate a cache identifier name to a hash that associates individual cache values to an ID in the table. */
    QHash<QString, QHash<QString, int> > m_cachedValueToId;

    /*! Associate a cache identifier name to a hash that associates ID in the table to a single cached value. */
    QHash<QString, QHash<int, QString> > m_IdToCachedValue;

    /*! Take tableName.fieldName and associate to the Cache Identifier Name.
     *  Use this to quickly obtain the field list names for a specific field in a table.
     *  The cache identifier takes a source table and a source field and
     *  builds an identifier that references another table and a list of fields in that table.
     */
    QHash<QString, QHash<QString, QString> > m_tableFieldToCashIdentifierName;
};

#endif // LINKEDFIELDSELECTIONCACHE_H
