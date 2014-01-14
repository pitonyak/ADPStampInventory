#include "genericdatacollectiontablemodel.h"

GenericDataCollectionTableModel::GenericDataCollectionTableModel(QObject *parent) :
  QAbstractTableModel(parent)
{
}

int GenericDataCollectionTableModel::rowCount( const QModelIndex & parent) const
{
    return parent.isValid() ? 0 : m_collection.valueCount();
}

int GenericDataCollectionTableModel::columnCount( const QModelIndex &) const
{
  return m_collection.getPropertyNameCount();
}

bool GenericDataCollectionTableModel::setData ( const QModelIndex & index, const QVariant & value, int role )
{
    (void) value;
  if (role == Qt::EditRole)
  {
    //SimpleLoggerRoutingInfo& info = m_routings[index.row()];
    QString s;
    switch (index.column())
    {
#if 0
    case enabledColumn:
      info.setEnabled(value.toBool());
      break;
    case nameColumn:
      info.setName(value.toString());
      break;
    case locationRegExpColumn:
      if (!info.setLocationRegExp(value.toString()))
      {
        // TODO: Print an error!
        return false;
      }
      break;
    case messageRegExpColumn:
      if (!info.setMessageRegExp(value.toString()))
      {
        // TODO: Print an error!
        return false;
      }
      break;
    case routFileColumn:
      info.setRouting(SimpleLoggerRoutingInfo::RouteFile, value.toBool());
      break;
    case routScreenColumn:
      info.setRouting(SimpleLoggerRoutingInfo::RouteEmit, value.toBool());
      break;
    case routDebugColumn:
      info.setRouting(SimpleLoggerRoutingInfo::RouteQDebug, value.toBool());
      break;
#endif
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


QVariant GenericDataCollectionTableModel::data( const QModelIndex &index, int role ) const
{
  //const SimpleLoggerRoutingInfo& info = m_routings[index.row()];
  switch( role )
  {
  case Qt::DisplayRole:
    switch (index.column())
    {
#if 0
    case enabledColumn:
      return info.isEnabled();
      break;
    case nameColumn:
      return info.getName();
      break;
    case locationRegExpColumn:
      return info.getLocationRegExpString();
      break;
    case messageRegExpColumn:
      return info.getMessageRegExpString();
      break;
    case routFileColumn:
      return info.isRoutingOn(SimpleLoggerRoutingInfo::RouteFile);
      break;
    case routScreenColumn:
      return info.isRoutingOn(SimpleLoggerRoutingInfo::RouteEmit);
      break;
    case routDebugColumn:
      return info.isRoutingOn(SimpleLoggerRoutingInfo::RouteQDebug);
      break;
    case componentColumn:
      return info.formatMessage(tr("message"), QString(QObject::tr("%1:%2")).arg(__FILE__, QString::number(__LINE__)), QDateTime::currentDateTime(), SimpleLoggerRoutingInfo::ErrorMessage, 1);
      break;
    case levelsColumn:
      return info.getLevelsAsString();
    {

    }
      break;
#endif
    default:
      // No other column is valid
      break;
    }
    return (index.row()+1) * (index.column()+1);

  case Qt::EditRole:
    switch (index.column())
    {
#if 0
    case enabledColumn:
      return info.isEnabled();
      break;
    case nameColumn:
      return info.getName();
      break;
    case locationRegExpColumn:
      return info.getLocationRegExpString();
      break;
    case messageRegExpColumn:
      return info.getMessageRegExpString();
      break;
    case routFileColumn:
      return info.isRoutingOn(SimpleLoggerRoutingInfo::RouteFile);
      break;
    case routScreenColumn:
      return info.isRoutingOn(SimpleLoggerRoutingInfo::RouteEmit);
      break;
    case routDebugColumn:
      return info.isRoutingOn(SimpleLoggerRoutingInfo::RouteQDebug);
      break;
#endif
    default:
      break;
    }

    return (index.row()+1) * (index.column()+1);
  case Qt::ToolTipRole:

    switch (index.column())
    {
#if 0
    case enabledColumn:
      return tr("Enable or disable this routing.");
      break;
    case nameColumn:
      return tr("User recognizable name used only in this dialog.");
      break;
    case locationRegExpColumn:
      return tr("Regular expression pattern used to match the message source location.");
      break;
    case messageRegExpColumn:
      return tr("Regular expression pattern used to match the unformatted message.");
      break;
    case componentColumn:
      return tr("Components printed as part of the message.");
      break;
    case levelsColumn:
      return tr("Only messages with level less than or equal to the level are passed.");
      break;
      break;
    case routFileColumn:
      return tr("Send log messages to the log file.");
      break;
    case routScreenColumn:
      return tr("Send log messages to all log listeners such as the screen.");
      break;
    case routDebugColumn:
      return tr("Use qDebug to send log messages to the console.");
      break;
#endif
    default:
      return QVariant();
      break;
    }
  default:
    break;
  }
  return QVariant();
}

QVariant GenericDataCollectionTableModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if( role != Qt::DisplayRole )
  {
    return QVariant();
  }
  const char* headers[] = {"Enabled", "Name", "Loc Reg Ex", "Msg Reg Ex", "To File", "To Screen", "To Console", "Levels", "Message"};
  if (orientation == Qt::Vertical)
  {
    return section + 1;
  }

  return (section < (int)(sizeof(headers)/sizeof(*headers))) ? QString(tr(headers[section])) : QVariant();
}

Qt::ItemFlags GenericDataCollectionTableModel::flags( const QModelIndex &index ) const
{
  if(!index.isValid())
    return Qt::ItemIsEnabled;

  // TODO: Based on column
  switch (index.column())
  {
#if 0
  case enabledColumn:
  case nameColumn:
  case locationRegExpColumn:
  case messageRegExpColumn:
  case routFileColumn:
  case routScreenColumn:
  case routDebugColumn:
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    break;
#endif
  default:
    break;
  }

  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}
