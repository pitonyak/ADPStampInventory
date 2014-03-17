#ifndef CHANGEDOBJECT_H
#define CHANGEDOBJECT_H

#include "changedobjectbase.h"

template <class T>
class ChangedObject : public ChangedObjectBase
{
public:
  explicit ChangedObject(ChangeType changeType=ChangedObjectBase::Unknown, T* data = nullptr, QObject *parent = nullptr) : ChangedObjectBase(changeType, parent), m_data(data) {}
  explicit ChangedObject(const QString& changeInfo, ChangeType changeType=ChangedObjectBase::Unknown, T* data = nullptr, QObject *parent = nullptr) : ChangedObjectBase(changeInfo, changeType, parent), m_data(data) {}

  ChangedObject(const ChangedObject& obj, QObject *parent = nullptr) : ChangedObjectBase(obj, parent), m_data(obj.m_data) {}

  virtual ~ChangedObject();

  virtual ChangedObjectBase* clone() const { return new ChangedObject<T>(*this); }

private:
  T* m_data;
};

template <class T> inline ChangedObject<T>::~ChangedObject()
{
  if (m_data != nullptr)
  {
    delete m_data;
  }
}


#endif // CHANGEDOBJECT_H
