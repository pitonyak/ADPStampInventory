#include "genericdatacollectionstablemodel.h"

#include "describesqltables.h"
#include "dbtransactionhandler.h"

#include <QLocale>
#include <QQueue>
#include <QSqlQuery>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QCollator>
#include <algorithm>
#include <QDebug>

GenericDataCollectionsTableModel::GenericDataCollectionsTableModel(const bool useLinks, const QString& tableName, GenericDataCollections& tables, DescribeSqlTables& schema, int defaultSourceId, QObject *parent) :
  QAbstractTableModel(parent),
  m_useLinks(useLinks), m_isTracking(true),
  m_tableName(tableName), m_tables(tables), m_table(nullptr), m_schemas(schema), m_defaultSourceId(defaultSourceId)
{
  m_table = m_tables.getTable(tableName);
  Q_ASSERT_X(m_table != nullptr, "GenericDataCollectionsTableModel::GenericDataCollectionsTableModel", qPrintable(QString("Collection does not contain table %1").arg(tableName)));
  const DescribeSqlTable* tableSchema = m_schemas.getTableByName(tableName);
  if (tableSchema != nullptr) {
      m_schema = *tableSchema;
  }
}

QModelIndex GenericDataCollectionsTableModel::getIndexByRowCol(int row, int col) const
{
  if (col < 0) {
    col = 0;
  }
  if (col >= m_table->getPropertNames().size()) {
    col = m_table->getPropertNames().size() - 1;
  }
  if (row < 0) {
    row = 0;
  }
  if (row >= m_table->getObjectCount())
  {
    row = m_table->getObjectCount() - 1;
  }
  //return (col >= 0 && row >= 0) ? createIndex(row, col) : QModelIndex();
  return (col >= 0 && row >= 0) ? index(row, col) : QModelIndex();
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

      QString fieldName = m_table->getPropertyName(index.column());
      const DescribeSqlField* fieldSchema = m_schema.getFieldByName(fieldName);
      if (m_useLinks && fieldSchema->isLinkField())
      {
        QString cacheId = m_linkCache.buildCacheIdentifier(fieldSchema->getLinkTableName(), fieldSchema->getLinkDisplayField());
        int refId = m_linkCache.getIdForCachedValue(cacheId, value.toString());

        if (refId >= 0) {
          object->setValueNative(fieldName, refId);
        }
      }
      else
      {
        // Doing nothing special for fieldSchema->isConcatenatedFields().
        object->setValueNative(fieldName, value);
      }
      bool updateSourceId = (m_defaultSourceId >= 0 && fieldName.compare("bookvalue", Qt::CaseInsensitive) == 0 && m_table->containsProperty("sourceid") && object->getValueNative("sourceid") != m_defaultSourceId);
      if (updateSourceId) {
        // Cannot just set the value or it will not update the DB, just the display.
        // If I do it the obvious way, it acts as a two "undo" actions. So, just set it
        // and handle it when I push the changes.
        //QModelIndex sourceIndex = getIndexByRowCol(index.row(), m_table->getPropertyIndex("sourceid"));
        //setData(sourceIndex, m_defaultSourceId, role);
        object->setValueNative("sourceid", m_defaultSourceId);
      }
      if (fieldName.compare("updated", Qt::CaseInsensitive) != 0 && m_table->containsProperty("updated"))
      {
        // This sets the data from a visual standpoint, but, this does not cause the data to be
        // updated in the DB because data is pushed when tracked changes are saved.
        object->setValueNative("updated", QDateTime::currentDateTime());
      }
      // I should probably check fieldSchema->isConcatenatedFields() and not bother in that case
      // as far as tracking goes, but, the update method will ignore it anyway and it allows undo to work
      // as expected I suppose.
      if (isTracking())
      {
        if (!updateSourceId) {
          m_changeTracker.push(index.row(), index.column(), fieldName, ChangedObjectBase::Edit, object->clone(), originalObject);
        } else {
          // Doing it this way groups the two field changes as a single "undo" group.
          // OK, so I use the same changed and original object both times, but, I don't think that it will matter.
          QStack<ChangedObject<GenericDataObject>*> * lastChanges = new QStack<ChangedObject<GenericDataObject>*>();
          lastChanges->push(new ChangedObject<GenericDataObject>(index.row(), m_table->getPropertyIndex("sourceid"), "sourceid", ChangedObjectBase::Edit, object->clone(), originalObject->clone()) );
          lastChanges->push(new ChangedObject<GenericDataObject>(index.row(), index.column(), fieldName, ChangedObjectBase::Edit, object->clone(), originalObject) );
          m_changeTracker.push(lastChanges);
        }
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

    if (role == Qt::EditRole)
    {
      if (m_useLinks && fieldSchema->isLinkField())
      {
        QVariant currentValue = data(index, Qt::DisplayRole);
        QStringList returnList;
        returnList << currentValue.toString();
        returnList.append(getLinkEditValues(fieldSchema->getLinkTableName(), fieldSchema->getLinkDisplayField()));
        return returnList;
      }
      else if (fieldSchema->isConcatenatedFields())
      {
        QVariant currentValue = data(index, Qt::DisplayRole);
        QStringList returnList;
        returnList << currentValue.toString();
        // Note sure if this is correct?
        returnList << getConcatenatedValues(object, fieldSchema->getLinkDisplayField());
        return returnList;
      }
      return object->getValue(m_table->getPropertyName(index.column()));
    }
    else if (role == Qt::DisplayRole)
    {
      // Only display currency symbol for display, not for edit!
      if (fieldSchema->isCurrency())
      {
        QLocale locale;
        return locale.toCurrencyString(object->getValueNative(fieldName).toDouble());
      }
      else if (m_useLinks && fieldSchema->isLinkField())
      {
        int linkId = object->getInt(m_table->getPropertyName(index.column()));
        return getLinkValues(fieldSchema->getLinkTableName(), linkId, fieldSchema->getLinkDisplayField());
      }
      else if (fieldSchema->isConcatenatedFields()) {
        return getConcatenatedValues(object, fieldSchema->getLinkDisplayField());
      }
      return object->getValue(m_table->getPropertyName(index.column()));
    }
  }
  return QVariant();
}

QStringList GenericDataCollectionsTableModel::getLinkEditValues(const QString& tableName, QStringList fields) const
{
  // Do we already have the list?
  QString cacheId = m_linkCache.buildCacheIdentifier(tableName, fields);
  if (m_linkCache.hasCachedList(cacheId)) {
    return m_linkCache.getCachedList(cacheId);
  }

  const DescribeSqlTable* schema = m_schemas.getTableByName(tableName);
  const GenericDataCollection* table = m_tables.getTable(tableName);

  // Build any internal cache IDs for the fields that we will use.
  QHash<int, QString> innerFieldToCacheId;
  for (int iField=0; iField < fields.size(); ++iField)
  {
    QString fieldName = fields.at(iField);
    const DescribeSqlField* fieldSchema = schema->getFieldByName(fieldName);
    if (fieldSchema->isLinkField())
    {
      QString fieldCacheId = m_linkCache.hasCacheIdentifier(tableName, fieldName) ? m_linkCache.getCacheIdentifer(tableName, fieldName) : const_cast<LinkedFieldSelectionCache&>(m_linkCache).setCacheIdentifier(tableName, fieldName, fieldSchema->getLinkTableName(), fieldSchema->getLinkDisplayField());
      innerFieldToCacheId[iField] = fieldCacheId;
    }
  }

  // List that will be returned
  QStringList list;

  for (int iRow=0; iRow < table->rowCount(); ++iRow)
  {
    const GenericDataObject* row = table->getObjectByRow(iRow);
    int rowId = row->getInt("id");
    const QString* cachedValue = m_linkCache.getCacheValue(cacheId, rowId);

    if (cachedValue != nullptr)
    {
      list << *cachedValue;
    } else {

      QString s = "";
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
          QString fieldCacheId = innerFieldToCacheId[iField];
          // Test for a cached value
          const QString* cachedValue = m_linkCache.getCacheValue(fieldCacheId, linkId);
          if (cachedValue != nullptr)
          {
            s = s.append(*cachedValue);
          }
          else
          {
            s.append(getLinkValues(fieldSchema->getLinkTableName(), linkId, fieldSchema->getLinkDisplayField()));
          }
        }
        else
        {
          s = s.append(row->getString(fieldName));
        }
      }
      const_cast<LinkedFieldSelectionCache&>(m_linkCache).addCacheValue(cacheId, rowId, s);
      list << s;
    }
  }

  // Sort this list before storing it.
  // Use a QCollator so that numbers and strings sort well.
  QCollator collator;
  collator.setNumericMode(true);
  collator.setCaseSensitivity(Qt::CaseInsensitive);

  // And this is why I like lambda expressions, I can define my sort function right here!
  auto mySort = ([collator](const QString& a, const QString& b){ return collator.compare(a,b) < 0; } );

  // Use the standard sort to sort the list in ascending order.
  std::sort(list.begin(), list.end(), mySort);

  const_cast<LinkedFieldSelectionCache&>(m_linkCache).setCachedList(cacheId, list);
  return list;
}

QString GenericDataCollectionsTableModel::getConcatenatedValues(const GenericDataObject* object, const QStringList &fields) const
{
  QString s = "";
  if (object != nullptr) {
    for (int i=0; i<fields.size(); ++i)
    {
      if (i > 0) {
        s = s.append('/');
      }
      // Get the value based on the field name.
      s = s.append(object->getString(fields.at(i)));
    }
  }
  return s;
}

QString GenericDataCollectionsTableModel::getLinkValues(const QString& tableName, const int id, const QStringList& fields) const
{
  QString cacheId = m_linkCache.buildCacheIdentifier(tableName, fields);
  const QString* cachedValue = m_linkCache.getCacheValue(cacheId, id);
  if (cachedValue != nullptr)
  {
    return *cachedValue;
  }

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
        QString fieldCacheId = m_linkCache.hasCacheIdentifier(tableName, fieldName) ? m_linkCache.getCacheIdentifer(tableName, fieldName) : const_cast<LinkedFieldSelectionCache&>(m_linkCache).setCacheIdentifier(tableName, fieldName, fieldSchema->getLinkTableName(), fieldSchema->getLinkDisplayField());
        const QString* cachedValue = m_linkCache.getCacheValue(fieldCacheId, linkId);
        if (cachedValue != nullptr)
        {
          s = s.append(*cachedValue);
        }
        else
        {
          s = s.append(getLinkValues(fieldSchema->getLinkTableName(), linkId, fieldSchema->getLinkDisplayField()));
        }
      }
      else
      {
        s = s.append(row->getString(fields.at(i)));
      }
    }
  }
  const_cast<LinkedFieldSelectionCache&>(m_linkCache).addCacheValue(cacheId, id, s);
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
  // Contains unique row numbers to prevent the same row from being included twice.
  QSet<int> rowSet;
  for (int i=0; i<list.size(); ++i)
  {
    //qDebug(qPrintable(QString("Checking entry %1 / %2").arg(i).arg(list.size())));
    if (!rowSet.contains(list.at(i).row())) {
      // This row not included, so, add it.
      rowSet.insert(list.at(i).row());
      rows.append(list.at(i).row());
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

void GenericDataCollectionsTableModel::incCell(const QModelIndex& index, int incrementValue, const bool setUpdated)
{
  incrementCell(index, incrementValue, setUpdated);
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
      qDebug() << "Removing row "<< row;
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


bool GenericDataCollectionsTableModel::saveTrackedChanges(const QString& tableName, GenericDataCollection &data, QSqlDatabase &db, const DescribeSqlTables& schema)
{
  bool trackState = isTracking();
  setTracking(false);
  bool errorOccurred = false;

  const DescribeSqlTable* tableSchema = schema.getTableByName(tableName);
  if (tableSchema == nullptr)
  {
    qDebug("tableSchema is NULL");
  }

  bool setUpdateField = (tableSchema != nullptr && tableSchema->containsField("updated"));

  DBTransactionHandler transactionHandler(db);

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
            //qDebug("Add record to the DB!");
            GenericDataObject* newData = bottomObject->getNewData();
            if (newData != nullptr)
            {
              QSqlQuery query(db);
              QString s = QString("INSERT INTO %1 (").arg(tableName);
              QString sValues = " VALUES (";
              bool aFieldAdded = false;
              for (int iCol=0; iCol<data.getPropertNames().size(); ++iCol)
              {
                // Ignore any "fake" fields that are just for display purposes.
                if (tableSchema == nullptr || !tableSchema->isConcatenatedFields(data.getPropertyName(iCol))) {
                  if (aFieldAdded) {
                    s = s.append(", ");
                    sValues = sValues.append(", ");
                  }
                  s = s.append(data.getPropertyName(iCol));
                  sValues = sValues.append(":").append(data.getPropertyName(iCol));
                  aFieldAdded = true;
                }
              }
              QString sSQL = s.append(") ").append(sValues).append(")");
              query.prepare(sSQL);
              for (int iCol=0; iCol<data.getPropertNames().size(); ++iCol)
              {
                if (tableSchema == nullptr || !tableSchema->isConcatenatedFields(data.getPropertyName(iCol))) {
                  if (!newData->containsValue(data.getPropertyName(iCol)))
                  {
                      TypeMapper mapper;
                      query.bindValue(QString(":%1").arg(data.getPropertyName(iCol)), QVariant(mapper.metaToVariantType(data.getPropertyTypeMeta(iCol))));
                  } else {
                      // Assume that it converts to the correct type!
                      query.bindValue(QString(":%1").arg(data.getPropertyName(iCol)), newData->getValueNative(data.getPropertyName(iCol)));
                  }
                }
              }
              if (!query.exec())
              {
                qDebug("Failed to add row");
              }
            }
          }
          else if (bottomObject->getChangeType() == ChangedObjectBase::Delete)
          {
            GenericDataObject* oldData = bottomObject->getOldData();
            if (oldData != nullptr)
            {
              QSqlQuery query(db);
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

            if (tableSchema == nullptr || !tableSchema->isConcatenatedFields(fieldName))
            {
              // Info contains the modified field name!
              GenericDataObject* oldData = bottomObject->getOldData();
              GenericDataObject* newData = bottomObject->getNewData();
              if (oldData != nullptr && newData != nullptr)
              {
                QSqlQuery query(db);
                QString s = setUpdateField ? QString("UPDATE %1 SET %2=:%2, updated=:updated WHERE %3=:id").arg(tableName).arg(fieldName).arg("id") : QString("UPDATE %1 SET %2=:%2 WHERE %3=:id").arg(tableName).arg(fieldName).arg("id");
                query.prepare(s);
                query.bindValue(":id", oldData->getInt("id"));
                if (setUpdateField) {
                  QDateTime now = QDateTime::currentDateTime();
                  query.bindValue(":updated", now);
                  GenericDataObject* currentObj = data.getObjectById(newData->getInt("id"));
                  if (currentObj != nullptr)
                  {
                    currentObj->setValueNative("updated", now);
                  }
                }
                qDebug() << "[" << oldData->getInt("id") << "] where id = " << s;

                if (!newData->containsValue(fieldName))
                {
                    TypeMapper mapper;
                    query.bindValue(QString(":%1").arg(fieldName), QVariant(mapper.metaToVariantType(data.getPropertyTypeMeta(fieldName))));
                } else {
                    // Assume that it converts to the correct type!
                    query.bindValue(QString(":%1").arg(fieldName), newData->getValueNative(fieldName));
                }
                if (!query.exec())
                {
                  qDebug("Failed to update row");
                }
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
  if (errorOccurred)
  {
    transactionHandler.rollback();
  }
  else
  {
    transactionHandler.commit();
  }
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
          qDebug() << "Delete row " << row;
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
            qDebug() << "Insert row " << row;
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

QString GenericDataCollectionsTableModel::incrementScottNumber(const QString& scott) const
{
  // This is eather the scotts number, or something like
  // <country>/<Scotts>/<type>

  QRegularExpression reDigit("\\d");

  // If the number does not contain at least one number
  // this is not a scott number of any sort.
  if (!scott.contains(reDigit)) {
    return scott;
  }

  if (scott.contains('/')) {
    // Assume that the scott number is the first one that contains a numeric value.
    // The default is to keep empty parts.
    QStringList list = scott.split('/');
    for (int i=0; i<list.size(); ++i) {
      QString x = list.at(i);
      if (x.contains(reDigit)) {
        list.replace(i, incrementScottNumber(x));
        return list.join('/');
      }
    }
    // It should be impossible to arrive here.
    return scott;
  }

  QString scottReg = "^(.*?)(\\d*)(\\D*)$";

  QRegularExpression re(scottReg);
  QRegularExpressionMatch match = re.match(scott);
  if (match.hasMatch())
  {
    QString lead = match.captured(1);   // Leading portion
    QString middle = match.captured(2); // Only numbers
    QString tail = match.captured(3);   // trailing portion with no number.
    if (middle.isEmpty())
    {
      // This should NEVER happen.
      return scott;
    }
    if (tail.isEmpty())
    {
      return QString("%1%2").arg(lead).arg(1 + middle.toLongLong());
    }
    if (tail.at(tail.length() - 1).isLetter())
    {
      QChar c(tail.at(tail.length() - 1).unicode() + 1);
      tail.truncate(tail.length() - 1);
      return QString("%1%2%3%4").arg(lead).arg(middle).arg(tail).arg(c);
    }
  }
  return scott;
}

void GenericDataCollectionsTableModel::incrementCell(const int row, const int col, int incrementValue, const bool setUpdated)
{
  incrementCell(getIndexByRowCol(row, col), incrementValue, setUpdated);
}

void GenericDataCollectionsTableModel::incrementCell(const QModelIndex& index, int incrementValue, const bool setUpdated)
{
  if (!index.isValid()) {
    qDebug("Invalid index in incrementCell");
    return;
  }
  GenericDataObject* rowData = m_table->getObjectByRow(index.row());
  QString columnName = m_table->getPropertyName(index.column());
  QVariant cellValue = rowData->getValueNative(columnName);
  GenericDataObject* originalRowData = rowData->clone();

  rowData->increment(columnName, incrementValue, cellValue);
  rowData->setValueNative(columnName, cellValue);

  if (setUpdated) {
    if (rowData->containsValue("updated") && rowData->isDateTime("updated")) {
      rowData->setValueNative("updated", QDateTime::currentDateTime());
    }
  }

  if (m_isTracking) {
    m_changeTracker.push(index.row(), index.column(), columnName, ChangedObjectBase::Edit, rowData->clone(), originalRowData);
  } else {
    delete originalRowData;
  }

  dataChanged(index, index);
}


void GenericDataCollectionsTableModel::copyCell(const int fromRow, const int fromCol, const int toRow, const int toCol, const bool setUpdated)
{
  copyCell(getIndexByRowCol(fromRow, fromCol), getIndexByRowCol(toRow, toCol), setUpdated);
}

void GenericDataCollectionsTableModel::copyCell(const QModelIndex& fromIndex, const QModelIndex& toIndex, const bool setUpdated)
{
  if (!fromIndex.isValid()) {
    qDebug() << "Invalid from index in copyCell";
    return;
  }
  if (!toIndex.isValid()) {
    qDebug() << "Invalid to index in copyCell";
    return;
  }
  if (fromIndex.row() == toIndex.row() && fromIndex.column() == toIndex.column()) {
    qDebug() << "Cannot copy a cell onto itself.";
    return;
  }

  GenericDataObject* fromRow = m_table->getObjectByRow(fromIndex.row());
  GenericDataObject* toRow = m_table->getObjectByRow(toIndex.row());
  GenericDataObject* originalToRow = toRow->clone();

  QString fromColumnName = m_table->getPropertyName(fromIndex.column());
  QString toColumnName = m_table->getPropertyName(toIndex.column());

  QVariant fromValue = fromRow->getValueNative(fromColumnName);
  QVariant toValue = toRow->getValueNative(toColumnName);

  if (fromValue == toValue) {
    qDebug() << "From and To values are the same, not copying cell";
    return;
  }

  toRow->setValueNative(toColumnName, fromValue);

  if (setUpdated) {
    if (toRow->containsValue("updated") && toRow->isDateTime("updated")) {
      toRow->setValueNative("updated", QDateTime::currentDateTime());
    }
  }

  if (m_isTracking)
  {
    m_changeTracker.push(toIndex.row(), toIndex.column(), toColumnName, ChangedObjectBase::Edit, toRow->clone(), originalToRow);
  } else {
    delete originalToRow;
  }

  dataChanged(toIndex, toIndex);
}

QList<int> GenericDataCollectionsTableModel::duplicateRows(const QModelIndexList& list, const bool autoIncrement, const bool setUpdated, const bool appendChar, const char charToAppend)
{
  QList<int> addedIds;
  qDebug() << "duplicateRows " << list.size();

  // Will contain a sorted list or row numbers in the list.
  QList<int> rows;
  getRowsAscending(list, rows);

  // Will contain a list or rows in sorted order.
  QQueue<GenericDataObject*> dataToCopy;
  for (int i=0; i<rows.size(); ++i)
  {
    //qDebug(qPrintable(QString("Copying %1").arg(i)));
    dataToCopy.enqueue(m_table->getObjectByRow(rows.at(i)));
  }

  //qDebug(qPrintable(QString("duplicateRows %1").arg(rows.size())));
  if (!rows.isEmpty())
  {
    int largestId = m_table->getLargestId();
    QStack<ChangedObject<GenericDataObject>*> * lastChanges = m_isTracking ? new QStack<ChangedObject<GenericDataObject>*>() : nullptr;
    while (!dataToCopy.isEmpty())
    {
      GenericDataObject* oldData = dataToCopy.dequeue();
      GenericDataObject* newData = oldData->clone();
      int nextId = ++largestId;
      addedIds << nextId;
      newData->setValueNative("id", nextId);
      if (autoIncrement || appendChar) {
        if (newData->containsValue("scott")) {
          if (autoIncrement) {
            newData->setValueNative("scott", incrementScottNumber(newData->getString("scott")));
          }
          if (appendChar) {
            newData->setValueNative("scott", newData->getString("scott").append(charToAppend));
          }
        }
      }
      if (setUpdated) {
        if (newData->containsValue("updated") && newData->isDateTime("updated")) {
          newData->setValueNative("updated", QDateTime::currentDateTime());
        }
      }
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
  return addedIds;
}

