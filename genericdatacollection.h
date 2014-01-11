#ifndef GENERICDATACOLLECTION_H
#define GENERICDATACOLLECTION_H

#include "genericdataobject.h"

#include <QObject>
#include <QStringList>
#include <QHash>
#include <QVariant>

class CSVWriter;

class GenericDataCollection : public QObject
{
  Q_OBJECT
public:
  explicit GenericDataCollection(QObject *parent = 0);

  const QVariant& getValue(const QString& name) const;
  void setValue(const QString& name, const QVariant& value);

  inline const QString& getPropertyName(const int i) const { return m_propertyNames.at(i); }
  inline const QStringList& getPropertNames() const { return m_propertyNames; }
  inline bool hasProperty(const QString& name) const { return m_propertyNames.contains(name); }

  inline bool hasObject(const int id) const { return m_objects.contains(id); }

  inline void appendPropertyName(const QString& name) { m_propertyNames.append(name); }
  inline void appendPropertyName(const QStringList& name) { m_propertyNames.append(name); }

  inline QVariant::Type getPropertyType(const int i) const { return m_propertyTypes.at(i); }
  inline void appendPropertyType(const QVariant::Type pType) { m_propertyTypes.append(pType); }

  void appendObject(const int id, GenericDataObject* obj);
  void removeObject(const int);

  bool exportToCSV(CSVWriter& writer) const;

  inline int getObjectCount() const { return m_objects.count(); }

signals:

public slots:

private:
  QStringList m_propertyNames;
  QList<QVariant::Type> m_propertyTypes;
  QHash<int, GenericDataObject*> m_objects;
};

#endif // GENERICDATACOLLECTION_H
