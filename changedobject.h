#ifndef CHANGEDOBJECT_H
#define CHANGEDOBJECT_H

#include "changedobjectbase.h"

template <class T>
class ChangedObject : public ChangedObjectBase
{
public:
  explicit ChangedObject(ChangeType changeType=ChangedObjectBase::Unknown, T* newData = nullptr, T* oldData = nullptr, QObject *parent = nullptr) : ChangedObjectBase(changeType, parent), m_newData(newData), m_oldData(oldData) {}
  explicit ChangedObject(const QString& changeInfo, ChangeType changeType=ChangedObjectBase::Unknown, T* newData = nullptr, T* oldData = nullptr, QObject *parent = nullptr) : ChangedObjectBase(changeInfo, changeType, parent), m_newData(newData), m_oldData(oldData) {}

  ChangedObject(const ChangedObject& obj, QObject *parent = nullptr) : ChangedObjectBase(obj, parent), m_newData(obj.m_newData) {}

  virtual ~ChangedObject();

  virtual ChangedObjectBase* clone() const { return new ChangedObject<T>(*this); }

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
}


#endif // CHANGEDOBJECT_H
