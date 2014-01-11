#ifndef DATAOBJECTCOUNTRY_H
#define DATAOBJECTCOUNTRY_H

#include "dataobjectbase.h"

class DataObjectCountry : public DataObjectBase
{
    Q_PROPERTY(QString Name READ getName WRITE setName)
    Q_PROPERTY(QString A3 READ getA3 WRITE setA3)

public:
    explicit DataObjectCountry(QObject *parent = 0, const int id=-1);
    DataObjectCountry(const DataObjectCountry& obj);

    inline QString getName() const { return m_Name; }
    inline void setName(const QString& name) { m_Name = name; }

    inline QString getA3() const { return m_A3; }
    inline void setA3(const QString& A3) { m_A3 = A3; }

    const DataObjectCountry& operator=(const DataObjectCountry& obj);

    bool operator==(const DataObjectCountry& obj) const;

    virtual DataObjectBase* newInstance(QObject *parent = 0, const int id=-1);
    virtual DataObjectBase* clone();

private:
    QString m_Name;
    QString m_A3;
};

inline DataObjectBase* DataObjectCountry::newInstance(QObject *parent, const int id)
{
  return new DataObjectCountry(parent, id);
}

inline DataObjectBase* DataObjectCountry::clone()
{
  return new DataObjectCountry(this);
}

#endif // DATAOBJECTCOUNTRY_H
