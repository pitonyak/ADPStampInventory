#ifndef DATAOBJECTBASE_H
#define DATAOBJECTBASE_H

#include <QObject>

class DataObjectBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int ID READ getId WRITE setId)

public:
    explicit DataObjectBase(QObject *parent = 0, const int id=-1);
    DataObjectBase(const DataObjectBase& obj);

    int getId() const;

    void setId(const int id=-1);

    const DataObjectBase& operator=(const DataObjectBase& obj);

    int compare(const DataObjectBase& obj) const;

    bool operator==(const DataObjectBase& obj) const;

signals:
    
public slots:

private:
    int m_Id;
    
};

inline int DataObjectBase::getId() const { return m_Id; }

inline void DataObjectBase::setId(const int id) { m_Id = id; }

#endif // DATAOBJECTBASE_H
