#ifndef CHANGETRACKERBASE_H
#define CHANGETRACKERBASE_H

#include "changedobjectbase.h"

#include <QObject>

//**************************************************************************
/*! \class ChangeTrackerBase
 * \brief Base for tracking changes. No real code here except to set the parent object. We can add functionality as needed.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2012-2019
 **************************************************************************/
class ChangeTrackerBase : public QObject
{
  Q_OBJECT
public:

  /*! \brief Constructor
   *
   *  \param [in] parent The parent of an object may be viewed as the object's owner. The destructor of a parent object destroys all child objects.
   */
  explicit ChangeTrackerBase(QObject *parent = nullptr);
  virtual ~ChangeTrackerBase();

signals:

public slots:
};

#endif // CHANGETRACKER_H
