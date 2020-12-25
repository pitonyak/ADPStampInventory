#ifndef STAMPDB_H
#define STAMPDB_H

#include "describesqltables.h"
#include "genericdatacollections.h"

#include <QObject>
#include <QString>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QMap>
#include <QList>

class QSqlRecord;
class QSqlField;
class CSVReader;
class QDir;
class DataObjectBase;
class GenericDataCollection;

//**************************************************************************
/*! \class StampDB
 * \brief Encapsulate database access.
 *
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2012-2020
 **************************************************************************/

class StampDB : public QObject
{
  Q_OBJECT
public:
  /*! \brief Constructor
   *
   *  \param [in, out] parent The object's owner. The parent's destructor destroys this object.
   */
  explicit StampDB(QObject *parent = nullptr);

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

  /*! \brief Search for a table name in a case-insensitive way.
   *
   * If a table name matches with the exact case, it is returned. If not,
   * a case insensitive search is performed.
   *
   *  \param [in] aName Name of the table to find.
   *
   *  \return The exact table name as used by the system.
   */
  QString getClosestTableName(const QString& aName);

  QStringList getTableNames(const bool ignoreSystemTables=true);

  /*! \brief Go directly to the DB and get the column names associated with this table.
   *
   *  \param [in] aName Name of the table to find.
   *
   *  \return List of column names for the specified tables.
   */
  QStringList getColumnNames(const QString& tableName);

  /**
   * @brief getDDLForExport Get the DDL for all tables. This method is used to create the DDL file during a CSV export.
   * @return List of DDL, one entry for each table.
   */
  QStringList getDDLForExport();

  /*! \brief Read all data from a table.
   *
   *  \param [in] tableName
   *  \param [in] useSchema If true, validate the input based on the schema and convert to an appropriate type.
   *  \param [in] includeLinks If true, include "linked" tables with the data.
   *
   *  \return a new generic data collection that you now own and must delete (or, nullptr if it fails).
   */
  GenericDataCollection* readTableName(const QString& tableName, const bool sortByKey=true, const bool useSchema=false);

  /*! \brief Read all data from a table and attempt to set the type based on the schema data.
   *
   * If a smarter DB is used, then we probably do not need to map the types.
   * The SqlFieldTypeMaster will automatically map all BIT and BIT Varying types to a Bool,
   * which is wrong for a sequence of bits.
   *
   *  \param [in] tableName
   *  \param [in] sortByKey If true, the sort key list is automatically created based on the key fields (probably "id");
   *
   *  \return a new generic data collection that you now own and must delete (or, nullptr if it fails).
   */
  GenericDataCollection* readTableBySchema(const QString& tableName, const bool sortByKey=true);

  /*! \brief Read all data from a table and attempt to set the type based on the schema data.
   *
   * If a smarter DB is used, then we probably do not need to map the types.
   * The SqlFieldTypeMaster will automatically map all BIT and BIT Varying types to a Bool,
   * which is wrong for a sequence of bits.
   *
   *  \param [in] tableName
   *  \param [in] orderByList Field name list by which the data is ordered.
   *
   *  \return a new generic data collection that you now own and must delete (or, nullptr if it fails).
   */
  GenericDataCollection* readTableBySchema(const QString& tableName, const QStringList& orderByList);

  /*! \brief Return a collection of tables including referenced tables, and yes, it pulls it all into memory.
   *
   * Fields know if they link to another table. For example, bookvalues.catalogid references catalog.id.
   * When I ask for the bookvalues table, therefore, it will also return the catalog table.
   *
   *  \param [in] tableName
   *  \param [in] maxLinkDepth - Just in case. Set to -1 to just keep going.
   *  \param [in] sortByKey If true, reads the data ordered by "id" (database key), otherwise, do not purposely sort the data.
   *
   *  \return a new generic data collection that you now own and must delete (or, nullptr if it fails).
   */
  GenericDataCollections* readTableWithLinks(const QString& tableName, const int maxLinkDepth=-1, const bool sortByKey=true);

  /*! \brief Return the maximum value from the field "id" in the specified tablename.
   *
   *  \param [in] tableName
   *
   *  \return Maximum value from the field "id" in the specified tablename.
   */
  int getMaxId(const QString& tableName);

  /*! \brief Return the maximum value from the specified field and table.
   *
   *  \param [in] tableName
   *  \param [in] fieldName
   *
   *  \return Maximum value from the specified field and table.
   */
  int getMaxId(const QString& tableName, const QString& fieldName);

  /*! \brief Return the first value in the first column, which is assumed to be an ID.
   *
   * The first column returned by the SQL command had better be convertable to an int.
   * Best bet is to have the SQL return a single value.
   *
   *  \param [in] sql
   *
   *  \return First value in the first column, which is assumed to be an ID.
   */
  int getIdFromSql(const QString& sql);

  /*! \brief Sort the value source alphabtically and return the first ID.
   *
   * This should return the "newest" book value.
   *
   *  \return The ID for the latest book values.
   */
  int getMaxValueSourceId();

  int selectValueSourceId(QWidget* parent);

  //GenericDataCollection* readTableName(const QString& tableName, const bool useSchema, const bool includeLinks);

  /*! \brief Execute SQL and create a table.
   *
   *  \param [in] sql
   *
   *  \return a new generic data collection that you now own and must delete (or, nullptr if it fails).
   */
  GenericDataCollection* readTableSql(const QString& sql);

  /*! \brief Read an already opened CSV file into an existing table.
   *
   *  \param [in,out] reader
   *
   *  \param [in] tableName Name of the table that will receive the CSV data.
   *
   *  \return True on success, false on failure.
   */
  bool loadCSV(CSVReader& reader, const QString& tableName);

  bool exportToCSV(const QDir& outputDir, const bool overwrite=false);

  QSqlDatabase& getDB() { return m_db; }

  //**************************************************************************
  /*! \brief Execute an SQL Query
   *
   *  \param [in] sqlSelect Query to execute
   *  \param [out] records List of records as returned by the SQL command.
   *  \param [in] keyField Field name containing the DB Key, probably "id". May be blank.
   *  \param [out] keys If we have a valid keyField, then this associates the ID to the returned record.
   *  \return True on success, false otherwise.
   ***************************************************************************/
  bool executeQuery(const QString& sqlSelect, QList<QSqlRecord>& records, const QString& keyField, QHash<int, int>& keys);

  /*!
   * @brief getOneColumnAsString Get every value for the first column returned by the SQL statement.
   * @param sqlSelect SQL statement assumed to select at least one row.
   * @return List of every value in the first column.
   */
  QStringList getOneColumnAsString(const QString& sqlSelect);

  DescribeSqlTables m_schema;

signals:

public slots:

private:

  /*! True if DB driver has been obtained.  */
  bool m_dbIsInitialized;

  /*! Associate the DDL from m_outerDDLRegExp to record types, which might be useful... */
  QMap<QString, QSqlRecord> *m_tableMap;

  /*! Regular expression that matches on "create table  <name> (...)"  */
  QRegularExpression *m_outerDDLRegExp;

  // TODO: Replace this with the DDL returned by DescribeTables class.
  /*! Each string is the DDL to create a single table. */
  QStringList *m_desiredSchemaDDLList;

  /*!   */
  QString m_pathToDB;

  /*!   */
  QSqlDatabase m_db;
};

#endif // STAMPDB_H
