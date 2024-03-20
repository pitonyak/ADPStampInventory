#include "changedobjectbase.h"

ChangedObjectBase::ChangedObjectBase(const int row, const int col, const QString& changeInfo, ChangeType changeType, QObject *parent) :
  QObject(parent), m_changeType(changeType), m_changeInfo(changeInfo), m_row(row), m_col(col)
{
}

ChangedObjectBase::ChangedObjectBase(const ChangedObjectBase& obj, QObject *parent) :
QObject(parent), m_changeType(obj.getChangeType()), m_changeInfo(obj.getChangeInfo()), m_row(obj.getRow()), m_col(obj.getCol())
{

}

const ChangedObjectBase& ChangedObjectBase::operator=(const ChangedObjectBase& obj)
{
  if (this != &obj)
  {
    setParent(obj.parent());
    setChangeType(obj.getChangeType());
    setChangeInfo(obj.getChangeInfo());
    setRow(obj.getRow());
    setCol(obj.getCol());
  }
  return *this;
}

ChangedObjectBase::~ChangedObjectBase()
{

}
