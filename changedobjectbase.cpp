#include "changedobjectbase.h"

ChangedObjectBase::ChangedObjectBase(ChangeType changeType, QObject *parent) :
  QObject(parent), m_changeType(changeType)
{
}

ChangedObjectBase::ChangedObjectBase(const QString& changeInfo, ChangeType changeType, QObject *parent) :
  QObject(parent), m_changeType(changeType), m_changeInfo(changeInfo)
{
}

ChangedObjectBase::ChangedObjectBase(const ChangedObjectBase& obj, QObject *parent) :
QObject(parent), m_changeType(obj.getChangeType()), m_changeInfo(obj.getChangeInfo())
{

}

const ChangedObjectBase& ChangedObjectBase::operator=(const ChangedObjectBase& obj)
{
  if (this != &obj)
  {
    setParent(obj.parent());
    setChangeType(obj.getChangeType());
    setChangeInfo(obj.getChangeInfo());
  }
  return *this;
}

ChangedObjectBase::~ChangedObjectBase()
{

}
