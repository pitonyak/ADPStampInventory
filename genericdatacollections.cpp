#include "genericdatacollections.h"

GenericDataCollections::GenericDataCollections(QObject *parent) :
    QObject(parent)
{
}

void GenericDataCollections::addCollection(const QString& name, GenericDataCollection* collection)
{
    Q_ASSERT_X(!name.isEmpty() && !name.isNull(), "GenericDataCollections::addCollection", "Cannot add an unnamed collection to the list.");
    Q_ASSERT_X(collection != nullptr, "GenericDataCollections::addCollection", "Cannot add a null collection to the list.");
    QString lowerName = name.toLower();

    Q_ASSERT_X(!m_tables.contains(lowerName), "GenericDataCollections::addCollection", qPrintable(QString("Collection named %1 already exists.").arg(name)));
    if (!m_tables.contains(lowerName))
    {
        m_names << name;
        m_tables[lowerName] = collection;
    }
}

GenericDataObject* GenericDataCollections::getObject(const QString& tableName, const int id) {
    GenericDataCollection* table = getTable(tableName);
    return (table != nullptr) ? table->getObjectById(id) : nullptr;
}

QVariant GenericDataCollections::getValue(const QString& tableName, const int id, const QString& fieldName)
{
  GenericDataCollection* table = getTable(tableName);
  if (table != nullptr)
  {

      if (table->containsValue(id, fieldName)) {
          return table->getObjectById(id)->getValueNative(fieldName);
      } else if (table->containsProperty(fieldName)) {
          TypeMapper mapper;
          return QVariant(QMetaType(table->getPropertyTypeMeta(fieldName)));
      }
  }
  return QVariant();
}
