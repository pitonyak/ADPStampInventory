#ifndef ACTIONTRACKEROBJECT_H
#define ACTIONTRACKEROBJECT_H

#include "actiontrackerobjectbase.h"

template<class T>
class ActionTrackerObject : ActionTrackerObjectBase
{
public:
  ActionTrackerObject();
  explicit ActionTrackerObject(const ActionTrackerObjectBase::TrackerAction action, T* obj);
  ActionTrackerObject(const ActionTrackerObject<T>& obj);

  const ActionTrackerObject<T> operator=(const ActionTrackerObject<T>& obj);

  T* getObject() const;
  void setObject(T* obj);

private:
  T* m_object;
};


template <class T> inline T* ActionTrackerObject<T>::getObject() const
{
  return m_object;
}

template <class T> inline void ActionTrackerObject<T>::setObject(T* obj)
{
  m_object = obj;
}



#endif // ACTIONTRACKEROBJECT_H
