#include "catalogentry.h"

CatalogEntry::CatalogEntry(QObject *parent)
    : QObject{parent}, m_DbId{-1}, m_FaceValue{0.0}, m_CatalogId{""},
      m_CountryA3{""}, m_Description{""},
      m_ReleaseDate(QDateTime::currentDateTime()),
      m_UpdatedDate(QDateTime::currentDateTime()) {}

CatalogEntry::CatalogEntry(const long dbId, const float faceValue,
                           const QString &catalogId, const QString &countryA3,
                           const QString &description,
                           const QDateTime releaseDate,
                           const QDateTime updatedDate, QObject *parent)
    : QObject{parent}, m_DbId(dbId), m_FaceValue(faceValue),
      m_CatalogId(catalogId), m_CountryA3(countryA3),
      m_Description(description), m_ReleaseDate(releaseDate),
      m_UpdatedDate(updatedDate) {}

CatalogEntry *CatalogEntry::clone(QObject *parent) const {
  return new CatalogEntry(getDbId(), getFaceValue(), getCatalogId(),
                          getCountryA3(), getDescription(), getReleaseDate(),
                          getUpdatedDate(), parent);
}

void CatalogEntry::setValuesFrom(const CatalogEntry &entry) {
  setDbId(entry.getDbId());
  setFaceValue(entry.getFaceValue());
  setCatalogId(entry.getCatalogId());
  setCountryA3(entry.getCountryA3());
  setDescription(entry.getDescription());
  setReleaseDate(entry.getReleaseDate());
  setUpdatedDate(entry.getUpdatedDate());
}

void CatalogEntry::setDbId(const long dbId) {
  if (m_DbId == dbId)
    return;

  m_DbId = dbId;
  emit DbIdChanged(dbId);
}

void CatalogEntry::setFaceValue(const float faceValue) {
  if (m_FaceValue == faceValue)
    return;

  m_FaceValue = faceValue;
  emit FaceValueChanged(faceValue);
}

void CatalogEntry::setCatalogId(const QString &catalogId) {
  if (m_CatalogId == catalogId)
    return;

  m_CatalogId = catalogId;
  emit CatalogIdChanged(catalogId);
}

void CatalogEntry::setCountryA3(const QString &countryA3) {
  if (m_CountryA3 == countryA3)
    return;

  m_CountryA3 = countryA3;
  emit CountryA3Changed(countryA3);
}

void CatalogEntry::setDescription(const QString &description) {
  if (m_Description == description)
    return;

  m_Description = description;
  emit DescriptionChanged(description);
}

void CatalogEntry::setReleaseDate(const QDateTime releaseDate) {
  if (m_ReleaseDate == releaseDate)
    return;

  m_ReleaseDate = releaseDate;
  emit ReleaseDateChanged(releaseDate);
}

void CatalogEntry::setUpdatedDate(const QDateTime updatedDate) {
  if (m_UpdatedDate == updatedDate)
    return;

  m_UpdatedDate = updatedDate;
  emit UpdatedDateChanged(updatedDate);
}
