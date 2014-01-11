#ifndef DATAOBJECTDEALER_H
#define DATAOBJECTDEALER_H

#include "dataobjectbase.h"

#include <QDateTime>


class DataObjectDealer : public DataObjectBase
{
    Q_PROPERTY(QDateTime Updated READ getUpdated WRITE setUpdated)
    Q_PROPERTY(QString Name READ getName WRITE setName)
    Q_PROPERTY(QString Contact READ getContact WRITE setContact)
    Q_PROPERTY(QString Address1 READ getAddress1 WRITE setAddress1)
    Q_PROPERTY(QString Address2 READ getAddress2 WRITE setAddress2)
    Q_PROPERTY(QString Address3 READ getAddress3 WRITE setAddress3)
    Q_PROPERTY(QString Address4 READ getAddress4 WRITE setAddress4)
    Q_PROPERTY(QString Phone READ getPhone WRITE setPhone)
    Q_PROPERTY(QString Fax READ getFax WRITE setFax)
    Q_PROPERTY(QString Comment READ getComment WRITE setComment)
    Q_PROPERTY(QString Email READ getEmail WRITE setEmail)
    Q_PROPERTY(QString Website READ getWebsite WRITE setWebsite)

public:
    explicit DataObjectDealer(QObject *parent = 0, const int id=-1);
    DataObjectDealer(const DataObjectDealer& obj);

    inline QDateTime getUpdated() const { return m_Updated; }
    inline void setUpdated(const QDateTime updated) { m_Updated = updated; }

    inline const QString& getName() const {return m_Name; }
    inline void setName(const QString& Name) { m_Name = Name;}

    inline const QString& getContact() const {return m_Contact; }
    inline void setContact(const QString& Contact) { m_Contact = Contact;}

    inline const QString& getAddress1() const {return m_Address1; }
    inline void setAddress1(const QString& Address) { m_Address1 = Address;}

    inline const QString& getAddress2() const {return m_Address2; }
    inline void setAddress2(const QString& Address) { m_Address2 = Address;}

    inline const QString& getAddress3() const {return m_Address3; }
    inline void setAddress3(const QString& Address) { m_Address3 = Address;}

    inline const QString& getAddress4() const {return m_Address4; }
    inline void setAddress4(const QString& Address) { m_Address4 = Address;}

    inline const QString& getPhone() const {return m_Phone; }
    inline void setPhone(const QString& Phone) { m_Phone = Phone;}

    inline const QString& getFax() const {return m_Fax; }
    inline void setFax(const QString& Fax) { m_Fax = Fax;}

    inline const QString& getComment() const {return m_Comment; }
    inline void setComment(const QString& Comment) { m_Comment = Comment;}

    inline const QString& getEmail() const {return m_Email; }
    inline void setEmail(const QString& email) { m_Email = email;}

    inline const QString& getWebsite() const {return m_Website; }
    inline void setWebsite(const QString& website) { m_Website = website;}

    const DataObjectDealer& operator=(const DataObjectDealer& obj);

    bool operator==(const DataObjectDealer& obj) const;

    virtual DataObjectBase* newInstance(QObject *parent = 0, const int id=-1);
    virtual DataObjectBase* clone();

private:
    QDateTime m_Updated;
    QString m_Name;
    QString m_Contact;
    QString m_Address1;
    QString m_Address2;
    QString m_Address3;
    QString m_Address4;
    QString m_Phone;
    QString m_Fax;
    QString m_Comment;
    QString m_Email;
    QString m_Website;
};

inline DataObjectBase* DataObjectDealer::newInstance(QObject *parent, const int id)
{
  return new DataObjectDealer(parent, id);
}

inline DataObjectBase* DataObjectDealer::clone()
{
  return new DataObjectDealer(this);
}

#endif // DATAOBJECTDEALER_H
