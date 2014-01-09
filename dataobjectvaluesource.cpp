#include "dataobjectvaluesource.h"

DataObjectValueSource::DataObjectValueSource(QObject *parent, const int id) :
    DataObjectBase(parent, id)
{
}

DataObjectValueSource::DataObjectValueSource(const DataObjectValueSource& obj) :
    DataObjectBase(obj), m_Year(obj.getYear()), m_Description(obj.getDescription())
{
}

const DataObjectValueSource& DataObjectValueSource::operator=(const DataObjectValueSource& obj)
{
    if (this != &obj)
    {
        DataObjectBase::operator=(obj);
        setYear(obj.getYear());
        setDescription(obj.getDescription());
    }
    return *this;
}

bool DataObjectValueSource::operator==(const DataObjectValueSource& obj) const
{
    return DataObjectBase::operator==(obj)
            && (getYear() == obj.getYear())
            && (getDescription().compare(obj.getDescription(), Qt::CaseSensitive) == 0);
}
