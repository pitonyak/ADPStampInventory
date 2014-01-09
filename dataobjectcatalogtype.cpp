#include "dataobjectcatalogtype.h"

DataObjectCatalogType::DataObjectCatalogType(QObject *parent, const int id) :
    DataObjectBase(parent, id)
{
}

DataObjectCatalogType::DataObjectCatalogType(const DataObjectCatalogType& obj) :
    DataObjectBase(obj), m_Name(obj.getName()), m_Description(obj.getDescription())
{
}

const DataObjectCatalogType& DataObjectCatalogType::operator=(const DataObjectCatalogType& obj)
{
    if (this != &obj)
    {
        DataObjectBase::operator=(obj);
        setName(obj.getName());
        setDescription(obj.getDescription());
    }
    return *this;
}

bool DataObjectCatalogType::operator==(const DataObjectCatalogType& obj) const
{
    return DataObjectBase::operator==(obj)
            && (getName().compare(obj.getName(), Qt::CaseSensitive) == 0)
            && (getDescription().compare(obj.getDescription(), Qt::CaseSensitive) == 0);
}
