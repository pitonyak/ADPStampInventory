#ifndef CHANGETRACKER_H
#define CHANGETRACKER_H

#include "changetrackerbase.h"
#include "changedobject.h"

#include <QStack>

//**************************************************************************
/*! \class ChangeTracker
 * \brief Class to track changes.
 *
 * This is used in the table models to track changes as they are made to the model.
 * These use a ChangeTracker<GenericDataObject>.
 *
 * So, how is this used?
 *
 * Every change is really a stack (QStack<ChangedObject<T>>) of change objects.
 * A stack is used because a single action by the user may really be multiple changes internally.
 *
 * If I make a simple single change, the code is similar to:
 *
 * m_changeTracker.push(row, column, fieldName, ChangedObjectBase::Edit, newObject->clone(), clonedOriginalObject);
 *
 * If I make more than one change, create a stack to hold the changes:
 *
 * QStack<ChangedObject<GenericDataObject>*> * lastChanges = new QStack<ChangedObject<GenericDataObject>*>();
 *
 * Push each change onto the new lastChanges stack, then push the changes onto the tracker
 *
 * m_changeTracker.push(lastChanges);
 *
 * To save changes, start at the bottom and work to the top.
 *
 * QStack<ChangedObject<GenericDataObject>*> * firstChanges = m_changeTracker.takeAt(0);
 *
 * Look at examles in saveTrackedChanges, which then walk the changes and save them to the DB.
 *
 * For undo, start at the top and work to the bottom.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2012-2021
 **************************************************************************/
template <class T> class ChangeTracker : public ChangeTrackerBase
{
public:
  /*! \brief Constructor
   *
   *  \param [in] parent The parent of an object may be viewed as the object's owner. The destructor of a parent object destroys all child objects.
   */
  explicit ChangeTracker(QObject *parent = nullptr) : ChangeTrackerBase(parent) {}

  virtual ~ChangeTracker() { clear(); }

  QStack<ChangedObject<T>*>* value(const int i) const { return m_list.value(i); }
  QStack<ChangedObject<T>*>* top() const { return m_list.top(); }
  QStack<ChangedObject<T>*>* takeAt(const int i) { return m_list.takeAt(i); }
  QStack<ChangedObject<T>*>* pop() { return m_list.isEmpty() ? nullptr : m_list.pop(); }

  // Make versions that return a changed object.
  // These are never used so they should probably be deleted.
  // The idea was that we new that there was that the requested stack item contained a single item.
  // ChangedObject<T>* valueObject(const int i) const;
  // ChangedObject<T>* topObject() const;
  // ChangedObject<T>* takeObjectAt(const int i);
  // ChangedObject<T>* popObject();

  /*! \brief Push a "stack" of changes onto the list of changes.
   *
   * Each "stack" object is considered a single user change.
   *
   *  \param [in] stack A list of changes to track.
   */
  void push(QStack<ChangedObject<T>*>* stack) { if (stack != nullptr) m_list.push(stack); }


  /*! \brief Add a single change object.
   *
   * This creates a single change object, stores it in a stack of size one, and pushes that stack onto the change list.
   *
   *  \param [in] row Row that is changed.
   *  \param [in] col Column for the changed data unless this is an entire row, then -1.
   *  \param [in] changeInfo Column name if a single column update (col >= 0) and empty for a row update such as new row or delete row.
   *  \param [in] changeType  (add, delete, edit, unknown)
   *  \param [in] newData The new data that must be saved; ownership is taken and it is destroyed in the destructor.
   *  \param [in] oldData Original data used if "undo" is used; ownership is taken and it is destroyed in the destructor.
   */
  void push(const int row, const int col, const QString& changeInfo, ChangedObjectBase::ChangeType changeType=ChangedObjectBase::Unknown, T* newData=nullptr, T* oldData=nullptr);

  /*! \brief Number of elements in the list. */
  int size() const { return m_list.size(); }

  /*! \brief Is the change stack empty? */
  bool isEmpty() const { return m_list.isEmpty(); }

  /*! \brief Delete every element from every contained stack, delete everything in the list, then clear the list. */
  virtual void clear();

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

/**
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

**/

#endif // CHANGETRACKER_H
