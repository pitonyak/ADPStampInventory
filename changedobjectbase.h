#ifndef CHANGEDOBJECTBASE_H
#define CHANGEDOBJECTBASE_H

#include <QObject>

class ChangedObjectBase : public QObject
{
  Q_OBJECT
public:

  //**************************************************************************
  /*! \brief Supported change types! */
  //**************************************************************************
  enum ChangeType {Unknown, Add, Delete, Edit};

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
  virtual ChangedObjectBase* clone() const = 0;

signals:

public slots:

private:
  ChangeType m_changeType;
  QString m_changeInfo;
  int m_row;
  int m_col;

};

#endif // CHANGEDOBJECTBASE_H
