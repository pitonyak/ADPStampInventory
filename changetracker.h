#ifndef CHANGETRACKER_H
#define CHANGETRACKER_H

#include "changetrackerbase.h"
#include "changedobject.h"

#include <QStack>

template <class T> class ChangeTracker : public ChangeTrackerBase
{
public:
  explicit ChangeTracker(QObject *parent = nullptr) : ChangeTrackerBase(parent) {}

  ~ChangeTracker() { clear(); }

  ChangedObject<T>* value(const int i) const { return m_list.value(i); }
  ChangedObject<T>* top() const { return m_list.top(); }
  ChangedObject<T>* takeAt(const int i) { return m_list.takeAt(i); }
  ChangedObject<T>* pop() { return m_list.isEmpty() ? nullptr : m_list.pop(); }

  void push(ChangedObject<T>* obj) { if (obj != nullptr) m_list.push(obj); }
  void push(ChangedObjectBase::ChangeType changeType, T* newData, T* oldData=nullptr);
  void push(ChangedObjectBase::ChangeType changeType, const QString& changeInfo, T* newData, T* oldData=nullptr);

  int size() const { return m_list.size(); }
  bool isEmpty() const { return m_list.isEmpty(); }
  void clear();

  protected:
  QStack<ChangedObject<T>*> m_list;

};

template <class T>
inline void ChangeTracker<T>::push(ChangedObjectBase::ChangeType changeType, T* newData, T *oldData)
{
  push (new ChangedObject<T>(changeType, newData, oldData));
}


template <class T>
inline void ChangeTracker<T>::push(ChangedObjectBase::ChangeType changeType, const QString& changeInfo, T* newData, T *oldData)
{
  push (new ChangedObject<T>(changeInfo, changeType, newData, oldData));
}

template <class T>
inline void ChangeTracker<T>::clear()
{
  for (int i=0; i<m_list.size(); ++i) {
    delete m_list.value(i);
  }
  m_list.clear();
}




#endif // CHANGETRACKER_H
