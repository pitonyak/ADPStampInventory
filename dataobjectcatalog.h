#ifndef DATAOBJECTCATALOG_H
#define DATAOBJECTCATALOG_H

#include "dataobjectbase.h"

#include <QDate>
#include <QDateTime>

class DataObjectCatalog : public DataObjectBase
{
  Q_PROPERTY(int TypeID READ getTypeID WRITE setTypeID)
  Q_PROPERTY(int CountryID READ getCountryID WRITE setCountryID)
  Q_PROPERTY(double FaceValue READ getFaceValue WRITE setFaceValue)
  Q_PROPERTY(QDate ReleaseDate READ getReleaseDate WRITE setReleaseDate)
  Q_PROPERTY(QDateTime Updated READ getUpdated WRITE setUpdated)
  Q_PROPERTY(QString Description READ getDescription WRITE setDescription)
  Q_PROPERTY(QString Scott READ getScott WRITE setScott)

public:
  explicit DataObjectCatalog(QObject *parent = 0, const int id=-1);
  DataObjectCatalog(const DataObjectCatalog& obj);

  inline QDateTime getUpdated() const { return m_Updated; }
  inline void setUpdated(const QDateTime updated) { m_Updated = updated; }

  inline const QString& getDescription() const {return m_Description; }
  inline void setDescription(const QString& Description) { m_Description = Description;}

  inline const QString& getScott() const {return m_Scott; }
  inline void setScott(const QString& Scott) { m_Scott = Scott;}

  inline int getTypeID() const {return m_TypeID; }
  inline void setTypeID(const int TypeID) { m_TypeID = TypeID;}

  inline const QDate& getReleaseDate() const {return m_ReleaseDate; }
  inline void setReleaseDate(const QDate& ReleaseDate) { m_ReleaseDate = ReleaseDate;}

  inline double getFaceValue() const {return m_FaceValue; }
  inline void setFaceValue(const double FaceValue) { m_FaceValue = FaceValue;}

  inline int getCountryID() const {return m_CountryID; }
  inline void setCountryID(const int CountryID) { m_CountryID = CountryID;}

  const DataObjectCatalog& operator=(const DataObjectCatalog& obj);

  bool operator==(const DataObjectCatalog& obj) const;

  virtual DataObjectBase* newInstance(QObject *parent = 0, const int id=-1);
  virtual DataObjectBase* clone();

private:
  int m_TypeID;
  int m_CountryID;
  double m_FaceValue;
  QDateTime m_Updated;
  QDate m_ReleaseDate;
  QString m_Scott;
  QString m_Description;
};

inline DataObjectBase* DataObjectCatalog::newInstance(QObject *parent, const int id)
{
  return new DataObjectCatalog(parent, id);
}

inline DataObjectBase* DataObjectCatalog::clone()
{
  return new DataObjectCatalog(this);
}

#endif // DATAOBJECTCATALOG_H
