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
        m_changeTracker.push(ChangedObjectBase::Edit, name, object->clone(), originalObject);
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

ChangedObject<GenericDataObject>* GenericDataCollectionTableModel::popLastChange()
{
  return m_changeTracker.isEmpty() ? nullptr : m_changeTracker.pop();
}

void GenericDataCollectionTableModel::addRow()
{

}

void GenericDataCollectionTableModel::deleteRow()
{

}

void GenericDataCollectionTableModel::undoChange()
{
  bool trackState = isTracking();
  setTracking(false);
  ChangedObject<GenericDataObject>* lastObject = popLastChange();
  if (lastObject != nullptr)
  {
    if (lastObject->getChangeType() == ChangedObjectBase::Add)
    {
      // TODO
    }
    else if (lastObject->getChangeType() == ChangedObjectBase::Delete)
    {
      // TODO
    }
    else if (lastObject->getChangeType() == ChangedObjectBase::Edit)
    {
      // Info contains the modified field name!
      GenericDataObject* oldData = lastObject->getOldData();
      if (oldData != nullptr)
      {
        int column = m_collection.getPropertyIndex(lastObject->getChangeInfo());
        int row = m_collection.getIndexOf(oldData->getInt("id", -1));
        QModelIndex index =  createIndex(row, column);
        setData(index, oldData->getValueNative(lastObject->getChangeInfo()));
      }
    }
    else
    {
      qDebug("Unknown change type in undo");
    }
    delete lastObject;
  }
  setTracking(trackState);
}

void GenericDataCollectionTableModel::duplicateRow()
{

}

