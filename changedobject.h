#ifndef CHANGEDOBJECT_H
#define CHANGEDOBJECT_H

#include "changedobjectbase.h"

template <class T>
class ChangedObject : public ChangedObjectBase
{
public:
  explicit ChangedObject(const int row, const int col, const QString& changeInfo, ChangeType changeType=ChangedObjectBase::Unknown, T* newData = nullptr, T* oldData = nullptr, QObject *parent = nullptr) : ChangedObjectBase(row, col, changeInfo, changeType, parent), m_newData(newData), m_oldData(oldData) {}

  ChangedObject(const ChangedObject& obj, QObject *parent = nullptr) : ChangedObjectBase(obj, parent), m_newData(obj.m_newData) {}

  virtual ~ChangedObject();

  virtual ChangedObjectBase* clone() const { return new ChangedObject<T>(*this); }

  T* getNewData() const { return m_newData; }
  T* getOldData() const { return m_oldData; }

  T* takeNewData() { T* data = m_newData; m_newData = nullptr; return data; }
  T* takeOldData() { T* data = m_oldData; m_oldData = nullptr; return data; }


private:
  T* m_newData;
  T* m_oldData;
};

template <class T> inline ChangedObject<T>::~ChangedObject()
{
  if (m_newData != nullptr)
  {
    delete m_newData;
  }
  if (m_oldData != nullptr)
  {
    delete m_oldData;
  }
}


#endif // CHANGEDOBJECT_H
