#ifndef CHANGEDOBJECTBASE_H
#define CHANGEDOBJECTBASE_H

#include <QObject>

//**************************************************************************
/*! \class ChangedObjectBase
 * \brief Base for tracking changed objects so the Template class will contain less code.
 *
 * Row, column, and type is similar for all objects. The Template class will contain the changed and original object.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2012-2019
 **************************************************************************/
class ChangedObjectBase : public QObject
{
  Q_OBJECT
public:

  //**************************************************************************
  /*! \brief Supported change types! */
  //**************************************************************************
  enum ChangeType {Unknown, Add, Delete, Edit};

  /*! \brief Constructor
   *
   *  Changed and original object are owned by this object.
   *  \param [in] row Row that is changed.
   *  \param [in] col Column for the changed data unless this is an entire row, then -1.
   *  \param [in] changeInfo
   *  \param [in] changeType Column name if a single column update (col >= 0) and empty for a row update such as new row or delete row.
   *  \param [in] newData The new data that must be saved; ownership is taken and it is destroyed in the destructor.
   *  \param [in] oldData Original data used if "undo" is used; ownership is taken and it is destroyed in the destructor.
   *  \param [in] parent The parent of an object may be viewed as the object's owner. The destructor of a parent object destroys all child objects.
   */
  explicit ChangedObjectBase(const int row, const int col, const QString& changeInfo, ChangeType changeType=ChangedObjectBase::Unknown, QObject *parent = nullptr);
  explicit ChangedObjectBase(const ChangedObjectBase& obj, QObject *parent = nullptr);

  const ChangedObjectBase& operator=(const ChangedObjectBase& obj);

  virtual ~ChangedObjectBase();

  ChangeType getChangeType() const { return m_changeType; }
  void setChangeType(const ChangeType changeType) { m_changeType = changeType; }

  const QString& getChangeInfo() const { return m_changeInfo; }
  void setChangeInfo(const QString& changeInfo) { m_changeInfo = changeInfo; }

  void setRow(const int row) { m_row = row; }
  void setCol(const int col) { m_col = col; }

  int getRow() const { return m_row; }
  int getCol() const { return m_col; }

  // clone
  //virtual ChangedObjectBase* clone() const = 0;

signals:

public slots:

private:
  /*! Usually add, delete, or edit. */
  ChangeType m_changeType;

  /*! When updating a field, this is the field name. Updating a row, this is empty. */
  QString m_changeInfo;
  int m_row;
  int m_col;

};

#endif // CHANGEDOBJECTBASE_H
