#include "dataobjectinventory.h"

DataObjectInventory::DataObjectInventory(QObject *parent, const int id) :
    DataObjectBase(parent, id)
{
}

DataObjectInventory::DataObjectInventory(const DataObjectInventory& obj) :
  DataObjectBase(obj), m_CatalogID(0), m_DealerID(0), m_LocationID(0),
  m_TypeID(0), m_Quantity(1), m_Replace(false), m_Paid(0.0), m_ValueMultiplier(1.0),
  m_Updated(QDateTime::currentDateTime()), m_PurchaseDate(QDate::currentDate())
{
    operator=(obj);
}

const DataObjectInventory& DataObjectInventory::operator=(const DataObjectInventory& obj)
{
    if (this != &obj)
    {
        DataObjectBase::operator=(obj);
        setCatalogID(obj.getCatalogID());
        setDealerID(obj.getDealerID());
        setLocationID(obj.getLocationID());
        setTypeID(obj.getTypeID());
        setQuantity(obj.getQuantity());
        setReplace(obj.getReplace());
        setPaid(obj.getPaid());
        setValueMultiplier(obj.getValueMultiplier());
        setUpdated(obj.getUpdated());
        setPurchaseDate(obj.getPurchaseDate());
        setCondition(obj.getCondition());
        setGrade(obj.getGrade());
        setSelvage(obj.getSelvage());
        setCentering(obj.getCentering());
        setBack(obj.getBack());
        setComment(obj.getComment());
    }
    return *this;
}

bool DataObjectInventory::operator==(const DataObjectInventory& obj) const
{
    // TODO: Do I want to ignore the last updated field?

    return DataObjectBase::operator==(obj)
        && (getCatalogID() == obj.getCatalogID())
        && (getDealerID() == obj.getDealerID())
        && (getLocationID() == obj.getLocationID())
        && (getTypeID() == obj.getTypeID())
        && (getQuantity() == obj.getQuantity())
        && (getReplace() == obj.getReplace())
        && (getPaid() == obj.getPaid())
        && (getValueMultiplier() == obj.getValueMultiplier())
        && (getPurchaseDate() == obj.getPurchaseDate())
        //&& (getUpdated() == obj.getUpdated())
        && (getCondition().compare(obj.getCondition(), Qt::CaseSensitive) == 0)
        && (getGrade().compare(obj.getGrade(), Qt::CaseSensitive) == 0)
        && (getSelvage().compare(obj.getSelvage(), Qt::CaseSensitive) == 0)
        && (getCentering().compare(obj.getCentering(), Qt::CaseSensitive) == 0)
        && (getBack().compare(obj.getBack(), Qt::CaseSensitive) == 0)
        && (getComment().compare(obj.getComment(), Qt::CaseSensitive) == 0);
}
