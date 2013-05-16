#ifndef STAMPDB_H
#define STAMPDB_H

#include <QObject>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QMap>

class QSqlRecord;
class QSqlField;
class QStringList;

//**************************************************************************
/*! \class StampDB
 * \brief Encapsulate database access.
 *
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2012-2013
 **************************************************************************/

class StampDB : public QObject
{
  Q_OBJECT
public:
  /*! Constructor */
  explicit StampDB(QObject *parent = 0);

  /*! Destructor */
  ~StampDB();

  /*! \brief Initialize the DB as needed, and then open it.
   *
   *  \return The True on success.
   */
  bool openDB();

  /*! \brief Close the DB if it is open */
  void closeDB();

  /*! \brief Close the DB if it is open, then delete the file.
   *
   *  \return The True on success.
   */
  bool deleteDB();

  /*! \brief Create the scehma. Tables that exist are left unchanged.
   *
   *  \return The True on success.
   */
  bool createSchema();


  /*! \brief Returns DDL for all tables in the DB.
   *
   *  \return DDL for all tables in the DB.
   */
  QString getSchema() const;

  /*! \brief Return the DDL required to create the table without the "create table" portion.
   *
   *  This is a wrapp that calls getSchema("", record).
   *
   *  \param [in] record List of fields for which DDL is created.
   *  \return DDL required to create the table without the "create table" portion.
   */
  QString getSchema(const QSqlRecord& record) const;

  /*! \brief Return the DDL required to create the table.
   *
   *  If the table name is included, the DDL includes the "create table" portion, if not,
   *  the DDL for each field is returned.
   *
   *  \param [in] tableName Table name for the record; because the record does not contain the table name.
   *  \param [in] record List of fields for which DDL is created.
   *  \return DDL required to create the table.
   */
  QString getSchema(const QString& tableName, const QSqlRecord& record) const;

  /*! \brief Return DDL required to create this one field in a table.
   *
   * Poorly tested and only assumed to work for my simple fields with the simple DB.
   * Currently ignores things such as default value, precision, read-only, or required.
   *
   *  \param [in] field Field for which DDL is returned.
   *  \return DDL required to create the field.
   */
  QString getSchema(const QSqlField& field) const;


  /*! \brief Initizlize (if needed) m_tableMap and return a reference.
   *
   *  \return Reference to m_tableMap
   */
  const QMap<QString, QSqlRecord>& tableMap();

  QSqlError lastError();

  /*! \brief Returns the full path to the datasource, which includes the file name.
   *
   *  \return The full path to the datasource, which includes the file name.
   */
  const QString& pathToDB() const;


  /*! \brief Set the full path to the datasource, which includes the file name.
   *
   * If the database is currently open and the name changes, the database is closed before the name is changed.
   *
   *  \param [in] fullpath Full path to the datasource, which includes the file name.
   */
  void pathToDB(const QString& fullpath);

signals:

public slots:

private:

  /*! True if DB driver has been obtained.  */
  bool m_dbIsInitialized;

  /*! Associate the DDL from m_outerDDLRegExp to record types, which might be useful... */
  QMap<QString, QSqlRecord> *m_tableMap;

  /*! Regular expression that matches on "create table  <name> (...)"  */
  QRegExp *m_outerDDLRegExp;

  /*! Each string is the DDL to create a single table. */
  QStringList *m_desiredSchemaDDLList;

  /*!   */
  QString m_pathToDB;

  /*!   */
  QSqlDatabase m_db;
};

#endif // STAMPDB_H
