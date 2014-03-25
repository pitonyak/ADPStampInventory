#include "genericdatacollectiontablemodel.h"

#include <QLocale>

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

void GenericDataCollectionTableModel::addRow()
{

}

void GenericDataCollectionTableModel::deleteRows(QModelIndexList& list)
{
  QHash<int, int> rowHash;
  QList<int> rows;
  for (int i=0; i<list.size(); ++i)
  {
    qDebug(qPrintable(QString("Checking entry %1 / %2").arg(i).arg(list.size())));
    if (!rowHash.contains(list.at(i).row()))
    {
      rowHash.insert(list.at(i).row(), i);
      rows.append(list.at(i).row());
    }
  }
  qSort(rows);

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
          // TODO
        }
        else if (topObject->getChangeType() == ChangedObjectBase::Delete)
        {
          qDebug("Undo the delete!");
          GenericDataObject* oldData = topObject->getOldData();
          qDebug("Got the old data!");
          if (oldData != nullptr)
          {
            qDebug("Got row from top object");
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

void GenericDataCollectionTableModel::duplicateRow()
{

}

