#ifndef ACTIONTRACKEROBJECT_H
#define ACTIONTRACKEROBJECT_H

#include "actiontrackerobjectbase.h"

//**************************************************************************
//! Tracks actions that can be undone. The tracked object is the template class.
/*!
 * This represents a single action and it includes a single object on which the action was performed.
 *
 * Who owns the tracked object?
 * A pointer is passed in and nothing here deletes the object.
 * Something else had better delete this thing if it needs to be deleted.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2019-2021
 ***************************************************************************/

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
