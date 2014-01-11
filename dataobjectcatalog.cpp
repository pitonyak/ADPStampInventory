#include "dataobjectcatalog.h"

DataObjectCatalog::DataObjectCatalog(QObject *parent, const int id) :
    DataObjectBase(parent, id)
{
}

DataObjectCatalog::DataObjectCatalog(const DataObjectCatalog& obj) :
    DataObjectBase(obj)
{
    operator=(obj);
}

const DataObjectCatalog& DataObjectCatalog::operator=(const DataObjectCatalog& obj)
{
    if (this != &obj)
    {
        DataObjectBase::operator=(obj);
        setDescription(obj.getDescription());
        setUpdated(obj.getUpdated());
        setScott(obj.getScott());
        setTypeID(obj.getTypeID());
        setReleaseDate(obj.getReleaseDate());
        setFaceValue(obj.getFaceValue());
        setCountryID(obj.getCountryID());
    }
    return *this;
}

bool DataObjectCatalog::operator==(const DataObjectCatalog& obj) const
{
    // TODO: Do I want to ignore the last updated field?

    return DataObjectBase::operator==(obj)
        && (getTypeID() == obj.getTypeID())
        && (getCountryID() == obj.getCountryID())
        && (getReleaseDate() == obj.getReleaseDate())
        && (getDescription().compare(obj.getDescription(), Qt::CaseSensitive) == 0)
        && (getFaceValue() == obj.getFaceValue())
        //&& (getUpdated() == obj.getUpdated())
        && (getScott().compare(obj.getScott(), Qt::CaseSensitive) == 0);
}
