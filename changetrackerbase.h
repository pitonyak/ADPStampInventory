#ifndef CHANGETRACKERBASE_H
#define CHANGETRACKERBASE_H

#include "changedobjectbase.h"

#include <QObject>

class ChangeTrackerBase : public QObject
{
  Q_OBJECT
public:

  explicit ChangeTrackerBase(QObject *parent = nullptr);
  virtual ~ChangeTrackerBase();

signals:

public slots:
};

#endif // CHANGETRACKER_H
