#include "tablesortfieldtablemodel.h"
#include "xmlutility.h"

#define TypeColumn 0
#define NameColumn 1
#define OrderColumn 2
#define CaseColumn 3
#define NumColumns 4

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
  return NumColumns;
}

bool TableSortFieldTableModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
  (void) value;
  if (role == Qt::EditRole)
  {
    TableSortField* field = m_collection[index.row()];
    QString s;
    switch (index.column())
    {
    case TypeColumn:
      field->setFieldType(m_mapper.getMetaType(value.toString()));
      break;
    case NameColumn:
      field->setFieldName(value.toString());
      break;
    case OrderColumn:
      field->setSortOrder(field->sortOrderFromName(value.toString()));
      break;
    case CaseColumn:
      field->setCase(XMLUtility::stringToCase(value.toString()));
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
  TableSortField* field = m_collection[index.row()];
  switch( role )
  {
  case Qt::DisplayRole:
    switch (index.column())
    {
    case TypeColumn:
      m_mapper.getMetaName(field->fieldType());
      break;
    case NameColumn:
      return field->fieldName();
      break;
    case OrderColumn:
      return field->sortOrderToName(field->sortOrder());
      break;
    case CaseColumn:
      return XMLUtility::caseToString(field->caseSensitivity());
      break;
    default:
      // No other column is valid
      break;
    }
    return (index.row()+1) * (index.column()+1);

  case Qt::EditRole:
    switch (index.column())
    {
    case TypeColumn:
      m_mapper.getMetaName(field->fieldType());
      break;
    case NameColumn:
      return field->fieldName();
      break;
    case OrderColumn:
      return field->sortOrder();
      break;
    case CaseColumn:
      return field->caseSensitivity();
      break;
    default:
      break;
    }

    return (index.row()+1) * (index.column()+1);
  case Qt::ToolTipRole:

    switch (index.column())
    {
    case TypeColumn:
      return tr("Field type.");
      break;
    case NameColumn:
      return tr("Field name.");
      break;
    case OrderColumn:
      return tr("Sort order, Ascending / Descending.");
      break;
    case CaseColumn:
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
  case TypeColumn :
  case NameColumn :
  case OrderColumn :
  case CaseColumn :
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    break;
  default:
    break;
  }

  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
