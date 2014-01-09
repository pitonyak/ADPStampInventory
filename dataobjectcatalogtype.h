#ifndef DATAOBJECTCATALOGTYPE_H
#define DATAOBJECTCATALOGTYPE_H

#include "dataobjectbase.h"

class DataObjectCatalogType : public DataObjectBase
{
    Q_PROPERTY(QString Name READ getName WRITE setName)
    Q_PROPERTY(QString Description READ getDescription WRITE setDescription)

public:
    explicit DataObjectCatalogType(QObject *parent = 0, const int id=-1);
    DataObjectCatalogType(const DataObjectCatalogType& obj);

    inline QString getName() const { return m_Name; }
    inline void setName(const QString& name) { m_Name = name; }

    inline QString getDescription() const { return m_Description; }
    inline void setDescription(const QString& Description) { m_Description = Description; }

    const DataObjectCatalogType& operator=(const DataObjectCatalogType& obj);

    bool operator==(const DataObjectCatalogType& obj) const;

private:
    QString m_Name;
    QString m_Description;

};

#endif // DATAOBJECTCATALOGTYPE_H
