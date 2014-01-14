#ifndef ACTIONTRACKER_H
#define ACTIONTRACKER_H

#include "actiontrackerobject.h"

#include <QVector>

template <class T>
class ActionTracker : QVector< ActionTrackerObject<T> >
{
public:
  ActionTracker();
  ActionTracker(int size);
  ActionTracker(int size, const ActionTrackerObject<T>& value);
  ActionTracker(const ActionTracker<T>& other);
  ActionTracker(const QVector< ActionTrackerObject<T> >& other);
  ActionTracker(std::initializer_list< ActionTrackerObject<T> > args);
  ~ActionTracker();
};

#endif // ACTIONTRACKER_H
