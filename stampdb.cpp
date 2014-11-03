#include "stampdb.h"
#include "scrollmessagebox.h"
#include "csvreader.h"
#include "csvwriter.h"
#include "genericdatacollection.h"

#include <QFile>
#include <QDir>
#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStringList>
#include <QRegExp>
#include <QMessageBox>
#include <QVariant>
#include <QMap>
#include <QSqlDriver>
#include <QTextStream>
#include <QList>
#include <QtGlobal>

#if defined(__GNUC__)
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 6)
#include "nullptr.h"
#endif
#endif

StampDB::StampDB(QObject *parent) :
  QObject(parent),
  m_dbIsInitialized(false),
  m_tableMap(nullptr),
  m_outerDDLRegExp(nullptr),
  m_desiredSchemaDDLList(nullptr)
{
  m_desiredSchemaDDLList = new QStringList();

  *m_desiredSchemaDDLList << "CREATE TABLE catalogtype("
                             " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             " name varchar(100),"
                             " description varchar(256))";

  *m_desiredSchemaDDLList << "CREATE TABLE country("
                             " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             " name varchar(100),"
                             " a3 varchar(3))";

  *m_desiredSchemaDDLList << "CREATE TABLE dealer("
                             " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             " updated TIMESTAMP,"
                             " name varchar(75),"
                             " contact varchar(75),"
                             " address1 varchar(75),"
                             " address2 varchar(75),"
                             " address3 varchar(75),"
                             " address4 varchar(75),"
                             " phone varchar(32),"
                             " fax varchar(32),"
                             " comment varchar(100),"
                             " email varchar(75),"
                             " website varchar(100))";

  *m_desiredSchemaDDLList << "CREATE TABLE stamplocation("
                             " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             " name varchar(100),"
                             " description varchar(256))";

  *m_desiredSchemaDDLList << "CREATE TABLE valuetype("
                             " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             " description varchar(100))";

  *m_desiredSchemaDDLList << "CREATE TABLE valuesource("
                             " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             " year DATE,"
                             " description varchar(100))";

  *m_desiredSchemaDDLList << "CREATE TABLE catalog("
                             " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             " scott varchar(20),"
                             " countryid INTEGER,"
                             " typeid INTEGER,"
                             " releasedate DATE,"
                             " updated TIMESTAMP,"
                             " facevalue FLOAT,"
                             " description varchar(100))";

  *m_desiredSchemaDDLList << "CREATE TABLE bookvalues("
                             " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             " catalogid INTEGER,"
                             " sourceid INTEGER,"
                             " valuetypeid INTEGER,"
                             " bookvalue FLOAT)";

  *m_desiredSchemaDDLList << "CREATE TABLE inventory("
                             " id INTEGER PRIMARY KEY AUTOINCREMENT,"
                             " catalogid INTEGER,"
                             " quantity INTEGER,"
                             " grade VARCHAR(12),"
                             " condition VARCHAR(20),"
                             " selvage VARCHAR(20),"
                             " centering VARCHAR(30),"
                             " back VARCHAR(20),"
                             " comment VARCHAR(100),"
                             " purchasedate DATE,"
                             " paid FLOAT,"
                             " dealerid INTEGER,"
                             " locationid INTEGER,"
                             " replace BOOLEAN,"
                             " updated TIMESTAMP,"
                             " typeid INTEGER,"
                             " valuemultiplier FLOAT)";

  m_outerDDLRegExp = new QRegExp("^\\s*create\\s+table\\s+([a-z0-9_\\-\\.]+)\\s*\\((.*)\\)\\s*$");
  m_outerDDLRegExp->setCaseSensitivity(Qt::CaseInsensitive);

  // There are now two distinct methods to get the stamp schema!
  m_schema = DescribeSqlTables::getStampSchema();
}

StampDB::~StampDB()
{
  closeDB();
  if (m_tableMap != nullptr) {
    delete m_tableMap;
    m_tableMap = nullptr;
  }
  if (m_desiredSchemaDDLList != nullptr) {
    delete m_desiredSchemaDDLList;
    m_desiredSchemaDDLList = nullptr;
  }
  if (m_outerDDLRegExp != nullptr) {
    delete m_outerDDLRegExp;
    m_outerDDLRegExp = nullptr;
  }
}

const QString& StampDB::pathToDB() const
{
  return m_pathToDB;
}

void StampDB::pathToDB(const QString& fullpath)
{
  if (m_pathToDB.compare(fullpath, Qt::CaseSensitive) != 0) {
    if (m_dbIsInitialized) {
      m_db.close();
      m_db.setDatabaseName(fullpath);
    }
    m_pathToDB = fullpath;
  }
}

bool StampDB::openDB()
{
  if (!m_db.isOpen()) {
    if (m_pathToDB.length() == 0) {
      // TODO: error handling
      qDebug("Empty path to database");
      return false;
    }
    if (!QFile::exists(m_pathToDB)) {
      QFileInfo fileInfo(m_pathToDB);
      if (!fileInfo.absoluteDir().exists()) {
        // TODO: error handling
        qDebug("FileInfo absolute dir");
        qDebug(qPrintable(fileInfo.absoluteDir().absolutePath()));
        return false;
      }
    }
    if (!m_dbIsInitialized) {
      // Find QSLite driver
      m_db = QSqlDatabase::addDatabase("QSQLITE");
      m_dbIsInitialized = true;
    }
    m_db.setDatabaseName(m_pathToDB);
    return m_db.open();
  }
  return true;
}

void StampDB::closeDB()
{
  if (m_db.isOpen()) {
    m_db.close();
  }
}


bool StampDB::deleteDB()
{
  closeDB();
  return QFile::remove(pathToDB());
}

QSqlError StampDB::lastError()
{
  // If opening database has failed user can ask
  // error description by QSqlError::text()
  return m_db.lastError();
}

bool StampDB::createSchema()
{
  bool ret = false;
  if (openDB()) {

    QRegExp tableNameRegExp("create\\s+table\\s+(\\w+)");
    tableNameRegExp.setCaseSensitivity(Qt::CaseInsensitive);

    QSqlQuery query(m_db);
    QStringList tables = m_db.tables(QSql::Tables);
    ret = true;
    for (int i=0; i<m_desiredSchemaDDLList->size() && ret; ++i) {
      const QString& ddl = m_desiredSchemaDDLList->at(i);
      if (tableNameRegExp.indexIn(ddl) == -1) {

        QMessageBox msgBox;
        msgBox.setText(tr("Schema Error: DDL does not contain 'create table'"));
        msgBox.setInformativeText(ddl);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        ret = false;

      } else if (tables.contains(tableNameRegExp.cap(1), Qt::CaseInsensitive)) {

        // Table already exists.
        /*
                QMessageBox msgBox;
                msgBox.setText(tr("Table '%1' already exists").arg(tableNameRegExp.cap(1)));
                msgBox.setInformativeText(tr("DDL:%1").arg(ddl));
                msgBox.setStandardButtons(QMessageBox::Ok);
                msgBox.setDefaultButton(QMessageBox::Ok);
                msgBox.exec();
                */
        //query.exec(QString("DROP TABLE %1").arg(tableNameRegExp.cap(1)));

      } else {
        QString tableName = tableNameRegExp.cap(1);
        ret = query.exec(ddl);
        if (!ret) {
          QMessageBox msgBox;
          msgBox.setText(tr("Schema Error creating table '%1'").arg(tableName));
          msgBox.setInformativeText(tr("Error:%1\nDDL:%2").arg(m_db.lastError().text(), ddl));
          msgBox.setStandardButtons(QMessageBox::Ok);
          msgBox.setDefaultButton(QMessageBox::Ok);
          msgBox.exec();
          query.exec(QString("DROP TABLE %1").arg(tableName));
        }
      }
    }
  }
  return ret;
}

QString StampDB::getSchema() const
{
  QString s;
  QStringList tables = m_db.tables(QSql::Tables);
  for (int i=0; i<tables.size(); ++i) {
    if (i > 0) {
      s += "\n\n";
    }
    const QString tableName = tables.at(i);
    s += getSchema(tableName, m_db.record(tableName));
  }
  return s;
}

QString StampDB::getSchema(const QSqlRecord& record) const
{
  return getSchema("", record);
}


QString StampDB::getSchema(const QString& tableName, const QSqlRecord& record) const
{
  QString s;
  if (!record.isEmpty()) {
    QString indent = "";
    if (!tableName.isEmpty()) {
      s = "CREATE TABLE ";
      s += tableName;
      s += "(";
      indent = "    ";
    }
    for (int i=0; i<record.count(); ++i) {
      if (i > 0) {
        s += ",";
      }
      s += "\n" + indent;
      s += getSchema(record.field(i));
    }
    if (!tableName.isEmpty()) {
      s += ")";
    }
  }
  return s;
}

QString StampDB::getSchema(const QSqlField& field) const
{
  QString s = field.name() + " ";
  switch (field.type()) {
  case QVariant::String :
    if (field.length() <= 0) {
      // Length is not available from the DB.
      // This must be SQLite.
      s += "VARCHAR";
    } else {
      s += QString("VARCHAR(%1)").arg(field.length());
    }
    break;
  case QVariant::Int :
    s += "INTEGER";
    break;
  case QVariant::Double :
    s += "DOUBLE";
    break;
  case QVariant::Date :
    // SQLite returns a String for date.
    s += "DATE";
    break;
  case QVariant::DateTime :
    // SQLite returns a String for date / time.
    s += "TIMESTAMP";
    break;
  default:
    s += QVariant::typeToName(field.type());
  }

  if (field.isAutoValue()) {
    s += " AUTOINCREMENT";
  }
  return s;
}

QStringList StampDB::getTableNames(const bool ignoreSystemTables)
{
    if (!ignoreSystemTables)
    {
        return m_db.tables(QSql::Tables);
    }

    QString sql = "SELECT tbl_name FROM sqlite_master WHERE type='table' and tbl_name<>'sqlite_sequence' order by tbl_name";
    return getOneColumnAsString(sql);
}

QStringList StampDB::getColumnNames(const QString& tableName)
{
  QStringList columnNames;
  if (openDB())
  {
    // The QSqlRecord class encapsulates the functionality and characteristics of a
    // database record (usually a row in a table or view within the database).
    // Get the values associated with this table.
    QSqlRecord record = m_db.record(tableName);
    for (int i=0; i<record.count(); ++i) {
      columnNames.append(record.field(i).name());
    }
  }
  return columnNames;
}

QStringList StampDB::getDDLForExport()
{
    QString sql = "SELECT sql FROM sqlite_master WHERE type='table' and tbl_name<>'sqlite_sequence' order by tbl_name";
    return getOneColumnAsString(sql);
}


GenericDataCollection *StampDB::readTableName(const QString& tableName, const bool useSchema, const bool includeLinks)
{
  if (!useSchema) {
    return readTableSql(QString("select * from %1 order by ID").arg(tableName));
  }
  return readTableSql(QString("select * from %1 order by ID").arg(tableName));
}

GenericDataCollection* StampDB::readTableBySchema(const QString& tableName, const bool includeLinks)
{
  Q_ASSERT_X(m_schema.hasTable(tableName), "readTableBySchema", qPrintable(QString("Table [%1] is not in the schema.").arg(tableName)));
  QStringList orderByList;
  // TODO: Find the "key" field and use that instead of ID.
  orderByList << "id";
  return readTableBySchema(tableName, orderByList, includeLinks);
}

GenericDataCollection* StampDB::readTableBySchema(const QString& tableName, const QStringList& orderByList, const bool includeLinks)
{
  Q_ASSERT_X(m_schema.hasTable(tableName), "readTableBySchema", qPrintable(QString("Table [%1] is not in the schema.").arg(tableName)));
  DescribeSqlTable table = m_schema.getTableByName(tableName);
  QStringList fieldNames = table.getFieldNames();
  for (QStringListIterator fieldSortIterator(orderByList); fieldSortIterator.hasNext(); )
  {
    QString fieldName = fieldSortIterator.next();
    Q_ASSERT_X(table.hasField(fieldName), "readTableBySchema", qPrintable(QString("Table [%1] does not have a field named [%2].").arg(tableName).arg(fieldName)));
    // TODO: Build sort list and arrange to allow direction to be included.
  }

  for (QStringListIterator fieldIterator(fieldNames); fieldIterator.hasNext(); )
  {

  }
  return nullptr;
}

GenericDataCollection* StampDB::readTableSql(const QString& sql)
{
  if (openDB())
  {
    qDebug(qPrintable(QString("Reading table using [%1]").arg(sql)));
    QSqlDatabase& db = getDB();
    QSqlQuery query(db);

    if (!query.exec(sql))
    {
        ScrollMessageBox::information(nullptr, "ERROR", query.lastError().text());
    }
    else if (query.isSelect())
    {
      GenericDataCollection* collection = new GenericDataCollection();

      // Set the property names and types in order!
      QStringList duplicateColumns;
      for (int i=0; i<query.record().count(); ++i)
      {
        if (!collection->appendPropertyName(query.record().fieldName(i), query.record().field(i).type()))
        {
          duplicateColumns.append(query.record().fieldName(i));
        }
      }
      if (duplicateColumns.count() > 0)
      {
        ScrollMessageBox::information(nullptr, "ERROR", QString(tr("Problem converting the following SQL\n\n%1\n\nThe following columns are duplicated:\n%2")).arg(sql).arg(duplicateColumns.join("\n")));
        delete collection;
        return nullptr;
      }

      bool hasIdColumn = collection->hasProperty("id");
      QString idString = hasIdColumn ? collection->getPropertyName("id") : "";
      int iCount = 0;
      while (query.isActive() && query.next())
      {
        GenericDataObject* gdo = new GenericDataObject(collection);
        for (int i=0; i<query.record().count(); ++i)
        {
          if (!query.record().isNull(i))
          {
            gdo->setValue(collection->getPropertyName(i), query.record().value(i));
          }
        }
        collection->appendObject(hasIdColumn ? gdo->getInt(idString) : iCount, gdo);
        ++iCount;
      }
      return collection;
    }
  }

  return nullptr;
}

QString StampDB::getClosestTableName(const QString& aName)
{
  if (openDB())
  {

    QStringList tables = m_db.tables(QSql::Tables);
    //ScrollMessageBox::information(nullptr, "Tables", tables.join("\n"));
    int i = tables.indexOf(aName);
    if (i >= 0)
    {
      return tables[i];
    }
    for (i =0; i<tables.size(); ++i)
    {
      if (aName.compare(tables[i], Qt::CaseInsensitive) == 0)
      {
        return tables[i];
      }
    }
  }
  return "";
}

QStringList StampDB::getOneColumnAsString(const QString& sqlSelect)
{
    QStringList colStrings;
    if (openDB())
    {
        QSqlDatabase& db = getDB();
        QSqlQuery query(db);

        if (!query.exec(sqlSelect))
        {
            ScrollMessageBox::information(nullptr, "ERROR", query.lastError().text());
        }
        else if (query.isSelect())
        {
            while (query.isActive() && query.next())
            {
                colStrings.append(query.record().value(0).toString());
            }
        }
    }

    return colStrings;
}

bool StampDB::executeQuery(const QString& sqlSelect, QList<QSqlRecord>& records, const QString &keyField, QHash<int, int> &keys)
{
    if (!openDB())
    {
      return false;
    }

    QSqlDatabase& db = getDB();
    QSqlQuery query(db);
    if (!query.exec(sqlSelect))
    {
        ScrollMessageBox::information(nullptr, "ERROR", query.lastError().text());
        return false;
    }
    else if (query.isSelect())
    {

        int keyIndex = query.record().indexOf(keyField);
        int recordKey = -1;

        while (query.isActive() && query.next())
        {
            QSqlRecord rec = query.record();
            if (keyIndex >= 0)
            {
                bool ok = false;
                recordKey = rec.value(keyIndex).toInt(&ok);
                if (keys.contains(recordKey)) {

                    // I really do not expect this, but, do it anyway.
                    records[keys[recordKey]] = rec;
                } else {
                    keys.insert(recordKey, records.size());
                    records.append(rec);
                }
            } else {
                records.append(rec);
            }
        }
    } else {
        // query.numRowsAffected();
    }
    return true;
}


bool StampDB::exportToCSV(const QDir& outputDir, const bool overwrite)
{
    QFile ddlFile(outputDir.filePath("stamps.ddl"));
    if (ddlFile.exists() && overwrite) {
        if (!ddlFile.remove()) {
            ScrollMessageBox::information(nullptr, "ERROR", QString(tr("Failed to remove file %1")).arg(ddlFile.fileName()));
        }
    }
    if (!ddlFile.exists())
    {
        QStringList ddl = getDDLForExport();
        if (!ddlFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            ScrollMessageBox::information(nullptr, "ERROR", QString(tr("Failed to open file %1 for writing.")).arg(ddlFile.fileName()));
        }
        QTextStream out(&ddlFile);
        out << ddl.join("\n");
        ddlFile.close();
    }

    QStringList tableNames = getTableNames(true);
    for (int iTable=0; iTable < tableNames.count(); ++iTable)
    {
      QFile file(outputDir.filePath(tableNames.at(iTable) + ".csv"));
        if (file.exists() && overwrite) {
            if (!file.remove()) {
                ScrollMessageBox::information(nullptr, "ERROR", QString(tr("Failed to remove file %1")).arg(file.fileName()));
            }
        }
        if (!file.exists())
        {
            CSVWriter writer;
            if (!writer.setStreamFromPath(file.fileName()))
            {
              ScrollMessageBox::information(nullptr, "ERROR", QString(tr("Write: Failed to open CSV file %1")).arg(file.fileName()));
            }
            else
            {
              //ScrollMessageBox::information(nullptr, "INFO", QString(tr("ready to read from table %1")).arg(tableNames.at(iTable)));
              GenericDataCollection* gdo = readTableName(tableNames.at(iTable));
              //ScrollMessageBox::information(nullptr, "INFO", QString(tr("read %1 records from table ")).arg(gdo->getObjectCount()));
              if (gdo == nullptr)
              {
                ScrollMessageBox::information(nullptr, "ERROR", QString(tr("Failed to load table %1 for export")).arg(tableNames.at(iTable)));
              }
              else
              {
                gdo->exportToCSV(writer);
                delete gdo;
              }
            }
        }
    }
    return true;
}

bool StampDB::loadCSV(CSVReader& reader, const QString& tableName)
{
  if (!openDB())
  {
    return false;
  }
  QString useTableName = getClosestTableName(tableName);
  if (tableName.isEmpty())
  {
    ScrollMessageBox::information(nullptr, "ERROR", QString(tr("Failed to find a table with a name close to the CSV filename %1")).arg(tableName));
    return false;
  }

  QSqlRecord record = m_db.record(useTableName);
  QStringList fieldNames;
  int i;
  for (i=0; i<record.count(); ++i)
  {
    fieldNames << record.fieldName(i);
  }

  if (fieldNames.size() == 0)
  {
    ScrollMessageBox::information(nullptr, "WARN", QString(tr("Table %1 has no fields.")).arg(useTableName));
    return false;
  }

  QList<int> csvColumnIndex = reader.getHeaderIndexByName(fieldNames);
  QString sError;
  int numberOfColumnMatches = 0;
  for (int iCol=0; iCol<csvColumnIndex.size(); ++iCol)
  {
    if (csvColumnIndex[iCol] >= 0)
    {
      ++numberOfColumnMatches;
    }
    else
    {
        if (!sError.isEmpty())
        {
            sError += "\n";
        }
        sError += QString(tr("No match found for field %1")).arg(fieldNames.at(iCol));
    }
  }

  if (!sError.isEmpty())
  {
      ScrollMessageBox::information(nullptr, "WARN", sError);
      sError.clear();
  }

  if (numberOfColumnMatches == 0)
  {
    ScrollMessageBox::information(nullptr, "WARN", QString(tr("No column names in the CSV file match column names in Table %1.")).arg(useTableName));
    return false;
  }

  QString sGetAll = QString("SELECT * FROM %1").arg(useTableName);
  QList<QSqlRecord> records;
  const QString keyField = "id";
  QHash<int, int> keys;
  int csvKeyColumn = -1;
  if (!executeQuery(sGetAll, records, keyField, keys)) {
      // TODO: how to deal with an error?
  }

  // TODO: Verify that a column is not repeated, ie, two CSV columns match to the same DB column.

  // TODO: build the prepared statement
  // Use positional rather than named
  QString q1 = QString("INSERT INTO %1 (").arg(useTableName);
  QString q2 = ") VALUES (";
  bool firstLine = true;
  for (int iCol=0; iCol<csvColumnIndex.size(); ++iCol)
  {
    // Verify that a column in the DB has a corresponding entry
    // in the CSV file
    if (0 <= csvColumnIndex[iCol])
    {
      if (firstLine)
      {
        firstLine = false;
        q2 += "?";
      }
      else
      {
        q1 += ", ";
        q2 += ", ?";
      }
      q1 += fieldNames[iCol];
      if (csvKeyColumn < 0 && fieldNames[iCol].compare(keyField, Qt::CaseInsensitive) == 0) {
          csvKeyColumn = iCol;
      }
    }
  }
  q2 += ")";

  QSqlQuery q;
  if (!q.prepare(q1 + q2))
  {
    ScrollMessageBox::information(nullptr, "ERROR", QString(tr("Prepare statement failed for (%1) error: %2")).arg(q1+q2).arg(q.lastError().text()));
    return false;
  }
  ScrollMessageBox::information(nullptr, "INFO", QString(tr("Prepared statement (%1)")).arg(q1+q2));

  bool useTransactions = m_db.driver()->hasFeature(QSqlDriver::Transactions);
  if (useTransactions && !m_db.transaction())
  {
    ScrollMessageBox::information(nullptr, "ERROR", QString(tr("Failed to begin a transaction: %1")).arg(q.lastError().text()));
    return false;
  }

  int numRowsAdded = 0;
  int numRowsSkipped = 0;
  int numErrors = 0;
  QString errorMessage;
  int iRow;
  int csvKeyValue;
  for (iRow=0; iRow < reader.countLines() || reader.readNextRecord(false); ++iRow)
  {
    csvKeyValue = -1;
    const CSVLine& readLine = reader.getLine(iRow);
    if (csvKeyColumn >= 0 && csvKeyColumn < readLine.count())
    {
        const CSVColumn& c = readLine[csvKeyColumn];
        QVariant v = c.toVariant();
        bool ok = true;
        csvKeyValue = v.toInt(&ok);
        if (!ok) {
            csvKeyValue = -1;
        }
    }

    for (int iCol=0; iCol<csvColumnIndex.size(); ++iCol)
    {
      // Verify that a column in the DB has a corresponding entry
      // in the CSV file
      if (0 <= csvColumnIndex[iCol])
      {
        // verify that the CSV file has at least
        // that many columns in the current line.
        if (csvColumnIndex[iCol] < readLine.count())
        {
          const CSVColumn& c = readLine[csvColumnIndex[iCol]];
          QVariant v = c.toVariant();
          // Insert this into the prepared statement
          q.bindValue(iCol, v);
        }
        else
        {
          // Get a null value based on the expected type for the column.
          q.bindValue(iCol, reader.getNullVariant(csvColumnIndex[iCol]));
        }
      }
      else
      {
        qDebug(qPrintable(QString("E: %1 %2").arg(iRow).arg(iCol)));
      }
    }
    if (csvKeyValue < 0 || !keys.contains(csvKeyValue))
    {
        if (q.exec())
        {
          ++numRowsAdded;
        }
        else
        {
          if (numErrors > 0)
          {
            errorMessage += "\n";
          }
          errorMessage += QString("Row %1 : %2").arg(iRow).arg(q.lastError().text());
          ++numErrors;
        }
    } else {
        // TODO: use an update instead!
        ++numRowsSkipped;
    }
  }

  QString status = "INFO";
  sError = QString(tr("Added %1 / %2 rows into table %3")).arg(numRowsAdded).arg(iRow).arg(useTableName);

  if (numRowsSkipped > 0) {
      sError += "\n";
      sError += QString(tr("Skipped %1 records.")).arg(numRowsSkipped);
      status = "WARN";
  }

  if (numErrors > 0)
  {
      sError += "\n";
      sError += QString(tr("Had %1 errors.")).arg(numErrors);
      status = "ERROR";
  }


  if (useTransactions && !m_db.commit())
  {
    ScrollMessageBox::information(nullptr, "ERROR", QString(tr("Failed to begin end the transaction after reading %1 rows: %2")).arg(iRow).arg(q.lastError().text()));
    return false;
  }
  else
  {
    ScrollMessageBox::information(nullptr, status, sError);
  }

  return true;
}


