#include "dataobjectvaluetype.h"

DataObjectValueType::DataObjectValueType(QObject *parent, const int id) :
    DataObjectBase(parent, id)
{
}

DataObjectValueType::DataObjectValueType(const DataObjectValueType& obj) :
    DataObjectBase(obj), m_Description(obj.getDescription())
{

}

const DataObjectValueType& DataObjectValueType::operator=(const DataObjectValueType& obj)
{
    if (this != &obj)
    {
        DataObjectBase::operator=(obj);
        setDescription(obj.getDescription());
    }
    return *this;
}

bool DataObjectValueType::operator==(const DataObjectValueType& obj) const
{
    return DataObjectBase::operator==(obj)
            && (getDescription().compare(obj.getDescription(), Qt::CaseSensitive) == 0);
}
