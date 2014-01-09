#ifndef DATAOBJECTVALUESOURCE_H
#define DATAOBJECTVALUESOURCE_H

#include "dataobjectbase.h"

#include <QDate>

class DataObjectValueSource : public DataObjectBase
{
    Q_PROPERTY(QString Year READ getYear WRITE setYear)
    Q_PROPERTY(QDate Description READ getDescription WRITE setDescription)

public:
    explicit DataObjectValueSource(QObject *parent = 0, const int id=-1);
    DataObjectValueSource(const DataObjectValueSource& obj);

    inline QDate getYear() const { return m_Year; }
    inline void setYear(const QDate& Year) { m_Year = Year; }

    inline QString getDescription() const { return m_Description; }
    inline void setDescription(const QString& Description) { m_Description = Description; }

    const DataObjectValueSource& operator=(const DataObjectValueSource& obj);

    bool operator==(const DataObjectValueSource& obj) const;

private:
    QDate m_Year;
    QString m_Description;
};

#endif // DATAOBJECTVALUESOURCE_H
