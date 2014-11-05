#include "genericdatacollection.h"
#include "csvwriter.h"

#include <QMetaType>
#include <QUrl>
#include <QUuid>

GenericDataCollection::GenericDataCollection(QObject *parent) :
  QObject(parent), m_largestId(-1), m_trackChanges(false)
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
  QList<QMetaType::Type> typeList;
  typeList << QMetaType::Int << QMetaType::QString << QMetaType::QDate << QMetaType::QTime
              << QMetaType::Double << QMetaType::Bool;
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
    m_sortedIDs.append(id);
    if (id > m_largestId)
    {
      m_largestId = id;
    }
  }
}

void GenericDataCollection::removeObject(const int id)
{
  if (containsObject(id))
  {
    m_sortedIDs.removeOne(id);
    delete m_objects.take(id);
  }
}

void GenericDataCollection::removeRow(const int i)
{
  if (0 <= i && i < m_sortedIDs.size()) {
    int id = m_sortedIDs.at(i);
    m_sortedIDs.removeAt(i);
    delete m_objects.take(id);
  }
}

void GenericDataCollection::insertRow(const int i, GenericDataObject *obj)
{
  if (obj != nullptr)
  {
    int id = obj->getValue("id").toInt();
    if (i < 0) {
      delete obj;
    }
    else if (i >= m_sortedIDs.size())
    {
      appendObject(id, obj);
    }
    else
    {
      m_sortedIDs.insert(i, id);
      m_objects.insert(id, obj);
    }
  }
}

QMetaType::Type GenericDataCollection::getPropertyTypeMeta(const QString& name) const
{
  QString lowerCaseName = name.toLower();
  Q_ASSERT_X(m_LowerCasePropertyNameMap.contains(lowerCaseName), "getPropertyTypeMeta", qPrintable(QString("Invalid field name %1").arg(name)));
  return m_LowerCasePropertyNameMap.contains(lowerCaseName) ? m_metaTypes.at(m_LowerCasePropertyNameMap.value(lowerCaseName)) : QMetaType::UnknownType;
}

QString GenericDataCollection::getPropertyName(const QString& name) const
{
  QString lowerCaseName = name.toLower();
  Q_ASSERT_X(m_LowerCasePropertyNameMap.contains(lowerCaseName), "getPropertyName", qPrintable(QString("Invalid field name %1").arg(name)));
  return m_LowerCasePropertyNameMap.contains(lowerCaseName) ? m_propertyNames.at(m_LowerCasePropertyNameMap.value(lowerCaseName)) : "";
}

int GenericDataCollection::getPropertyIndex(const QString& name) const
{
  return m_LowerCasePropertyNameMap.value(name.toLower(), -1);
}

bool GenericDataCollection::appendPropertyName(const QString& name, const QMetaType::Type pType)
{
    QString lowerCaseName = name.toLower();
    if (m_LowerCasePropertyNameMap.contains(lowerCaseName))
    {
      return false;
    }
    m_LowerCasePropertyNameMap.insert(lowerCaseName, m_propertyNames.size());
    m_propertyNames.append(name);
    m_metaTypes.append(pType);
    return true;
}


bool GenericDataCollection::exportToCSV(CSVWriter& writer) const
{
  writer.clearHeader();
  for (int i=0; i<getPropertNames().size(); ++i)
  {
    writer.addHeader(getPropertyName(i), getPropertyTypeMeta(i));
  }
  writer.writeHeader();

  // Export the data ordered by ID
  QList<int> objKeys = m_objects.keys();
  qSort(objKeys);

  for (int idx=0; idx < objKeys.size(); ++idx)
  {
    CSVLine newLine;
    GenericDataObject* obj = m_objects.value(objKeys[idx]);
    for (int i=0; i<getPropertNames().size(); ++i)
    {
      if (obj->containsValue(getPropertyName(i)))
      {
        QMetaType::Type columnType = getPropertyTypeMeta(i);
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
    for (int i=0; i<getPropertNames().size(); ++i)
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
    m_metaTypes.clear();
    m_LowerCasePropertyNameMap.clear();
}

const GenericDataCollection& GenericDataCollection::operator=(const GenericDataCollection& obj)
{
    if (this != &obj)
    {
        clear();
        m_propertyNames = obj.m_propertyNames;
        m_metaTypes = obj.m_metaTypes;
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


bool GenericDataCollection::addSortField(const QString& name, const Qt::SortOrder order, Qt::CaseSensitivity sensitive)
{
    if (!containsProperty(name))
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
    if (m_sortFields.size() == 0)
    {
        qSort(m_sortedIDs);
    }
    else
    {
        // TODO: Use the fields.
        // https://qt-project.org/forums/viewthread/4978
    }
}

GenericDataObject* GenericDataCollection::createEmptyObject() const
{
    GenericDataObject* data = new GenericDataObject();

    for (int i=0; i<m_propertyNames.size() && i < m_metaTypes.size(); ++i) {
        if (m_propertyNames.at(i).compare("id", Qt::CaseInsensitive) == 0) {
            data->setValue("id", getLargestId() + 1);
        } else {
            switch (m_metaTypes.at(i))
            {
            case QMetaType::Bool :
                data->setValue(m_propertyNames.at(i), false);
                break;
            case QMetaType::Int :
            case QMetaType::UInt :
            case QMetaType::LongLong :
            case QMetaType::ULongLong :
            case QMetaType::Long :
            case QMetaType::ULong :
            case QMetaType::Short :
            case QMetaType::UShort :
                data->setValue(m_propertyNames.at(i), 0);
                break;
            case QMetaType::Char :
            case QMetaType::QChar :
            case QMetaType::UChar :
            case QMetaType::SChar :
                data->setValue(m_propertyNames.at(i), 'X');
                break;

            case QMetaType::QString :
                data->setValue(m_propertyNames.at(i), "");
                break;

            case QMetaType::QUrl :
                data->setValue(m_propertyNames.at(i), QUrl());
                break;

            case QVariant::Uuid :
                data->setValue(m_propertyNames.at(i), QUuid());
                break;

            case QMetaType::Double :
            case QMetaType::Float :
                data->setValue(m_propertyNames.at(i), 0.0);
                break;

            case QMetaType::QDate :
                data->setValue(m_propertyNames.at(i), QDate::currentDate());
                break;

            case QMetaType::QDateTime :
                data->setValue(m_propertyNames.at(i), QDateTime::currentDateTime());
                break;

            case QMetaType::QTime :
                data->setValue(m_propertyNames.at(i), QTime::currentTime());
                break;

            case QMetaType::QStringList :
                data->setValue(m_propertyNames.at(i), QStringList());
                break;

            default:
              qDebug(qPrintable(QString("Type %1 not supported").arg(QMetaType::typeName(m_metaTypes.at(i)))));
            }
        }
    }
    return data;
}
