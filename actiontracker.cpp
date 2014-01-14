#include "actiontracker.h"

template <class T>
ActionTracker<T>::ActionTracker() :
  QVector< ActionTrackerObject<T> >()
{
}

template <class T>
ActionTracker<T>::ActionTracker(int size) :
  QVector< ActionTrackerObject<T> >(size)
{
}

template <class T>
ActionTracker<T>::ActionTracker(int size, const ActionTrackerObject<T>& value) :
  QVector< ActionTrackerObject<T> >(size, value)
{
}

template <class T>
ActionTracker<T>::ActionTracker(const ActionTracker<T>& other) :
  QVector< ActionTrackerObject<T> >(other)
{
}

template <class T>
ActionTracker<T>::ActionTracker(const QVector<ActionTrackerObject<T> > &other) :
  QVector< ActionTrackerObject<T> >(other)
{
}

template <class T>
ActionTracker<T>::ActionTracker(std::initializer_list< ActionTrackerObject<T> > args) :
   QVector< ActionTrackerObject<T> >(args)
{
}

template <class T>
ActionTracker<T>::~ActionTracker()
{
}

