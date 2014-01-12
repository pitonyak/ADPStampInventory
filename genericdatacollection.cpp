#include "genericdatacollection.h"
#include "csvwriter.h"

#include <QMetaType>

GenericDataCollection::GenericDataCollection(QObject *parent) :
  QObject(parent)
{
}

void GenericDataCollection::appendObject(const int id, GenericDataObject* obj)
{
  removeObject(id);
  if (obj != nullptr)
  {
    m_objects.insert(id, obj);
  }
}

void GenericDataCollection::removeObject(const int id)
{
  if (hasObject(id))
  {
    delete m_objects.take(id);
  }
}

QVariant::Type GenericDataCollection::getPropertyType(const QString& name) const
{
  QString lowerCaseName = name.toLower();
  return m_LowerCasePropertyNameMap.contains(lowerCaseName) ? m_propertyTypes.at(m_LowerCasePropertyNameMap.value(lowerCaseName)) : QVariant::Invalid;
}

QString GenericDataCollection::getPropertyName(const QString& name) const
{
  QString lowerCaseName = name.toLower();
  return m_LowerCasePropertyNameMap.contains(lowerCaseName) ? m_propertyNames.at(m_LowerCasePropertyNameMap.value(lowerCaseName)) : "";
}

bool GenericDataCollection::appendPropertyName(const QString& name, const QVariant::Type pType)
{
  QString lowerCaseName = name.toLower();
  if (m_LowerCasePropertyNameMap.contains(lowerCaseName))
  {
    return false;
  }
  m_LowerCasePropertyNameMap.insert(lowerCaseName, m_propertyNames.count());
  m_propertyNames.append(name);
  m_propertyTypes.append(pType);
  return true;
}

bool GenericDataCollection::exportToCSV(CSVWriter& writer) const
{
  writer.clearHeader();
  for (int i=0; i<getPropertNames().count(); ++i)
  {
    writer.addHeader(getPropertyName(i), CSVColumn::variantTypeToMetaType(getPropertyType(i)));
  }
  writer.writeHeader();

  // Export the data ordered by ID
  QList<int> objKeys = m_objects.keys();
  qSort(objKeys);

  for (int idx=0; idx < objKeys.count(); ++idx)
  {
    CSVLine newLine;
    GenericDataObject* obj = m_objects.value(objKeys[idx]);
    for (int i=0; i<getPropertNames().count(); ++i)
    {
      if (obj->hasValue(getPropertyName(i)))
      {
        QMetaType::Type columnType = CSVColumn::variantTypeToMetaType(getPropertyType(i));
        bool qualified = (columnType == QMetaType::QString);
        newLine.append(CSVColumn(obj->getString(getPropertyName(i)), qualified, columnType));
      }
      else
      {
        newLine.append(CSVColumn("", false, QMetaType::QString));
      }
    }
    writer.write(newLine);
  }
#if 0
  QHashIterator<int, GenericDataObject*> iterator(m_objects);
  while (iterator.hasNext())
  {
    iterator.next();
    CSVLine newLine;
    for (int i=0; i<getPropertNames().count(); ++i)
    {
      if (iterator.value()->hasValue(getPropertyName(i)))
      {
        QMetaType::Type columnType = CSVColumn::variantTypeToMetaType(getPropertyType(i));
        bool qualified = (columnType == QMetaType::QString);
        newLine.append(CSVColumn(iterator.value()->getString(getPropertyName(i)), qualified, columnType));
      }
      else
      {
        newLine.append(CSVColumn("", false, QMetaType::QString));
      }
    }
    writer.write(newLine);
  }
#endif
  return true;
}
