#include "genericdatacollectiontablemodel.h"
#include "describesqltables.h"

#include <QLocale>
#include <QQueue>
#include <QSqlQuery>

GenericDataCollectionTableModel::GenericDataCollectionTableModel(GenericDataCollection &data, QObject *parent) :
  QAbstractTableModel(parent), m_isTracking(true), m_collection(data)
{
}

int GenericDataCollectionTableModel::rowCount( const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : m_collection.rowCount();
}

int GenericDataCollectionTableModel::columnCount( const QModelIndex &) const
{
  return m_collection.getPropertyNameCount();
}

bool GenericDataCollectionTableModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
  if (role == Qt::EditRole)
  {
    GenericDataObject* object = (index.column() < m_collection.getPropertyNameCount()) ? m_collection.getObjectByRow(index.row()) : nullptr;
    if (object != nullptr)
    {
      GenericDataObject* originalObject = isTracking() ? object->clone() : nullptr;
      QString name = m_collection.getPropertyName(index.column());
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


QVariant GenericDataCollectionTableModel::data( const QModelIndex &index, int role ) const
{
  const GenericDataObject* object = (index.column() < m_collection.getPropertyNameCount()) ? m_collection.getObjectByRow(index.row()) : nullptr;
  if (object != nullptr)
  {
    QString fieldName = m_collection.getPropertyName(index.column());
    if (role == Qt::DisplayRole)
    {
      if (fieldName.compare("paid", Qt::CaseInsensitive) == 0 ||
          fieldName.compare("facevalue", Qt::CaseInsensitive) == 0 ||
          fieldName.compare("bookvalue", Qt::CaseInsensitive) == 0)
      {
        // TODO: For a face value, can potentially use a better locale for non-US stamps.
        QLocale locale;
        return locale.toCurrencyString(object->getValueNative(fieldName).toDouble());
      }
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
      return object->getValue(m_collection.getPropertyName(index.column()));
    }
  }
  return QVariant();
}

QVariant GenericDataCollectionTableModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if( role != Qt::DisplayRole )
  {
    return QVariant();
  }
  if (orientation == Qt::Vertical)
  {
    return section + 1;
  }
  return (section < m_collection.getPropertyNameCount()) ? m_collection.getPropertyName(section) : QVariant();
}

Qt::ItemFlags GenericDataCollectionTableModel::flags( const QModelIndex &index ) const
{
  if(!index.isValid())
  {
    return Qt::ItemIsEnabled;
  }

  if (index.column() < m_collection.getPropertyNameCount() && m_collection.getPropertyName(index.column()).compare("id", Qt::CaseInsensitive) != 0)
  {
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
  }
  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QStack<ChangedObject<GenericDataObject>*> *GenericDataCollectionTableModel::popLastChange()
{
  return m_changeTracker.isEmpty() ? nullptr : m_changeTracker.pop();
}

void GenericDataCollectionTableModel::getRowsAscending(const QModelIndexList& list, QList<int>& rows) const
{
  rows.clear();
  QHash<int, int> rowHash;
  for (int i=0; i<list.size(); ++i)
  {
    qDebug(qPrintable(QString("Checking entry %1 / %2").arg(i).arg(list.size())));
    if (!rowHash.contains(list.at(i).row()))
    {
      rowHash.insert(list.at(i).row(), i);
      rows.append(list.at(i).row());
      qDebug(qPrintable(QString(" row = %1 / %2").arg(list.at(i).row()).arg(rows.size())));
    }
  }
  qSort(rows);
}

void GenericDataCollectionTableModel::addRow()
{
  GenericDataObject* newData = m_collection.createEmptyObject();
  QStack<ChangedObject<GenericDataObject>*> * lastChanges = m_isTracking ? new QStack<ChangedObject<GenericDataObject>*>() : nullptr;

  int row = m_collection.getObjectCount();
  if (m_isTracking)
  {
    lastChanges->push(new ChangedObject<GenericDataObject>(row, -1, "", ChangedObjectBase::Add, newData->clone(), nullptr) );
  }
  beginInsertRows(QModelIndex(), row, row);
  m_collection.appendObject(newData->getInt("id"), newData);
  endInsertRows();
  if (m_isTracking)
  {
    m_changeTracker.push(lastChanges);
  }
}

void GenericDataCollectionTableModel::deleteRows(const QModelIndexList &list)
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
        GenericDataObject* oldData = m_collection.getObjectByRow(row);
        if (oldData != nullptr)
        {
          oldData = oldData->clone();
        }
        lastChanges->push(new ChangedObject<GenericDataObject>(row, -1, "", ChangedObjectBase::Delete, nullptr, oldData) );
      }
      qDebug(qPrintable(QString("Removing row %1").arg(row)));
      beginRemoveRows(QModelIndex(), row, row);
      m_collection.removeRow(row);
      endRemoveRows();
    }
    if (m_isTracking)
    {
      m_changeTracker.push(lastChanges);
    }
  }
}


bool GenericDataCollectionTableModel::saveTrackedChanges(const QString& tableName, const GenericDataCollection &data, QSqlDatabase &db, const DescribeSqlTables& schema)
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

void GenericDataCollectionTableModel::undoChange()
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
          //row = m_collection.getIndexOf(newData->getInt("id", -1));
          qDebug(qPrintable(QString("Delete row %1").arg(row)));
          beginRemoveRows(QModelIndex(), row, row);
          m_collection.removeRow(row);
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
            m_collection.insertRow(row, topObject->takeOldData());
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
            column = m_collection.getPropertyIndex(topObject->getChangeInfo());
            row = m_collection.getIndexOf(oldData->getInt("id", -1));
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

void GenericDataCollectionTableModel::duplicateRows(const QModelIndexList& list)
{
  qDebug(qPrintable(QString("duplicateRows %1").arg(list.size())));
  QList<int> rows;
  getRowsAscending(list, rows);
  QQueue<GenericDataObject*> dataToCopy;
  for (int i=0; i<rows.size(); ++i)
  {
    qDebug(qPrintable(QString("Copying %1").arg(i)));
    dataToCopy.enqueue(m_collection.getObjectByRow(rows.at(i)));
  }
  qDebug(qPrintable(QString("duplicateRows %1").arg(rows.size())));
  if (!rows.isEmpty())
  {
    int largestId = m_collection.getLargestId();
    QStack<ChangedObject<GenericDataObject>*> * lastChanges = m_isTracking ? new QStack<ChangedObject<GenericDataObject>*>() : nullptr;
    while (!dataToCopy.isEmpty())
    {
      GenericDataObject* oldData = dataToCopy.dequeue();
      GenericDataObject* newData = oldData->clone();
      newData->setValue("id", ++largestId);
      int row = m_collection.getObjectCount();
      if (m_isTracking)
      {
        lastChanges->push(new ChangedObject<GenericDataObject>(row, -1, "", ChangedObjectBase::Add, newData->clone(), nullptr) );
      }
      beginInsertRows(QModelIndex(), row, row);
      m_collection.appendObject(largestId, newData);
      endInsertRows();
    }
    if (m_isTracking)
    {
      m_changeTracker.push(lastChanges);
    }
  }
}

