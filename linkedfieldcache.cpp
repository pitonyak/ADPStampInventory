#include "linkedfieldcache.h"

LinkedFieldCache::LinkedFieldCache(QObject *parent) :
  QObject(parent)
{
}

QString* LinkedFieldCache::getValue(const QString& tableName, const int id, const QString& fieldName)
{
  QHash<QString, QString>& objectCache = m_linkedFieldCache[tableName][id];
  return objectCache.contains(fieldName) ? &objectCache[fieldName] : nullptr;
}

void LinkedFieldCache::setValue(const QString& tableName, const int id, const QString& fieldName, const QString& cachedValue)
{
  m_linkedFieldCache[tableName][id].insert(fieldName, cachedValue);
}

void LinkedFieldCache::clearValue(const QString& tableName, const int id, const QString& fieldName)
{
  if (m_linkedFieldCache.contains(tableName))
  {
    QHash<int, QHash<QString, QString> >& tableCache = m_linkedFieldCache[tableName];
    if (tableCache.contains(id)) {
      tableCache[id].remove(fieldName);
    }
  }
}

void LinkedFieldCache::clearValue(const QString& tableName, const int id)
{
  if (m_linkedFieldCache.contains(tableName))
  {
    m_linkedFieldCache[tableName].remove(id);
  }
}

void LinkedFieldCache::clearValue(const QString& tableName)
{
  m_linkedFieldCache.remove(tableName);
}

void LinkedFieldCache::clearValue()
{
  m_linkedFieldCache.clear();
}
