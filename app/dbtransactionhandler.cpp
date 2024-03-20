#include "dbtransactionhandler.h"

#include <QSqlDatabase>

DBTransactionHandler::DBTransactionHandler(QSqlDatabase &db) : m_db(db), m_hasTransaction(false)
{
  m_hasTransaction = db.transaction();
}

DBTransactionHandler::~DBTransactionHandler()
{
  if (m_hasTransaction)
  {
    m_db.rollback();
  }
}

bool DBTransactionHandler::commit()
{
  if (m_hasTransaction)
  {
    m_hasTransaction = false;
    return m_db.commit();
  }

  return false;
}

bool DBTransactionHandler::rollback()
{
  if (m_hasTransaction)
  {
    m_hasTransaction = false;
    return m_db.rollback();
  }

  return false;
}


