#ifndef DATAOBJECTSTAMPLOCATION_H
#define DATAOBJECTSTAMPLOCATION_H

#include "dataobjectbase.h"

class DataObjectStampLocation : public DataObjectBase
{
  Q_PROPERTY(QString Name READ getName WRITE setName)
  Q_PROPERTY(QString Description READ getDescription WRITE setDescription)

public:
  explicit DataObjectStampLocation(QObject *parent = 0, const int id=-1);
  DataObjectStampLocation(const DataObjectStampLocation& obj);

  inline QString getName() const { return m_Name; }
  inline void setName(const QString& name) { m_Name = name; }

  inline QString getDescription() const { return m_Description; }
  inline void setDescription(const QString& Description) { m_Description = Description; }

  const DataObjectStampLocation& operator=(const DataObjectStampLocation& obj);

  bool operator==(const DataObjectStampLocation& obj) const;

  virtual DataObjectBase* newInstance(QObject *parent = 0, const int id=-1);
  virtual DataObjectBase* clone();

private:
  QString m_Name;
  QString m_Description;
};

inline DataObjectBase* DataObjectStampLocation::newInstance(QObject *parent, const int id)
{
  return new DataObjectStampLocation(parent, id);
}

inline DataObjectBase* DataObjectStampLocation::clone()
{
  return new DataObjectStampLocation(this);
}

#endif // DATAOBJECTSTAMPLOCATION_H
