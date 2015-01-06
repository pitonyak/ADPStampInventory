#ifndef DBTRANSACTIONHANDLER_H
#define DBTRANSACTIONHANDLER_H

class QSqlDatabase;

//**************************************************************************
/*! \class DBTransactionHandler
 * \brief Provide an easy way to make sure that a transaction is closed.
 *
 * For some databases, commit or rollback fails and return false if there is an active
 * query using the database for a SELECT. Make the query inactive before doing the commit or rollback.
 * This class does not consider an active statement, so, it is best to define the query in a different
 * scope so that it is closed automagically before this class destructor is called.
 *
 * The destructor will perform a rollback, so, commit the transaction manually.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2015
 **************************************************************************/
class DBTransactionHandler
{
public:
  DBTransactionHandler(QSqlDatabase &db);
  ~DBTransactionHandler();

  bool commit();
  bool rollback();

private:
  QSqlDatabase& m_db;
  bool m_hasTransaction;

  // Do not implement these so you cannot use these.
  DBTransactionHandler();
  DBTransactionHandler(const DBTransactionHandler&);
  const DBTransactionHandler& operator=(const DBTransactionHandler&);

};

#endif // DBTRANSACTIONHANDLER_H
