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

  QStack<ChangedObject<T>*>* value(const int i) const { return m_list.value(i); }
  QStack<ChangedObject<T>*>* top() const { return m_list.top(); }
  QStack<ChangedObject<T>*>* takeAt(const int i) { return m_list.takeAt(i); }
  QStack<ChangedObject<T>*>* pop() { return m_list.isEmpty() ? nullptr : m_list.pop(); }

  // Make versions that return a changed object.
  ChangedObject<T>* valueObject(const int i) const;
  ChangedObject<T>* topObject() const;
  ChangedObject<T>* takeObjectAt(const int i);
  ChangedObject<T>* popObject();

  void push(QStack<ChangedObject<T>*>* stack) { if (stack != nullptr) m_list.push(stack); }
  void push(const int row, const int col, const QString& changeInfo, ChangedObjectBase::ChangeType changeType=ChangedObjectBase::Unknown, T* newData=nullptr, T* oldData=nullptr);

  int size() const { return m_list.size(); }
  bool isEmpty() const { return m_list.isEmpty(); }
  void clear();

  protected:
  QStack< QStack<ChangedObject<T>*>* > m_list;

};

template <class T>
inline void ChangeTracker<T>::push(const int row, const int col, const QString& changeInfo, ChangedObjectBase::ChangeType changeType, T* newData, T* oldData)
{
  QStack<ChangedObject<T>*>* stack = new QStack<ChangedObject<T>*>();
  stack->push(new ChangedObject<T>(row, col, changeInfo, changeType, newData, oldData));
  push (stack);
}

template <class T>
inline void ChangeTracker<T>::clear()
{
  for (int i=0; i<m_list.size(); ++i) {
    qDeleteAll(*m_list.value(i));
  }
  qDeleteAll(m_list);
  m_list.clear();
}


template <class T>
inline ChangedObject<T>* ChangeTracker<T>::valueObject(const int i) const
{
  const QStack<ChangedObject<T>*>* stack = value(i);
  return (stack != nullptr && !stack->isEmpty()) ? stack->top() : nullptr;
}

template <class T>
inline ChangedObject<T>* ChangeTracker<T>::topObject() const
{
  const QStack<ChangedObject<T>*>* stack = top();
  return (stack != nullptr && !stack->isEmpty()) ? stack->top() : nullptr;
}

template <class T>
inline ChangedObject<T>* ChangeTracker<T>::takeObjectAt(const int i)
{
  QStack<ChangedObject<T>*>* stack = takeAt(i);
  if (stack != nullptr)
  {
    ChangedObject<T>* obj = stack->isEmpty() ? nullptr : stack->pop();
    qDeleteAll(*stack);
    return obj;
  }
  return nullptr;
}


template <class T>
inline ChangedObject<T>* ChangeTracker<T>::popObject()
{
  QStack<ChangedObject<T>*>* stack = pop();
  if (stack != nullptr)
  {
    ChangedObject<T>* obj = stack->isEmpty() ? nullptr : stack->pop();
    qDeleteAll(*stack);
    return obj;
  }
  return nullptr;
}



#endif // CHANGETRACKER_H
