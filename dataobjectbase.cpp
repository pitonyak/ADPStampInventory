#include "dataobjectbase.h"

DataObjectBase::DataObjectBase(QObject *parent, const int id) :
    QObject(parent), m_Id(id)
{
}

DataObjectBase::DataObjectBase(const DataObjectBase& obj) :
    QObject(obj.parent()), m_Id(obj.getId())
{

}


const DataObjectBase& DataObjectBase::operator=(const DataObjectBase& obj)
{
    if (&obj != this)
    {
        setId(obj.getId());
    }
    return *this;
}

int DataObjectBase::compare(const DataObjectBase& obj) const
{
    return (getId() == obj.getId()) ? 0 : (getId() < obj.getId() ? -1 : 1);
}

bool DataObjectBase::operator==(const DataObjectBase& obj) const
{
    return getId() == obj.getId();
}

