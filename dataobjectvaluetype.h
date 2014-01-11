#ifndef DATAOBJECTVALUETYPE_H
#define DATAOBJECTVALUETYPE_H

#include "dataobjectbase.h"

class DataObjectValueType : public DataObjectBase
{
    Q_PROPERTY(QString Description READ getDescription WRITE setDescription)

public:
    explicit DataObjectValueType(QObject *parent = 0, const int id=-1);
    DataObjectValueType(const DataObjectValueType& obj);

    inline QString getDescription() const { return m_Description; }
    inline void setDescription(const QString& Description) { m_Description = Description; }

    const DataObjectValueType& operator=(const DataObjectValueType& obj);

    bool operator==(const DataObjectValueType& obj) const;

    virtual DataObjectBase* newInstance(QObject *parent = 0, const int id=-1);
    virtual DataObjectBase* clone();

private:
    QString m_Description;
};

inline DataObjectBase* DataObjectValueType::newInstance(QObject *parent, const int id)
{
  return new DataObjectValueType(parent, id);
}

inline DataObjectBase* DataObjectValueType::clone()
{
  return new DataObjectValueType(this);
}

#endif // DATAOBJECTVALUETYPE_H
