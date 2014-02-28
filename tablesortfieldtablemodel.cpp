#include "tablesortfieldtablemodel.h"
#include "xmlutility.h"


const int TableSortFieldTableModel::nameColumn = 0;
const int TableSortFieldTableModel::ascendingColumn = 1;
const int TableSortFieldTableModel::caseColumn = 2;
const int TableSortFieldTableModel::typeColumn = 3;
const int TableSortFieldTableModel::numColumns = 4;


TableSortFieldTableModel::TableSortFieldTableModel(QObject *parent) :
  QAbstractTableModel(parent)
{
}

int TableSortFieldTableModel::rowCount( const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : m_collection.count();
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
    TableSortField& field = m_collection[index.row()];
    QString s;
    switch (index.column())
    {
    case typeColumn:
      field.setFieldType(m_mapper.getMetaType(value.toString()));
      break;
    case nameColumn:
      field.setFieldName(value.toString());
      break;
    case ascendingColumn:
      field.setSortOrder(field.sortOrderFromName(value.toString()));
      break;
    case caseColumn:
      field.setCase(XMLUtility::stringToCase(value.toString()));
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
  const TableSortField& field = m_collection[index.row()];
  switch( role )
  {
  case Qt::DisplayRole:
    switch (index.column())
    {
    case typeColumn:
      m_mapper.getMetaName(field.fieldType());
      break;
    case nameColumn:
      return field.fieldName();
      break;
    case ascendingColumn:
      return field.sortOrderToName(field.sortOrder());
      break;
    case caseColumn:
      return XMLUtility::caseToString(field.caseSensitivity());
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
      m_mapper.getMetaName(field.fieldType());
      break;
    case nameColumn:
      return field.fieldName();
      break;
    case ascendingColumn:
      return field.sortOrder();
      break;
    case caseColumn:
      return field.caseSensitivity();
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
      return tr("Sort order, Ascending / Descending.");
      break;
    case caseColumn:
      return tr("Case sensitive or insensitive.");
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
  const char* headers[] = {"Type", "Name", "Order", "Case"};
  if (orientation == Qt::Vertical)
  {
    return section + 1;
  }

  return (section < (int)(sizeof(headers)/sizeof(*headers))) ? QString(tr(headers[section])) : QVariant();
}

Qt::ItemFlags TableSortFieldTableModel::flags( const QModelIndex &index ) const
{
  if(!index.isValid())
    return Qt::ItemIsEnabled;

  switch (index.column())
  {
  case typeColumn :
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
  if (0 <= row && row < m_collection.count())
  {
    m_collection[row] = field;
  }
}

void TableSortFieldTableModel::insertRow(int row, const TableSortField& field)
{
  if (row < 0)
  {
    row = 0;
  } else if (row > m_collection.count())
  {
    row = m_collection.count();
  }

  beginInsertRows(QModelIndex(), row, row);
  m_collection.insert(row, field);
  endInsertRows();
}

void TableSortFieldTableModel::copyRow(int row)
{
  if (row < 0)
  {
    return;
  }
  insertRow(row, m_collection.at(row));
}

void TableSortFieldTableModel::removeRow(int row)
{
  if (0 <= row && row < m_collection.count())
  {
    beginRemoveRows(QModelIndex(), row, row);
    m_collection.removeAt(row);
    endRemoveRows();
  }
}

void TableSortFieldTableModel::clear()
{
  if (m_collection.count() > 0)
  {
    beginRemoveRows(QModelIndex(), 0, m_collection.count() - 1);
    m_collection.clear();
    endRemoveRows();
  }
}
void TableSortFieldTableModel::moveRowUp(int row)
{
  if (0 < row && row < m_collection.count())
  {
    if (beginMoveRows(QModelIndex(), row, row, QModelIndex(), row- 1))
    {
      m_collection.move(row, row - 1);
      endMoveRows();
    }
  }
}

void TableSortFieldTableModel::moveRowDown(int row)
{
  if (0 <= row && row < m_collection.count() - 1)
  {
    // Yes, I really do need to use row+2, odd as it is!
    if (beginMoveRows(QModelIndex(), row, row, QModelIndex(), row + 2))
    {
      m_collection.move(row, row + 1);
      endMoveRows();
    }
  }
}

int TableSortFieldTableModel::count() const
{
  return m_collection.count();
}
