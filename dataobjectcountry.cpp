#include "dataobjectcountry.h"

DataObjectCountry::DataObjectCountry(QObject *parent, const int id) :
    DataObjectBase(parent, id)
{
}


DataObjectCountry::DataObjectCountry(const DataObjectCountry& obj) :
    DataObjectBase(obj), m_Name(obj.getName()), m_A3(obj.getA3())
{
}

const DataObjectCountry& DataObjectCountry::operator=(const DataObjectCountry& obj)
{
    if (this != &obj)
    {
        DataObjectBase::operator=(obj);
        setName(obj.getName());
        setA3(obj.getA3());
    }
    return *this;
}

bool DataObjectCountry::operator==(const DataObjectCountry& obj) const
{
    return DataObjectBase::operator==(obj)
            && (getName().compare(obj.getName(), Qt::CaseSensitive) == 0)
            && (getA3().compare(obj.getA3(), Qt::CaseSensitive) == 0);
}
