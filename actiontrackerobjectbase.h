#ifndef ACTIONTRACKEROBJECTBASE_H
#define ACTIONTRACKEROBJECTBASE_H

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
