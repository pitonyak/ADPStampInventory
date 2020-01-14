#include "actiontrackerobject.h"

template <class T>
ActionTrackerObject<T>::ActionTrackerObject() :
  ActionTrackerObjectBase(), m_object(nullptr)
{
}

template <class T>
ActionTrackerObject<T>::ActionTrackerObject(const ActionTrackerObjectBase::TrackerAction action, T* obj) :
  ActionTrackerObjectBase(action), m_object(obj)
{
  if (obj != nullptr) {
    setObject(obj->getObject());
  }
}

template <class T>
ActionTrackerObject<T>::ActionTrackerObject(const ActionTrackerObject<T>& obj) :
  ActionTrackerObjectBase(obj), m_object(nullptr)
{
  setObject(obj.getObject());
}

template <class T>
const ActionTrackerObject<T> ActionTrackerObject<T>::operator=(const ActionTrackerObject<T>& obj)
{
  if (this != &obj)
  {
    ActionTrackerObjectBase::operator=(obj);
    setObject(obj.getObject());
  }
  return *this;
}
