#include "genericdatacollectionstablemodel.h"

#include "genericdatacollectiontablemodel.h"
#include "describesqltables.h"

#include <QLocale>
#include <QQueue>
#include <QSqlQuery>

GenericDataCollectionsTableModel::GenericDataCollectionsTableModel(const bool useLinks, const QString& tableName, GenericDataCollections& tables, DescribeSqlTables& schema, QObject *parent) :
  QAbstractTableModel(parent),
  m_useLinks(useLinks), m_isTracking(true),
  m_tableName(tableName), m_tables(tables), m_table(nullptr), m_schemas(schema)
{
  m_table = m_tables.getTable(tableName);
  Q_ASSERT_X(m_table != nullptr, "GenericDataCollectionsTableModel::GenericDataCollectionsTableModel", qPrintable(QString("Collection does not contain table %1").arg(tableName)));
  const DescribeSqlTable* tableSchema = m_schemas.getTableByName(tableName);
  if (tableSchema != nullptr) {
      m_schema = *tableSchema;
  }
}

int GenericDataCollectionsTableModel::rowCount( const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : m_table->rowCount();
}

int GenericDataCollectionsTableModel::columnCount( const QModelIndex &) const
{
  return m_table->getPropertyNameCount();
}

bool GenericDataCollectionsTableModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
  if (role == Qt::EditRole)
  {
    GenericDataObject* object = (index.column() < m_table->getPropertyNameCount()) ? m_table->getObjectByRow(index.row()) : nullptr;
    if (object != nullptr)
    {
      GenericDataObject* originalObject = isTracking() ? object->clone() : nullptr;
      QString name = m_table->getPropertyName(index.column());
      object->setValue(name, value);
      if (isTracking())
      {
        m_changeTracker.push(index.row(), index.column(), name, ChangedObjectBase::Edit, object->clone(), originalObject);
      }
      emit dataChanged(index, index);
      return true;
    }
  }
  return false;
}


QVariant GenericDataCollectionsTableModel::data( const QModelIndex &index, int role ) const
{
  const GenericDataObject* object = (index.column() < m_table->getPropertyNameCount()) ? m_table->getObjectByRow(index.row()) : nullptr;
  if (object != nullptr)
  {
    QString fieldName = m_table->getPropertyName(index.column());
    Q_ASSERT_X(m_schema.containsField(fieldName), "GenericDataCollectionsTableModel::data", qPrintable(QString("Schema does not have field %1").arg(fieldName)));
    const DescribeSqlField* fieldSchema = m_schema.getFieldByName(fieldName);

    if (role == Qt::DisplayRole)
    {
      // Only display currency symbol for display, not for edit!
      if (fieldSchema->isCurrency())
      {
        // TODO: For a face value, can potentially use a better locale for non-US stamps.
        QLocale locale;
        return locale.toCurrencyString(object->getValueNative(fieldName).toDouble());
      }
    }

    if (role == Qt::EditRole)
    {
        if (m_useLinks && fieldSchema->isLinkField())
        {
            if (m_linkCache.hasCachedListBySource(m_tableName, fieldName))
            {
                return m_linkCache.getCachedListBySource(m_tableName, fieldName);
            }
            QString cacheId = m_linkCache.hasCacheIdentifier(m_tableName, fieldName) ? m_linkCache.getCacheIdentifer(m_tableName, fieldName) : const_cast<LinkedFieldSelectionCache&>(m_linkCache).setCacheIdentifier(m_tableName, fieldName, fieldSchema->getLinkTableName(), fieldSchema->getLinkDisplayField());
            QStringList list = getLinkEditValues(fieldSchema->getLinkTableName(), fieldSchema->getLinkDisplayField());
            const_cast<LinkedFieldSelectionCache&>(m_linkCache).setCachedList(cacheId, list);
            return list;
        }
        return object->getValue(m_table->getPropertyName(index.column()));
    }
    else if (role == Qt::DisplayRole)
    {
      if (m_useLinks && fieldSchema->isLinkField())
      {
        int objectId = object->getInt("id");
        bool ok = true;
        int linkId = object->getValue(m_table->getPropertyName(index.column())).toInt(&ok);

        const QString* cachedValue = m_linkCache.getCacheValueBySourceTable(m_tableName, fieldName, objectId);

        if (cachedValue != nullptr)
        {
          return *cachedValue;
        }
        else
        {
          QString cacheId = m_linkCache.hasCacheIdentifier(m_tableName, fieldName) ? m_linkCache.getCacheIdentifer(m_tableName, fieldName) : const_cast<LinkedFieldSelectionCache&>(m_linkCache).setCacheIdentifier(m_tableName, fieldName, fieldSchema->getLinkTableName(), fieldSchema->getLinkDisplayField());
          QString newValue = getLinkValues(fieldSchema->getLinkTableName(), linkId, fieldSchema->getLinkDisplayField());
          const_cast<LinkedFieldSelectionCache&>(m_linkCache).addCacheValue(cacheId, linkId, newValue);
          return newValue;
        }
      }
      return object->getValue(m_table->getPropertyName(index.column()));
    }
  }
  return QVariant();
}

QStringList GenericDataCollectionsTableModel::getLinkEditValues(const QString& tableName, QStringList fields) const
{
    QStringList list;
    const DescribeSqlTable* schema = m_schemas.getTableByName(tableName);
    const GenericDataCollection* table = m_tables.getTable(tableName);

    // Begin by getting all cache IDs.
    QHash<int, QString> fieldToCacheId;
    for (int iField=0; iField < fields.size(); ++iField)
    {
        QString fieldName = fields.at(iField);
        const DescribeSqlField* fieldSchema = schema->getFieldByName(fieldName);
        if (fieldSchema->isLinkField())
        {
            QString cacheId = m_linkCache.hasCacheIdentifier(m_tableName, fieldName) ? m_linkCache.getCacheIdentifer(m_tableName, fieldName) : const_cast<LinkedFieldSelectionCache&>(m_linkCache).setCacheIdentifier(m_tableName, fieldName, fieldSchema->getLinkTableName(), fieldSchema->getLinkDisplayField());
            fieldToCacheId[iField] = cacheId;
        }
    }

    for (int iRow=0; iRow<table->rowCount(); ++iRow)
    {
        QString s = "";
        const GenericDataObject* row = table->getObjectByRow(iRow);
        for (int iField=0; iField < fields.size(); ++iField)
        {
            if (iField > 0) {
                s = s.append(('/'));
            }
            QString fieldName = fields.at(iField);
            const DescribeSqlField* fieldSchema = schema->getFieldByName(fieldName);
            int linkId = row->getInt(fieldName);
            if (fieldSchema->isLinkField())
            {
                QString cacheId = fieldToCacheId[iField];
                // Test for a cached value
                const QString* cachedValue = m_linkCache.getCacheValue(cacheId, linkId);
                if (cachedValue != nullptr)
                {
                  s = s.append(*cachedValue);
                }
                else
                {
                  QString newValue = getLinkValues(fieldSchema->getLinkTableName(), linkId, fieldSchema->getLinkDisplayField());
                  const_cast<LinkedFieldSelectionCache&>(m_linkCache).addCacheValue(cacheId, linkId, newValue);
                  s = s.append(newValue);
                }
            }
            else
            {
              s = s.append(row->getString(fieldName));
            }
        }
        list << s;
    }
    return list;
}

QString GenericDataCollectionsTableModel::getLinkValues(const QString& tableName, const int id, QStringList fields) const
{
  QString s = "";

  const DescribeSqlTable* schema = m_schemas.getTableByName(tableName);
  GenericDataCollection* linkTable = m_tables.getTable(tableName);
  GenericDataObject* row = (linkTable != nullptr) ? linkTable->getObjectById(id) : nullptr;
  for (int i=0; i<fields.size(); ++i)
  {
    if (i > 0) {
      s = s.append('/');
    }
    QString fieldName = fields.at(i);
    if (row != nullptr && row->containsValue(fieldName)) {
      const DescribeSqlField* fieldSchema = schema->getFieldByName(fieldName);
      int linkId = row->getInt(fieldName);
      if (fieldSchema->isLinkField())
      {
        // Test for a cached value
        const QString* cachedValue = m_linkCache.getCacheValueBySourceTable(tableName, fieldName, linkId);
        if (cachedValue != nullptr)
        {
          s = s.append(*cachedValue);
        }
        else
        {
          QString cacheId = m_linkCache.hasCacheIdentifier(tableName, fieldName) ? m_linkCache.getCacheIdentifer(tableName, fieldName) : const_cast<LinkedFieldSelectionCache&>(m_linkCache).setCacheIdentifier(tableName, fieldName, fieldSchema->getLinkTableName(), fieldSchema->getLinkDisplayField());
          QString newValue = getLinkValues(fieldSchema->getLinkTableName(), linkId, fieldSchema->getLinkDisplayField());
          const_cast<LinkedFieldSelectionCache&>(m_linkCache).addCacheValue(cacheId, linkId, newValue);
          s = s.append(newValue);
        }
      }
      else
      {
        s = s.append(row->getString(fields.at(i)));
      }
    }
  }
  return s;
}

QVariant GenericDataCollectionsTableModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if( role != Qt::DisplayRole )
  {
    return QVariant();
  }
  if (orientation == Qt::Vertical)
  {
    return section + 1;
  }
  return (section < m_table->getPropertyNameCount()) ? m_table->getPropertyName(section) : QVariant();
}

Qt::ItemFlags GenericDataCollectionsTableModel::flags( const QModelIndex &index ) const
{
  if(!index.isValid())
  {
    return Qt::ItemIsEnabled;
  }

  if (index.column() < m_table->getPropertyNameCount() && m_table->getPropertyName(index.column()).compare("id", Qt::CaseInsensitive) != 0)
  {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
  }
  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QStack<ChangedObject<GenericDataObject>*> *GenericDataCollectionsTableModel::popLastChange()
{
  return m_changeTracker.isEmpty() ? nullptr : m_changeTracker.pop();
}

void GenericDataCollectionsTableModel::getRowsAscending(const QModelIndexList& list, QList<int>& rows) const
{
  rows.clear();
  QHash<int, int> rowHash;
  for (int i=0; i<list.size(); ++i)
  {
    //qDebug(qPrintable(QString("Checking entry %1 / %2").arg(i).arg(list.size())));
    if (!rowHash.contains(list.at(i).row()))
    {
      rowHash.insert(list.at(i).row(), i);
      rows.append(list.at(i).row());
      //qDebug(qPrintable(QString(" row = %1 / %2").arg(list.at(i).row()).arg(rows.size())));
    }
  }
  qSort(rows);
}

void GenericDataCollectionsTableModel::addRow()
{
  GenericDataObject* newData = m_table->createEmptyObject();
  QStack<ChangedObject<GenericDataObject>*> * lastChanges = m_isTracking ? new QStack<ChangedObject<GenericDataObject>*>() : nullptr;

  int row = m_table->getObjectCount();
  if (m_isTracking)
  {
    lastChanges->push(new ChangedObject<GenericDataObject>(row, -1, "", ChangedObjectBase::Add, newData->clone(), nullptr) );
  }
  beginInsertRows(QModelIndex(), row, row);
  m_table->appendObject(newData->getInt("id"), newData);
  endInsertRows();
  if (m_isTracking)
  {
    m_changeTracker.push(lastChanges);
  }
}

void GenericDataCollectionsTableModel::deleteRows(const QModelIndexList &list)
{
  QList<int> rows;
  // I must delete from the bottom up so that I don't change the order.
  getRowsAscending(list, rows);

  if (!rows.isEmpty())
  {
    QStack<ChangedObject<GenericDataObject>*> * lastChanges = m_isTracking ? new QStack<ChangedObject<GenericDataObject>*>() : nullptr;

    for (int i=rows.size() - 1; i>=0; --i)
    {
      int row = rows.at(i);
      if (m_isTracking)
      {
        GenericDataObject* oldData = m_table->getObjectByRow(row);
        if (oldData != nullptr)
        {
          oldData = oldData->clone();
        }
        lastChanges->push(new ChangedObject<GenericDataObject>(row, -1, "", ChangedObjectBase::Delete, nullptr, oldData) );
      }
      qDebug(qPrintable(QString("Removing row %1").arg(row)));
      beginRemoveRows(QModelIndex(), row, row);
      m_table->removeRow(row);
      endRemoveRows();
    }
    if (m_isTracking)
    {
      m_changeTracker.push(lastChanges);
    }
  }
}


bool GenericDataCollectionsTableModel::saveTrackedChanges(const QString& tableName, const GenericDataCollection &data, QSqlDatabase &db, const DescribeSqlTables& schema)
{
  bool trackState = isTracking();
  setTracking(false);
  bool errorOccurred = false;

  while (!m_changeTracker.isEmpty() && !errorOccurred)
  {
    // While persisting changes, start at the bottom and work to the top.
    // Undo must start at the top and work to the bottom.

    QStack<ChangedObject<GenericDataObject>*> * firstChanges = m_changeTracker.takeAt(0);
    if (firstChanges != nullptr)
    {
      while (!firstChanges->isEmpty()) {
        ChangedObject<GenericDataObject>* bottomObject = firstChanges->takeFirst();
        if (bottomObject != nullptr)
        {
          if (bottomObject->getChangeType() == ChangedObjectBase::Add)
          {
            qDebug("Add record to the DB!");
            GenericDataObject* newData = bottomObject->getNewData();
            if (newData != nullptr)
            {
              QSqlQuery query(db);
              QString s = QString("INSERT INTO %1 (").arg(tableName);
              QString sValues = " VALUES (";
              for (int iCol=0; iCol<data.getPropertNames().size(); ++iCol)
              {
                if (iCol > 0) {
                  s = s.append(", ");
                  sValues = sValues.append(", ");
                }
                s = s.append(data.getPropertyName(iCol));
                sValues = sValues.append(":").append(data.getPropertyName(iCol));
              }
              QString sSQL = s.append(") ").append(sValues).append(")");
              query.prepare(sSQL);
              for (int iCol=0; iCol<data.getPropertNames().size(); ++iCol)
              {
                if (!newData->containsValue(data.getPropertyName(iCol)))
                {
                    TypeMapper mapper;
                    query.bindValue(QString(":%1").arg(data.getPropertyName(iCol)), QVariant(mapper.metaToVariantType(data.getPropertyTypeMeta(iCol))));
                } else {
                    // Assume that it converts to the correct type!
                    query.bindValue(QString(":%1").arg(data.getPropertyName(iCol)), newData->getValueNative(data.getPropertyName(iCol)));
                } /** else {
                    switch (data.getPropertyTypeMeta(iCol))
                    {
                    case QMetaType::Bool :
                    case QMetaType::Int :
                    case QMetaType::UInt :
                    case QMetaType::LongLong :
                    case QMetaType::ULongLong :
                        query.bindValue(QString(":%1").arg(data.getPropertyName(iCol)), newData->getInt(data.getPropertyName(iCol)));
                        break;
                    case QMetaType::Char :
                    case QMetaType::QChar :
                    case QMetaType::UChar :
                    case QMetaType::SChar :
                    case QMetaType::QString :
                    case QMetaType::QStringList :
                        query.bindValue(QString(":%1").arg(data.getPropertyName(iCol)), newData->getString(data.getPropertyName(iCol)));
                        break;

                    // Do I need a special case for this?
                    case QMetaType::QUrl :
                    case QMetaType::QUuid :
                        query.bindValue(QString(":%1").arg(data.getPropertyName(iCol)), newData->getString(data.getPropertyName(iCol)));
                        break;

                    case QMetaType::Double :
                    case QMetaType::Float :
                        query.bindValue(QString(":%1").arg(data.getPropertyName(iCol)), newData->getDouble(data.getPropertyName(iCol)));
                        break;
                    case QMetaType::QDate :
                        query.bindValue(QString(":%1").arg(data.getPropertyName(iCol)), newData->getDate(data.getPropertyName(iCol)));
                        break;
                    case QMetaType::QDateTime :
                        query.bindValue(QString(":%1").arg(data.getPropertyName(iCol)), newData->getDateTime(data.getPropertyName(iCol)));
                        break;
                    case QMetaType::QTime :
                        query.bindValue(QString(":%1").arg(data.getPropertyName(iCol)), newData->getTime(data.getPropertyName(iCol)));
                        break;
                    default:
                      qDebug(qPrintable(QString("Type name %1 not supported").arg(QMetaType::typeName(data.getPropertyTypeMeta(iCol)))));
                    }
                } **/
              }
              if (!query.exec())
              {
                qDebug("Failed to add row");
              }
            }
            qDebug("done adding row!");
          }
          else if (bottomObject->getChangeType() == ChangedObjectBase::Delete)
          {
            qDebug("Delete record from the DB!");
            // TODO: What if referenced by another table.
            GenericDataObject* oldData = bottomObject->getOldData();
            if (oldData != nullptr)
            {
              QSqlQuery query(db);
              // TODO: What if the key field is not id.
              QString s = QString("DELETE FROM %1 WHERE %2=:id").arg(tableName).arg("id");
              query.prepare(s);
              query.bindValue(":id", oldData->getInt("id"));
              if (!query.exec())
              {
                qDebug("Failed to delete row");
              }
            }
          }
          else if (bottomObject->getChangeType() == ChangedObjectBase::Edit)
          {
            // TODO: What if changed a key field referenced by another table.
            QString fieldName = bottomObject->getChangeInfo();

            // Info contains the modified field name!
            GenericDataObject* oldData = bottomObject->getOldData();
            GenericDataObject* newData = bottomObject->getNewData();
            if (oldData != nullptr && newData != nullptr)
            {
              QSqlQuery query(db);
              // TODO: What if the key field is not id.
              QString s = QString("UPDATE %1 SET %2=:%2 WHERE %3=:id").arg(tableName).arg(fieldName).arg("id");
              qDebug(qPrintable(QString("Update (%1)").arg(s)));
              query.prepare(s);
              query.bindValue(":id", oldData->getInt("id"));
              qDebug(qPrintable(QString("where id = %1").arg(oldData->getInt("id"))));

              if (!newData->containsValue(fieldName))
              {
                  TypeMapper mapper;
                  query.bindValue(QString(":%1").arg(fieldName), QVariant(mapper.metaToVariantType(data.getPropertyTypeMeta(fieldName))));
              } else {
                  // Assume that it converts to the correct type!
                  query.bindValue(QString(":%1").arg(fieldName), newData->getValueNative(fieldName));
              }
              /**
              switch (data.getPropertyTypeMeta(fieldName))
              {
              case QMetaType::Bool :
              case QMetaType::Int :
              case QMetaType::UInt :
              case QMetaType::LongLong :
              case QMetaType::ULongLong :
                  query.bindValue(QString(":%1").arg(fieldName), newData->getInt(fieldName));
                  qDebug(qPrintable(QString("%1 = %2").arg(fieldName).arg(newData->getInt(fieldName))));
                  break;
              case QMetaType::Char :
              case QMetaType::QString :
              case QMetaType::QUrl :
              case QMetaType::QStringList :
                  query.bindValue(QString(":%1").arg(fieldName), newData->getString(fieldName));
                  qDebug(qPrintable(QString("%1 = %2").arg(fieldName).arg(newData->getString(fieldName))));
                  break;
              case QMetaType::Double :
              case QMetaType::Float :
              case QMetaType::QDate :
              //case QVariant::Date :
                  query.bindValue(QString(":%1").arg(fieldName), newData->getDate(fieldName));
                  qDebug(qPrintable(QString("%1 = %2").arg(fieldName).arg(newData->getString(fieldName))));
                  break;
              case QMetaType::QDateTime :
                  query.bindValue(QString(":%1").arg(fieldName), newData->getDateTime(fieldName));
                  qDebug(qPrintable(QString("%1 = %2").arg(fieldName).arg(newData->getString(fieldName))));
                  break;
              case QMetaType::QTime :
                  query.bindValue(QString(":%1").arg(fieldName), newData->getTime(fieldName));
                  qDebug(qPrintable(QString("%1 = %2").arg(fieldName).arg(newData->getString(fieldName))));
                  break;
              default:
                qDebug(qPrintable(QString("Type name %1 not supported").arg(QMetaType::typeName(data.getPropertyTypeMeta(fieldName)))));
              }
              **/
              if (!query.exec())
              {
                qDebug("Failed to update row");
              }
            }
          }
          else
          {
            qDebug("Unknown change type in undo");
          }
          delete bottomObject;
        }
      }
      delete firstChanges;
    }
  }
  m_changeTracker.clear();
  setTracking(trackState);
  return errorOccurred;
}

void GenericDataCollectionsTableModel::undoChange()
{
  bool trackState = isTracking();
  setTracking(false);
  QStack<ChangedObject<GenericDataObject>*> * lastChanges = popLastChange();
  if (lastChanges != nullptr)
  {
    while (!lastChanges->isEmpty())
    {
      ChangedObject<GenericDataObject>* topObject = lastChanges->pop();
      if (topObject != nullptr)
      {
        int row = topObject->getRow();
        int column = topObject->getCol();

        if (topObject->getChangeType() == ChangedObjectBase::Add)
        {
          qDebug("Undo the add!");
          //GenericDataObject* newData = topObject->getNewData();
          //row = m_table->getIndexOf(newData->getInt("id", -1));
          qDebug(qPrintable(QString("Delete row %1").arg(row)));
          beginRemoveRows(QModelIndex(), row, row);
          m_table->removeRow(row);
          endRemoveRows();
          qDebug("done!");
        }
        else if (topObject->getChangeType() == ChangedObjectBase::Delete)
        {
          qDebug("Undo the delete!");
          GenericDataObject* oldData = topObject->getOldData();
          qDebug("Got the old data!");
          if (oldData != nullptr)
          {
            qDebug(qPrintable(QString("Inserting row %1").arg(row)));
            beginInsertRows(QModelIndex(), row, row);
            qDebug("Ready for insert");
            m_table->insertRow(row, topObject->takeOldData());
            qDebug("Ending insert");
            endInsertRows();
            qDebug("done!");
          }
        }
        else if (topObject->getChangeType() == ChangedObjectBase::Edit)
        {
          // Info contains the modified field name!
          GenericDataObject* oldData = topObject->getOldData();
          if (oldData != nullptr)
          {
            // TODO: Try using existing values.
            column = m_table->getPropertyIndex(topObject->getChangeInfo());
            row = m_table->getIndexOf(oldData->getInt("id", -1));
            QModelIndex index =  createIndex(row, column);
            setData(index, oldData->getValueNative(topObject->getChangeInfo()));
          }
        }
        else
        {
          qDebug("Unknown change type in undo");
        }
        delete topObject;
      }
    }

    delete lastChanges;
  }
  setTracking(trackState);
}

void GenericDataCollectionsTableModel::duplicateRows(const QModelIndexList& list)
{
  qDebug(qPrintable(QString("duplicateRows %1").arg(list.size())));
  QList<int> rows;
  getRowsAscending(list, rows);
  QQueue<GenericDataObject*> dataToCopy;
  for (int i=0; i<rows.size(); ++i)
  {
    qDebug(qPrintable(QString("Copying %1").arg(i)));
    dataToCopy.enqueue(m_table->getObjectByRow(rows.at(i)));
  }
  qDebug(qPrintable(QString("duplicateRows %1").arg(rows.size())));
  if (!rows.isEmpty())
  {
    int largestId = m_table->getLargestId();
    QStack<ChangedObject<GenericDataObject>*> * lastChanges = m_isTracking ? new QStack<ChangedObject<GenericDataObject>*>() : nullptr;
    while (!dataToCopy.isEmpty())
    {
      GenericDataObject* oldData = dataToCopy.dequeue();
      GenericDataObject* newData = oldData->clone();
      newData->setValue("id", ++largestId);
      int row = m_table->getObjectCount();
      if (m_isTracking)
      {
        lastChanges->push(new ChangedObject<GenericDataObject>(row, -1, "", ChangedObjectBase::Add, newData->clone(), nullptr) );
      }
      beginInsertRows(QModelIndex(), row, row);
      m_table->appendObject(largestId, newData);
      endInsertRows();
    }
    if (m_isTracking)
    {
      m_changeTracker.push(lastChanges);
    }
  }
}

