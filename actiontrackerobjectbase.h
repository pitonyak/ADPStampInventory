#ifndef ACTIONTRACKEROBJECTBASE_H
#define ACTIONTRACKEROBJECTBASE_H

//**************************************************************************
//! Base class to track "actions".
/*!
 * This represents a single action. This object tracks only the action type.
 * This is part of a "database" type application, so, every tracked object is an Add, Modify, or Delete.
 *
 * The derived class ActionTrackerObject is a template class that contains
 * a reference to the tracked object.
 *
 * There is no data to clean-up so the destructor is empty,
 * but it is virtual in case it needs to be in derived classes.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2019-2021
 ***************************************************************************/
class ActionTrackerObjectBase
{
public:
  enum TrackerAction {
    UNKNOWN,
    DELETE,
    ADD,
    MODIFY
  };

  ActionTrackerObjectBase();
  ActionTrackerObjectBase(const TrackerAction action);
  ActionTrackerObjectBase(const ActionTrackerObjectBase& obj);
  virtual ~ActionTrackerObjectBase();

  const ActionTrackerObjectBase& operator=(const ActionTrackerObjectBase& obj);

  TrackerAction getAction() const;
  void setAction(const TrackerAction action);

private:
  ActionTrackerObjectBase::TrackerAction m_action;
};

inline ActionTrackerObjectBase::TrackerAction ActionTrackerObjectBase::getAction() const
{
  return m_action;
}

inline void ActionTrackerObjectBase::setAction(const TrackerAction action)
{
  m_action = action;
}

#endif // ACTIONTRACKEROBJECTBASE_H
