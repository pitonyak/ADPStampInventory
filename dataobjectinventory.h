#ifndef DATAOBJECTINVENTORY_H
#define DATAOBJECTINVENTORY_H

#include "dataobjectbase.h"

#include <QDate>
#include <QDateTime>

class DataObjectInventory : public DataObjectBase
{
  Q_PROPERTY(int CatalogID READ getCatalogID WRITE setCatalogID)
  Q_PROPERTY(int DealerID READ getDealerID WRITE setDealerID)
  Q_PROPERTY(int LocationID READ getLocationID WRITE setLocationID) //??
  Q_PROPERTY(int TypeID READ getTypeID WRITE setTypeID)             //??
  Q_PROPERTY(int Quantity READ getQuantity WRITE setQuantity)       //??
  Q_PROPERTY(bool Replace READ getReplace WRITE setReplace) //??
  Q_PROPERTY(double ValueMultiplier READ getValueMultiplier WRITE setValueMultiplier)
  Q_PROPERTY(double Paid READ getPaid WRITE setPaid)  //??
  Q_PROPERTY(QDate PurchaseDate READ getPurchaseDate WRITE setPurchaseDate)
  Q_PROPERTY(QDateTime Updated READ getUpdated WRITE setUpdated)
  Q_PROPERTY(QString Grade READ getGrade WRITE setGrade)
  Q_PROPERTY(QString Condition READ getCondition WRITE setCondition)

  Q_PROPERTY(QString Selvage READ getSelvage WRITE setSelvage) //??
  Q_PROPERTY(QString Centering READ getCentering WRITE setCentering) //??
  Q_PROPERTY(QString Back READ getBack WRITE setBack) //??
  Q_PROPERTY(QString Comment READ getComment WRITE setComment) //??

public:
  explicit DataObjectInventory(QObject *parent = 0, const int id=-1);
  DataObjectInventory(const DataObjectInventory& obj);

  inline QDateTime getUpdated() const { return m_Updated; }
  inline void setUpdated(const QDateTime updated) { m_Updated = updated; }

  inline const QString& getGrade() const {return m_Grade; }
  inline void setGrade(const QString& Grade) { m_Grade = Grade;}

  inline const QString& getCondition() const {return m_Condition; }
  inline void setCondition(const QString& Condition) { m_Condition = Condition;}

  inline int getCatalogID() const {return m_CatalogID; }
  inline void setCatalogID(const int CatalogID) { m_CatalogID = CatalogID;}

  inline const QDate& getPurchaseDate() const {return m_PurchaseDate; }
  inline void setPurchaseDate(const QDate& PurchaseDate) { m_PurchaseDate = PurchaseDate;}

  inline double getValueMultiplier() const {return m_ValueMultiplier; }
  inline void setValueMultiplier(const double ValueMultiplier) { m_ValueMultiplier = ValueMultiplier;}

  inline int getDealerID() const {return m_DealerID; }
  inline void setDealerID(const int DealerID) { m_DealerID = DealerID;}

  inline int getLocationID() const {return m_LocationID; }
  inline void setLocationID(const int LocationID) { m_LocationID = LocationID;}

  inline int getTypeID() const {return m_TypeID; }
  inline void setTypeID(const int TypeID) { m_TypeID = TypeID;}

  inline int getQuantity() const {return m_Quantity; }
  inline void setQuantity(const int Quantity) { m_Quantity = Quantity;}

  inline bool getReplace() const {return m_Replace; }
  inline void setReplace(const bool Replace) { m_Replace = Replace;}

  inline double getPaid() const {return m_Paid; }
  inline void setPaid(const double Paid) { m_Paid = Paid;}

  inline const QString& getSelvage() const {return m_Selvage; }
  inline void setSelvage(const QString& Selvage) { m_Selvage = Selvage;}

  inline const QString& getCentering() const {return m_Centering; }
  inline void setCentering(const QString& Centering) { m_Centering = Centering;}

  inline const QString& getBack() const {return m_Back; }
  inline void setBack(const QString& Back) { m_Back = Back;}

  inline const QString& getComment() const {return m_Comment; }
  inline void setComment(const QString& Comment) { m_Comment = Comment;}

  const DataObjectInventory& operator=(const DataObjectInventory& obj);

  bool operator==(const DataObjectInventory& obj) const;

  virtual DataObjectBase* newInstance(QObject *parent = 0, const int id=-1);
  virtual DataObjectBase* clone();

private:
  int m_CatalogID;
  int m_DealerID;
  int m_LocationID;
  int m_TypeID;
  int m_Quantity;
  bool m_Replace;
  double m_Paid;
  double m_ValueMultiplier;
  QDateTime m_Updated;
  QDate m_PurchaseDate;
  QString m_Condition;
  QString m_Grade;
  QString m_Selvage;
  QString m_Centering;
  QString m_Back;
  QString m_Comment;
};

inline DataObjectBase* DataObjectInventory::newInstance(QObject *parent, const int id)
{
  return new DataObjectInventory(parent, id);
}

inline DataObjectBase* DataObjectInventory::clone()
{
  return new DataObjectInventory(this);
}

#endif // DATAOBJECTINVENTORY_H
