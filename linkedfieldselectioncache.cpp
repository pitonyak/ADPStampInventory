#include "linkedfieldselectioncache.h"

LinkedFieldSelectionCache::LinkedFieldSelectionCache(QObject *parent) :
    QObject(parent)
{
}

bool LinkedFieldSelectionCache::hasCacheIdentifier(const QString& tableName, const QString& fieldName) const
{
  return m_tableFieldToCashIdentifierName.contains(tableName) && m_tableFieldToCashIdentifierName[tableName].contains(fieldName);
}

QString LinkedFieldSelectionCache::buildCacheIdentifier(const QString& targetTableName, const QStringList fields) const
{
    return QString("%1|%2").arg(targetTableName).arg(fields.join('|'));
}
QString LinkedFieldSelectionCache::buildCacheIdentifier(const QString& targetTableName, const QString& field) const
{
    return QString("%1|%2").arg(targetTableName).arg(field);
}

QString LinkedFieldSelectionCache::setCacheIdentifier(const QString& tableName, const QString& fieldName, const QString& targetTableName, const QStringList fields)
{
  return setCacheIdentifier(tableName, fieldName, buildCacheIdentifier(targetTableName, fields));
}

QString LinkedFieldSelectionCache::setCacheIdentifier(const QString& tableName, const QString& fieldName, const QString& cacheIdentifier)
{
  m_tableFieldToCashIdentifierName[tableName][fieldName] = cacheIdentifier;
  return cacheIdentifier;
}

QString LinkedFieldSelectionCache::getCacheIdentifer(const QString& tableName, const QString& fieldName) const
{
  return m_tableFieldToCashIdentifierName.contains(tableName) && m_tableFieldToCashIdentifierName[tableName].contains(fieldName) ? m_tableFieldToCashIdentifierName[tableName][fieldName] : "";
}

void LinkedFieldSelectionCache::addCacheValue(const QString& cacheIdentifier, const int id, const QString& cacheValue)
{
  m_cachedValueToId[cacheIdentifier][cacheValue] = id;
  m_IdToCachedValue[cacheIdentifier][id] = cacheValue;
  //qDebug(qPrintable(QString("Show  m_cachedValueToId[%1][%2]=%3").arg(cacheIdentifier).arg(cacheValue).arg(id)));
  //qDebug(qPrintable(QString("Show  m_IdToCachedValue[%1][%2]=%3").arg(cacheIdentifier).arg(id).arg(cacheValue)));
}

const QString *LinkedFieldSelectionCache::getCacheValue(const QString& cacheIdentifier, const int id) const
{
    if (m_IdToCachedValue.contains(cacheIdentifier))
    {
        QHash<int, QString>& innerHash = const_cast<QHash<QString, QHash<int, QString> >&>(m_IdToCachedValue)[cacheIdentifier];
        if (innerHash.contains(id)) {
            return &innerHash[id];
        }
    }
    return nullptr;
}

bool LinkedFieldSelectionCache::hasCachedList(const QString& cacheIdentifier) const
{
    return m_cachedLists.contains(cacheIdentifier);
}

QStringList LinkedFieldSelectionCache::getCachedList(const QString& cacheIdentifier) const
{
    return m_cachedLists.contains(cacheIdentifier) ? m_cachedLists[cacheIdentifier] : QStringList();
}

void LinkedFieldSelectionCache::setCachedList(const QString& cacheIdentifier, QStringList list)
{
  m_cachedLists[cacheIdentifier] = list;
}

void LinkedFieldSelectionCache::clear()
{
  m_cachedLists.clear();
  m_cachedValueToId.clear();
  m_IdToCachedValue.clear();
  m_tableFieldToCashIdentifierName.clear();
}

const QString* LinkedFieldSelectionCache::getCacheValueBySourceTable(const QString& tableName, const QString& fieldName, const int id) const
{
  if (hasCacheIdentifier(tableName, fieldName))
  {
    QString cacheId = m_tableFieldToCashIdentifierName[tableName][fieldName];
    if (m_IdToCachedValue.contains(cacheId)) {
      QHash<int, QString>& hash = const_cast<QHash<QString, QHash<int, QString> >&>(m_IdToCachedValue)[cacheId];
      if (hash.contains(id)) {
        return &hash[id];
      }
    }
  }
  return nullptr;
}

bool LinkedFieldSelectionCache::hasCachedListBySource(const QString& tableName, const QString& fieldName) const
{
  return hasCacheIdentifier(tableName, fieldName) && hasCachedList(m_tableFieldToCashIdentifierName[tableName][fieldName]);
}

QStringList LinkedFieldSelectionCache::getCachedListBySource(const QString& tableName, const QString& fieldName) const
{
  if (hasCacheIdentifier(tableName, fieldName))
  {
    QString cacheId = m_tableFieldToCashIdentifierName[tableName][fieldName];
    if (hasCachedList(cacheId)) {
        return m_cachedLists[cacheId];
    }
  }
  return QStringList();
}

int LinkedFieldSelectionCache::getIdForCachedValue(const QString& cacheId, const QString& cachedValue) const
{
  if (m_cachedValueToId.contains(cacheId) && m_cachedValueToId[cacheId].contains(cachedValue))
  {
    return m_cachedValueToId[cacheId][cachedValue];
  }
  return -1;
}
