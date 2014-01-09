#include "dataobjectdealer.h"

DataObjectDealer::DataObjectDealer(QObject *parent, const int id) :
    DataObjectBase(parent, id)
{
}

DataObjectDealer::DataObjectDealer(const DataObjectDealer& obj) :
    DataObjectBase(obj)
{
    operator=(obj);
}

const DataObjectDealer& DataObjectDealer::operator=(const DataObjectDealer& obj)
{
    if (this != &obj)
    {
        DataObjectBase::operator=(obj);
        setName(obj.getName());
        setUpdated(obj.getUpdated());
        setContact(obj.getContact());
        setAddress1(obj.getAddress1());
        setAddress2(obj.getAddress2());
        setAddress3(obj.getAddress3());
        setAddress4(obj.getAddress4());
        setPhone(obj.getPhone());
        setFax(obj.getFax());
        setComment(obj.getComment());
        setEmail(obj.getEmail());
        setWebsite(obj.getWebsite());
    }
    return *this;
}

bool DataObjectDealer::operator==(const DataObjectDealer& obj) const
{
    // TODO: Do I want to ignore the last updated field?

    return DataObjectBase::operator==(obj)
        && (getName().compare(obj.getName(), Qt::CaseSensitive) == 0)
        //&& (getUpdated() == obj.getUpdated())
        && (getContact().compare(obj.getContact(), Qt::CaseSensitive) == 0)
        && (getAddress1().compare(obj.getAddress1(), Qt::CaseSensitive) == 0)
        && (getAddress2().compare(obj.getAddress2(), Qt::CaseSensitive) == 0)
        && (getAddress3().compare(obj.getAddress3(), Qt::CaseSensitive) == 0)
        && (getAddress4().compare(obj.getAddress4(), Qt::CaseSensitive) == 0)
        && (getPhone().compare(obj.getPhone(), Qt::CaseSensitive) == 0)
        && (getFax().compare(obj.getFax(), Qt::CaseSensitive) == 0)
        && (getComment().compare(obj.getComment(), Qt::CaseSensitive) == 0)
        && (getEmail().compare(obj.getEmail(), Qt::CaseSensitive) == 0)
        && (getWebsite().compare(obj.getWebsite(), Qt::CaseSensitive) == 0);
}

