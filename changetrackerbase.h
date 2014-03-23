#ifndef CHANGETRACKERBASE_H
#define CHANGETRACKERBASE_H

#include "changedobjectbase.h"

#include <QObject>
#include <QStack>

class ChangeTrackerBase : public QObject
{
  Q_OBJECT
public:

  explicit ChangeTrackerBase(QObject *parent = nullptr);
  virtual ~ChangeTrackerBase();

  int size() const { return m_list.size(); }
  bool isEmpty() const { return m_list.isEmpty(); }
  void clear();

protected:
  ChangedObjectBase* valueBase(const int i) const { return m_list.value(i); }
  ChangedObjectBase* topBase() const { return m_list.top(); }
  ChangedObjectBase* takeAtBase(const int i) { return m_list.takeAt(i); }
  ChangedObjectBase* popBase() { return m_list.pop(); }
  void pushBase(ChangedObjectBase* obj) { if (obj != nullptr) m_list.push(obj); }

signals:

public slots:

protected:
  QStack<ChangedObjectBase*> m_list;
};

#endif // CHANGETRACKER_H
