#ifndef DATAOBJECTBASE_H
#define DATAOBJECTBASE_H

#include <QObject>

//**************************************************************************
/*! \class DataObjectBase
 * \brief Originally intended to have a specific object for each table.
 *
 * TODO: Can I remove this?
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2012-2014
 **************************************************************************/
class DataObjectBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int ID READ getId WRITE setId)

public:
    explicit DataObjectBase(QObject *parent = 0, const int id=-1);
    DataObjectBase(const DataObjectBase& obj);
    virtual ~DataObjectBase() {}

    int getId() const;

    void setId(const int id=-1);

    const DataObjectBase& operator=(const DataObjectBase& obj);

    int compare(const DataObjectBase& obj) const;

    bool operator==(const DataObjectBase& obj) const;

    virtual DataObjectBase* newInstance(QObject *parent = 0, const int id=-1);
    virtual DataObjectBase* clone();

signals:
    
public slots:

private:
    int m_Id;
    
};

inline int DataObjectBase::getId() const { return m_Id; }

inline void DataObjectBase::setId(const int id) { m_Id = id; }

inline DataObjectBase* DataObjectBase::newInstance(QObject *parent, const int id)
{
  return new DataObjectBase(parent, id);
}

inline DataObjectBase* DataObjectBase::clone()
{
  return new DataObjectBase(this);
}

#endif // DATAOBJECTBASE_H
