#ifndef CATALOGENTRY_H
#define CATALOGENTRY_H

#include <QObject>
#include <QDateTime>


// select catalog.id, catalog.scott, country.a3, catalog.releasedate, catalog.facevalue, catalog.description, catalog.updated from catalog, country where catalog.countryid= country.id AND country.a3='USA' order by catalog.scott

class CatalogEntry : public QObject
{
    Q_OBJECT
public:
    /*! \brief Constructor. */
    explicit CatalogEntry(QObject *parent = nullptr);
    explicit CatalogEntry(const long dbId, const float faceValue, const QString& catalogId, const QString& countryA3, const QString& description, const QDateTime releaseDate, const QDateTime updatedDate, QObject *parent = nullptr);

    CatalogEntry* clone(QObject *parent = nullptr) const;
    void setValuesFrom(const CatalogEntry& entry);

    /*! \brief Database ID in the Catalog table. */
    long getDbId() const {return m_DbId;}

    /*! \brief Face Value of the stamp. For a sheet with multiple stamps this includes all stamps included. */
    float getFaceValue() const {return m_FaceValue;}

    /*! \brief Catalog ID (Scott Number). */
    QString getCatalogId() const {return m_CatalogId;}

    /*! \brief Three letter country abreviation. */
    QString getCountryA3() const {return m_CountryA3;}

    /*! \brief Text description of the stamp. */
    QString getDescription() const {return m_Description;}

    /*! \brief Approximate release date, I have not been careful with these values. */
    QDateTime getReleaseDate() const {return m_ReleaseDate;}

    /*! \brief Last time this record was updated in the DB. */
    QDateTime getUpdatedDate() const {return m_UpdatedDate;}

public slots:
    /*! \brief Database ID in the Catalog table. */
    void setDbId(const long dbId);

    /*! \brief Face Value of the stamp. For a sheet with multiple stamps this includes all stamps included. */
    void setFaceValue(const float faceValue);

    /*! \brief Catalog ID (Scott Number). */
    void setCatalogId(const QString& catalogId);

    /*! \brief Three letter country abreviation. */
    void setCountryA3(const QString& countryA3);

    /*! \brief Text description of the stamp. */
    void setDescription(const QString& description);

    /*! \brief Approximate release date, I have not been careful with these values. */
    void setReleaseDate(const QDateTime releaseDate);

    /*! \brief Last time this record was updated in the DB. */
    void setUpdatedDate(const QDateTime updatedDate);

    QString getIdCategory() const {return m_IdCategory;}
    long getIdNum() const {return m_IdNum;}
    QString getIdTrailer() const {return m_IdTrailer;}

    void setIdCategory(const QString& idCategory) {m_IdCategory = idCategory;}
    void setIdNum(const long idNum) {m_IdNum = idNum;}
    void setIdTrailer(const QString& idTrailer) {m_IdTrailer = idTrailer;}

signals:
    void DbIdChanged(long dbId);
    void FaceValueChanged(float faceValue);
    void CatalogIdChanged(QString catalogId);
    void CountryA3Changed(QString countryA3);
    void DescriptionChanged(QString description);
    void ReleaseDateChanged(QDateTime releaseDate);
    void UpdatedDateChanged(QDateTime updatedDate);

private:
    /*! \brief Database ID in the Catalog table. */
    long m_DbId;

    /*! \brief Face Value of the stamp. For a sheet with multiple stamps this includes all stamps included. */
    float m_FaceValue;

    /*! \brief Catalog ID (Scott Number). */
    QString m_CatalogId;

    /*! \brief Split the Catalog ID and this is the leading category identifier. Needed because of things such as 1CVP7 */
    QString m_IdCategory;
    /*! \brief Split the Catalog ID and this is the numeric part */
    long m_IdNum;
    /*! \brief Split the Catalog ID and this is anything after the numeric part. */
    QString m_IdTrailer;

    /*! \brief Three letter country abreviation. */
    QString m_CountryA3;

    /*! \brief Text description of the stamp. */
    QString m_Description;

    /*! \brief Approximate release date, I have not been careful with these values. */
    QDateTime m_ReleaseDate;

    /*! \brief Last time this record was updated in the DB. */
    QDateTime m_UpdatedDate;
};

#endif // CATALOGENTRY_H
