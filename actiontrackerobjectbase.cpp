#include "actiontrackerobjectbase.h"

ActionTrackerObjectBase::ActionTrackerObjectBase() : m_action(UNKNOWN)
{
}

ActionTrackerObjectBase::~ActionTrackerObjectBase()
{
}

ActionTrackerObjectBase::ActionTrackerObjectBase(const TrackerAction action)
{
    m_action = action;
}

ActionTrackerObjectBase::ActionTrackerObjectBase(const ActionTrackerObjectBase& obj)
{
  setAction(obj.getAction());
}

const ActionTrackerObjectBase& ActionTrackerObjectBase::operator=(const ActionTrackerObjectBase& obj)
{
  if (this != &obj)
  {
    setAction(obj.getAction());
  }
  return *this;
}
