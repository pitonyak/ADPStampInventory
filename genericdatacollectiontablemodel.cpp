#include "genericdatacollectiontablemodel.h"

GenericDataCollectionTableModel::GenericDataCollectionTableModel(GenericDataCollection &data, QObject *parent) :
  QAbstractTableModel(parent), m_collection(data)
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
      // TODO: Verify that the data really did change!
      QString name = m_collection.getPropertyName(index.column());
      object->setValue(name, value);
      m_changeTracker.push(ChangedObjectBase::Edit, name, object->clone());
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
