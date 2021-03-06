#include "tablesortfieldtablemodel.h"
#include "xmlutility.h"
#include "genericdatacollection.h"

const int TableSortFieldTableModel::nameColumn = 0;
const int TableSortFieldTableModel::ascendingColumn = 1;
const int TableSortFieldTableModel::caseColumn = 2;
const int TableSortFieldTableModel::typeColumn = 3;
const int TableSortFieldTableModel::numColumns = 4;


TableSortFieldTableModel::TableSortFieldTableModel(const GenericDataCollection* dataCollection, QObject *parent) :
  QAbstractTableModel(parent), m_dataCollection(dataCollection)
{
}

int TableSortFieldTableModel::rowCount( const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : m_sortFields.size();
}

int TableSortFieldTableModel::columnCount( const QModelIndex &) const
{
  return numColumns;
}

bool TableSortFieldTableModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
  (void) value;
  if (role == Qt::EditRole)
  {
    TableSortField& field = m_sortFields[index.row()];
    switch (index.column())
    {
    case typeColumn:
      // Read-only field, this should not happen.
      // field.setFieldType(m_mapper.getMetaType(value.toString()));
      return false;
      break;
    case nameColumn:
      field.setFieldName(value.toString());
      if (m_dataCollection != nullptr)
      {
        field.setFieldType(m_dataCollection->getPropertyTypeMeta(field.fieldName()));
        field.setFieldIndex(m_dataCollection->getPropertyIndex(field.fieldName()));
      }
      break;
    case ascendingColumn:
      field.setSortOrder(value.toBool() ? Qt::AscendingOrder : Qt::DescendingOrder);
      break;
    case caseColumn:
      field.setCase(value.toBool() ? Qt::CaseSensitive : Qt::CaseInsensitive);
      break;
    default:
      // No other column is setable here!
      return false;
      break;
    }
    emit dataChanged(index, index);
    return true;
  }
  return false;
}


QVariant TableSortFieldTableModel::data( const QModelIndex &index, int role ) const
{
  const TableSortField& field = m_sortFields[index.row()];
  switch( role )
  {
  case Qt::DisplayRole:
    switch (index.column())
    {
    case typeColumn:
      return m_mapper.getMetaName(field.fieldType());
      break;
    case nameColumn:
      return field.fieldName();
      break;
    case ascendingColumn:
      return (field.sortOrder() == Qt::AscendingOrder);
      break;
    case caseColumn:
      return (field.caseSensitivity() == Qt::CaseSensitive);
      break;
    default:
      // No other column is valid
      break;
    }
    return (index.row()+1) * (index.column()+1);

  case Qt::EditRole:
    switch (index.column())
    {
    case typeColumn:
      // Cannot edit the type column.
      return m_mapper.getMetaName(field.fieldType());
      break;
    case nameColumn:
      {
        QStringList qsl;
        QString sName = data(index, Qt::DisplayRole).toString();
        qsl << sName;
        qsl << sName;
        if (m_dataCollection != nullptr)
        {
          for (int i=0; i<m_dataCollection->getPropertyNameCount(); ++i)
          {
            bool found_it = false;
            for (int j=0; j<m_sortFields.size() && !found_it; ++j)
            {
              found_it = m_sortFields.at(j).fieldName().compare(m_dataCollection->getPropertyName(i), Qt::CaseInsensitive) == 0;
            }
            if (!found_it)
            {
              qsl << m_dataCollection->getPropertyName(i);
            }
          }
        }
        return qsl;
      }
      break;
    case ascendingColumn:
      return (field.sortOrder() == Qt::AscendingOrder) ? true : false;
      break;
    case caseColumn:
      return (field.caseSensitivity() == Qt::CaseSensitive) ? true : false;
      break;
    default:
      break;
    }

    return (index.row()+1) * (index.column()+1);
  case Qt::ToolTipRole:

    switch (index.column())
    {
    case typeColumn:
      return tr("Field type.");
      break;
    case nameColumn:
      return tr("Field name.");
      break;
    case ascendingColumn:
      return tr("Sort order, Ascending (checked) or Descending.");
      break;
    case caseColumn:
      return tr("Case sensitive (checked) or insensitive.");
      break;
    default:
      return QVariant();
      break;
    }
  default:
    break;
  }
  return QVariant();
}

QVariant TableSortFieldTableModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if( role != Qt::DisplayRole )
  {
    return QVariant();
  }
  if (orientation == Qt::Vertical)
  {
    return section + 1;
  }

  switch (section)
  {
  case typeColumn:
    return "Type";
    break;
  case nameColumn:
    return "Field";
    break;
  case ascendingColumn:
    return "Ascending";
    break;
  case caseColumn:
    return "Case Sensitive";
    break;
  default:
    return QVariant();
    break;
  }

  return QVariant();
}

Qt::ItemFlags TableSortFieldTableModel::flags( const QModelIndex &index ) const
{
  if(!index.isValid())
    return Qt::ItemIsEnabled;

  switch (index.column())
  {
  case typeColumn :
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    break;
  case nameColumn :
  case ascendingColumn :
  case caseColumn :
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    break;
  default:
    break;
  }

  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

void TableSortFieldTableModel::updateRow(int row, const TableSortField& field)
{
  if (0 <= row && row < m_sortFields.size())
  {
    m_sortFields[row] = field;
  }
}

void TableSortFieldTableModel::insertRow(int row, const TableSortField& field)
{
  if (row < 0)
  {
    row = 0;
  } else if (row > m_sortFields.size())
  {
    row = m_sortFields.size();
  }

  beginInsertRows(QModelIndex(), row, row);
  m_sortFields.insert(row, field);
  endInsertRows();
}

void TableSortFieldTableModel::copyRow(int row)
{
  if (row < 0)
  {
    return;
  }
  insertRow(row, m_sortFields.at(row));
}

void TableSortFieldTableModel::removeRow(int row)
{
  if (0 <= row && row < m_sortFields.size())
  {
    beginRemoveRows(QModelIndex(), row, row);
    m_sortFields.removeAt(row);
    endRemoveRows();
  }
}

void TableSortFieldTableModel::clear()
{
  if (m_sortFields.size() > 0)
  {
    beginRemoveRows(QModelIndex(), 0, m_sortFields.size() - 1);
    m_sortFields.clear();
    endRemoveRows();
  }
}

void TableSortFieldTableModel::add(const QList<TableSortField>& list)
{
  if (list.size() > 0)
  {
    int row = m_sortFields.size();
    beginInsertRows(QModelIndex(), row, row + list.size() - 1);
    for (int i=0; i<list.size(); ++i)
    {
      m_sortFields.insert(row + i, list.at(i));
    }
    endInsertRows();
  }
}


void TableSortFieldTableModel::moveRowUp(int row)
{
  if (0 < row && row < m_sortFields.size())
  {
    if (beginMoveRows(QModelIndex(), row, row, QModelIndex(), row- 1))
    {
      m_sortFields.move(row, row - 1);
      endMoveRows();
    }
  }
}

void TableSortFieldTableModel::moveRowDown(int row)
{
  if (0 <= row && row < m_sortFields.size() - 1)
  {
    // Yes, I really do need to use row+2, odd as it is!
    if (beginMoveRows(QModelIndex(), row, row, QModelIndex(), row + 2))
    {
      m_sortFields.move(row, row + 1);
      endMoveRows();
    }
  }
}

int TableSortFieldTableModel::size() const
{
  return m_sortFields.size();
}

bool TableSortFieldTableModel::containsFieldName(const QString& name) const
{
  for (int i=0; i<m_sortFields.size(); ++i)
  {
    if (name.compare(m_sortFields.at(i).fieldName(), Qt::CaseInsensitive) == 0) {
      return true;
    }
  }
  return false;
}

