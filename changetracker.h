#ifndef CHANGETRACKER_H
#define CHANGETRACKER_H

#include "changetrackerbase.h"
#include "changedobject.h"

template <class T> class ChangeTracker : public ChangeTrackerBase
{
public:
  explicit ChangeTracker(QObject *parent = nullptr) : ChangeTrackerBase(parent) {}

  ChangedObject<T>* value(const int i) const { return dynamic_cast< ChangedObject<T> >(valueBase(i)); }
  ChangedObject<T>* top() const { return dynamic_cast< ChangedObject<T> >(topBase()); }
  ChangedObject<T>* takeAt(const int i) { return dynamic_cast< ChangedObject<T> >(takeAtBase(i)); }
  ChangedObject<T>* pop() { return dynamic_cast< ChangedObject<T> >(popBase()); }
  void push(ChangedObject<T>* obj) { pushBase(obj); }
  void push(ChangedObjectBase::ChangeType changeType, T* data);
  void push(ChangedObjectBase::ChangeType changeType, const QString& changeInfo, T* data);

};

template <class T>
inline void ChangeTracker<T>::push(ChangedObjectBase::ChangeType changeType, T* data)
{
  push (new ChangedObject<T>(changeType, data));
}


template <class T>
inline void ChangeTracker<T>::push(ChangedObjectBase::ChangeType changeType, const QString& changeInfo, T* data)
{
  push (new ChangedObject<T>(changeInfo, changeType, data));
}


#endif // CHANGETRACKER_H
