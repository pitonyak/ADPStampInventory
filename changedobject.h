#ifndef CHANGEDOBJECT_H
#define CHANGEDOBJECT_H

#include "changedobjectbase.h"

//**************************************************************************
/*! \class ChangedObject
 * \brief Contains the new and changed object, both are owned by this object.
 *
 * Common information such as row, column, and type is stored in the base class.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2012-2019
 **************************************************************************/
template <class T>
class ChangedObject : public ChangedObjectBase
{
public:
  /*! \brief Constructor
   *
   *  Changed and original object are owned by this object.
   *  \param [in] row Row that is changed.
   *  \param [in] col Column for the changed data unless this is an entire row, then -1.
   *  \param [in] changeInfo Column name if a single column update (col >= 0) and empty for a row update such as new row or delete row.
   *  \param [in] changeType (add, delete, edit, unknown)
   *  \param [in] newData The new data that must be saved; ownership is taken and it is destroyed in the destructor.
   *  \param [in] oldData Original data used if "undo" is used; ownership is taken and it is destroyed in the destructor.
   *  \param [in] parent The parent of an object may be viewed as the object's owner. The destructor of a parent object destroys all child objects.
   */
  explicit ChangedObject(const int row, const int col, const QString& changeInfo, ChangeType changeType=ChangedObjectBase::Unknown, T* newData = nullptr, T* oldData = nullptr, QObject *parent = nullptr) : ChangedObjectBase(row, col, changeInfo, changeType, parent), m_newData(newData), m_oldData(oldData) {}

  virtual ~ChangedObject();

  //virtual ChangedObjectBase* clone() const { return new ChangedObject<T>(*this); }

  T* getNewData() const { return m_newData; }
  T* getOldData() const { return m_oldData; }

  T* takeNewData() { T* data = m_newData; m_newData = nullptr; return data; }
  T* takeOldData() { T* data = m_oldData; m_oldData = nullptr; return data; }

  //T* cloneNewData() { return m_newData == nullptr ? nullptr : m_newData->clone(); }
  //T* cloneOldData() { return m_oldData == nullptr ? nullptr : m_oldData->clone(); }

private:
  T* m_newData;
  T* m_oldData;

  // This is private, and there is no data, so, it cannot be created.
  // If this is allowed, then we probably need to be able to clone the new and old data to be safe.
  ChangedObject(const ChangedObject& obj);
  //explicit ChangedObject(const ChangedObject& obj, QObject *parent = nullptr) : ChangedObjectBase(obj, parent), m_newData(obj.cloneNewData()), m_oldData(obj.cloneOldData()) {}

};

template <class T> inline ChangedObject<T>::~ChangedObject()
{
  if (m_newData != nullptr)
  {
    delete m_newData;
  }
  if (m_oldData != nullptr)
  {
    delete m_oldData;
  }
}


#endif // CHANGEDOBJECT_H
