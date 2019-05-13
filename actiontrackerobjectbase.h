#ifndef ACTIONTRACKEROBJECTBASE_H
#define ACTIONTRACKEROBJECTBASE_H

//**************************************************************************
//! Base class to track "actions".
/*!
 * This represents a single action. This object tracks only the action type.
 * See the derived class ActionTrackerObject, which is a template class that contains
 * a reference to the tracked object.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2019
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
