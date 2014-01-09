#ifndef DATAOBJECTVALUETYPE_H
#define DATAOBJECTVALUETYPE_H

#include "dataobjectbase.h"

class DataObjectValueType : public DataObjectBase
{
    Q_PROPERTY(QString Description READ getDescription WRITE setDescription)

public:
    explicit DataObjectValueType(QObject *parent = 0, const int id=-1);
    DataObjectValueType(const DataObjectValueType& obj);

    inline QString getDescription() const { return m_Description; }
    inline void setDescription(const QString& Description) { m_Description = Description; }

    const DataObjectValueType& operator=(const DataObjectValueType& obj);

    bool operator==(const DataObjectValueType& obj) const;

private:
    QString m_Description;
};

#endif // DATAOBJECTVALUETYPE_H
