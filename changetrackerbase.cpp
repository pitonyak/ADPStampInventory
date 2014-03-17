#include "changetrackerbase.h"

ChangeTrackerBase::ChangeTrackerBase(QObject *parent) :
  QObject(parent)
{
}


ChangeTrackerBase::~ChangeTrackerBase()
{
  clear();
}

void ChangeTrackerBase::clear()
{
  for (int i=0; i<m_list.size(); ++i) {
    delete m_list.value(i);
  }
  m_list.clear();
}
