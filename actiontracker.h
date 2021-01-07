#ifndef ACTIONTRACKER_H
#define ACTIONTRACKER_H

#include "actiontrackerobject.h"

#include <QVector>

//**************************************************************************
//! Array of tracked objects in the order that the objects were added.
/*!
 * The object derives from QVector, and is nothing more than a wrapper for nice naming conventions.
 * Nothing more than a set of constructors that pass through to the base class and a destructor. All are empty.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2019-2021
 ***************************************************************************/
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
