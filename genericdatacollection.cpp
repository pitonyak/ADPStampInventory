#include "genericdatacollection.h"
#include "csvwriter.h"

#include <QMetaType>

GenericDataCollection::GenericDataCollection(QObject *parent) :
  QObject(parent)
{
}

GenericDataCollection::GenericDataCollection(const GenericDataCollection& obj) :
    QObject(nullptr)
{
    operator=(obj);
}

void GenericDataCollection::makeDummy()
{
  QStringList nameList;
  nameList << "Id" << "Name" << "Date" << "Time" << "Double" << "Bool";
  QList<QVariant::Type> typeList;
  typeList << QVariant::Int << QVariant::String << QVariant::Date << QVariant::Time
              << QVariant::Double << QVariant::Bool;
  for (int i=0; i<nameList.size(); ++i)
  {
    appendPropertyName(nameList.value(i), typeList.value(i));
  }
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

const GenericDataObject* GenericDataCollection::getObjectByValue(const QString& name, const QString& compareValue, const Qt::CaseSensitivity sensitive) const
{
  QString lowerCaseName = name.toLower();
  if (m_LowerCasePropertyNameMap.contains(lowerCaseName))
  {
    QHashIterator<int, GenericDataObject*> i(m_objects);
    while (i.hasNext())
    {
      i.next();
      if (i.value() != nullptr && i.value()->valueIs(lowerCaseName, compareValue, sensitive))
      {
        return i.value();
      }
    }
  }

  return nullptr;
}

int GenericDataCollection::countValues(const QString& name, const QString& compareValue, const Qt::CaseSensitivity sensitive) const
{
  int iCount = 0;
  QString lowerCaseName = name.toLower();
  if (m_LowerCasePropertyNameMap.contains(lowerCaseName))
  {
    QHashIterator<int, GenericDataObject*> i(m_objects);
    while (i.hasNext())
    {
      i.next();
      if (i.value() != nullptr && i.value()->valueIs(lowerCaseName, compareValue, sensitive))
      {
        ++iCount;
      }
    }
  }

  return iCount;
}

void GenericDataCollection::clear()
{
    QHashIterator<int, GenericDataObject*> i(m_objects);
    while (i.hasNext())
    {
        i.next();
        delete i.value();
    }
    m_objects.clear();
    m_propertyNames.clear();
    m_propertyTypes.clear();
    m_LowerCasePropertyNameMap.clear();
}

const GenericDataCollection& GenericDataCollection::operator=(const GenericDataCollection& obj)
{
    if (this != &obj)
    {
        clear();
        m_propertyNames = obj.m_propertyNames;
        m_propertyTypes = obj.m_propertyTypes;
        m_LowerCasePropertyNameMap = obj.m_LowerCasePropertyNameMap;

        QHashIterator<int, GenericDataObject*> i(obj.m_objects);
        while (i.hasNext())
        {
            i.next();
            if (i.value() != nullptr)
            {
                m_objects.insert(i.key(), i.value()->clone(this));
            }
        }
    }
    return *this;
}


bool GenericDataCollection::addSortField(const QString& name, const TableSortField::SortOrder order, Qt::CaseSensitivity sensitive)
{
    if (!hasProperty(name))
    {
        return false;
    }
    m_sortFields.append(new TableSortField(this, name.toLower(), m_LowerCasePropertyNameMap.value(name.toLower()), order, sensitive));
    return true;
}

void GenericDataCollection::clearSortFields()
{
    qDeleteAll(m_sortFields);
    m_sortFields.clear();
}

void GenericDataCollection::sort()
{
    m_sortedIDs = m_objects.keys();
    if (m_sortFields.count() == 0)
    {
        qSort(m_sortedIDs);
    }
    else
    {
        // TODO: Use the fields.
        // https://qt-project.org/forums/viewthread/4978
    }
}
