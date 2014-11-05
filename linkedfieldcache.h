#ifndef LINKEDFIELDCACHE_H
#define LINKEDFIELDCACHE_H

#include <QObject>
#include <QHash>

//**************************************************************************
/*! \class LinkedFieldCache
 *
 * \brief Quick link for a <Table, object ID, Field Name, Cached Value>
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2014
 ***************************************************************************/
class LinkedFieldCache : public QObject
{
  Q_OBJECT
public:
  /*! \brief Constructor
   *
   *  \param [in, out] parent The object's owner. The parent's destructor destroys this object.
   */
  explicit LinkedFieldCache(QObject *parent = nullptr);

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
  const QString* getValue(const QString& tableName, const int id, const QString& fieldName);

  /*! \brief Set the cached value for the table.row.field.
   *
   *  \param [in] tableName
   *  \param [in] id
   *  \param [in] fieldName
   *  \param [in] cachedValue
   */
  void setValue(const QString& tableName, const int id, const QString& fieldName, const QString& cachedValue);

  /*! \brief Clear the cached value for the table.row.field.
   *
   *  \param [in] tableName
   *  \param [in] id
   *  \param [in] fieldName
   */
  void clearValue(const QString& tableName, const int id, const QString& fieldName);

  /*! \brief Clear all cached values for the table.row
   *
   *  \param [in] tableName
   *  \param [in] id
   */
  void clearValue(const QString& tableName, const int id);

  /*! \brief Clear all cached values for the table
   *
   *  \param [in] tableName
   */
  void clearValue(const QString& tableName);

  /*! \brief Clear all cached values */
  void clearValue();

signals:

public slots:

private:
  /*! QHash<tableName, QHash<objectId, QHash<fieldName, cachedValue> > > */
  QHash<QString, QHash<int, QHash<QString, QString> > > m_linkedFieldCache;
};

#endif // LINKEDFIELDCACHE_H
