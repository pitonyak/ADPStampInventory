#include "dataobjectstamplocation.h"

DataObjectStampLocation::DataObjectStampLocation(QObject *parent, const int id) :
    DataObjectBase(parent, id)
{
}

DataObjectStampLocation::DataObjectStampLocation(const DataObjectStampLocation& obj) :
    DataObjectBase(obj), m_Name(obj.getName()), m_Description(obj.getDescription())
{
}

const DataObjectStampLocation& DataObjectStampLocation::operator=(const DataObjectStampLocation& obj)
{
    if (this != &obj)
    {
        DataObjectBase::operator=(obj);
        setName(obj.getName());
        setDescription(obj.getDescription());
    }
    return *this;
}

bool DataObjectStampLocation::operator==(const DataObjectStampLocation& obj) const
{
    return DataObjectBase::operator==(obj)
            && (getName().compare(obj.getName(), Qt::CaseSensitive) == 0)
            && (getDescription().compare(obj.getDescription(), Qt::CaseSensitive) == 0);
}
