#include "stampdb.h"
#include "scrollmessagebox.h"
#include "csvreader.h"

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
                             " web varchar(100))";

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
    //s += getSchema(m_db.record(tableName));
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


//"CREATE TABLE bookvalues( id INTEGER PRIMARY KEY AUTOINCREMENT, catalogid INTEGER, sourceid INTEGER, valuetypeid INTEGER, bookvalue FLOAT)

const QMap<QString, QSqlRecord>& StampDB::tableMap()
{
  if (!m_tableMap) {

/**
    QMap<QString, QVariant::Type> sqlTypeToVariantType;
    QStringList sqlTypeList;
    sqlTypeList << "NATIONAL CHARACTER VARYING(n)";
    sqlTypeList << "NVARCHAR(n)";
    sqlTypeList << "NATIONAL CHARACTER(n)";
    sqlTypeList << "NCHAR(n)";
    sqlTypeList << "CHARACTER VARYING(n)";
    sqlTypeList << "VARCHAR(n)";
    sqlTypeList << "CHARACTER(n)";
    sqlTypeList << "CHAR(n)";
    sqlTypeList << "BIT VARYING(n)";
    sqlTypeList << "BIT(n)";
    sqlTypeList << "TIMESTAMP WITH TIME ZONE";
    sqlTypeList << "TIMESTAMPTZ";
    sqlTypeList << "TIMESTAMP";
    sqlTypeList << "TIME WITH TIME ZONE";
    sqlTypeList << "TIMETZ";
    sqlTypeList << "TIME";
    sqlTypeList << "DATE";
    sqlTypeList << "INTEGER";
    sqlTypeList << "SMALLINT";
    sqlTypeList << "FLOAT";
    sqlTypeList << "REAL";
    sqlTypeList << "DOUBLE PRECISION";
    sqlTypeList << "DOUBLE";
    sqlTypeList << "NUMERIC(precision, scale)";
    sqlTypeList << "DECIMAL(precision, scale)";
**/

    QRegExp fieldLengthRegExp("\\(\\d+\\)");
    QRegExp recordSplitRegExp("\\s*,\\s*");
    QRegExp fieldAndTypeRegExp("^\\s*(\\w+)\\s+(\\w+)");

    QRegExp primaryKeyRegExp("PRIMARY\\S+KEY\\s*(ASC)?(DESC)?");
    QRegExp collateRegExp("COLLATE\\s+([a-z]+)");
    QRegExp notNullRegExp("NOT\\s+NULL");
    primaryKeyRegExp.setCaseSensitivity(Qt::CaseInsensitive);
    collateRegExp.setCaseSensitivity(Qt::CaseInsensitive);
    notNullRegExp.setCaseSensitivity(Qt::CaseInsensitive);
    //QRegExp defaultRegExp("DEFAULT\\s+(signed-number|literal-value|\\(expr\\))");

    m_tableMap = new QMap<QString, QSqlRecord>();

    for (int i=0; i<m_desiredSchemaDDLList->size(); ++i) {
      const QString& ddl = m_desiredSchemaDDLList->at(i);
      if (m_outerDDLRegExp->indexIn(ddl) == -1) {

        QMessageBox msgBox;
        msgBox.setText(tr("Schema Error: DDL not recognized"));
        msgBox.setInformativeText(ddl);
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return *m_tableMap;

      } else {
        QString tableName = m_outerDDLRegExp->cap(1);
        QString bigFieldString = m_outerDDLRegExp->cap(2);
        QStringList fieldStrings = bigFieldString.split(recordSplitRegExp, QString::SkipEmptyParts);
        QSqlRecord currentRecord;
        for (int currentFieldIndex=0; currentFieldIndex<fieldStrings.size(); ++currentFieldIndex)
        {
          // Get the current field and then remove any reference to Primary Key.
          QString fieldString = fieldStrings.at(currentFieldIndex);
          fieldString = fieldString.replace(primaryKeyRegExp, "");
          bool isAuto = fieldString.contains("AUTOINCREMENT", Qt::CaseInsensitive);
          if (isAuto) {
            fieldString = fieldString.replace("AUTOINCREMENT", "", Qt::CaseInsensitive);
          }
          int fieldLength = -1;
          bool containsFieldLength = fieldLengthRegExp.indexIn(fieldString) != -1;
          if (containsFieldLength) {
            fieldLength = fieldLengthRegExp.cap(1).toInt();
            fieldString = fieldString.replace(fieldLengthRegExp, "");
          }
          bool notNull = fieldString.contains(notNullRegExp);
          if (notNull) {
            fieldString = fieldString.replace(notNullRegExp, "");
          }
          if (fieldString.contains(notNullRegExp)) {
            fieldString = fieldString.replace(collateRegExp, "");
          }

          if (fieldAndTypeRegExp.indexIn(fieldString) < 0) {
            // Error message
          } else {
            QSqlField field(fieldAndTypeRegExp.cap(1));
            QString fieldType = fieldAndTypeRegExp.cap(2);
            field.setAutoValue(isAuto);
            field.setRequired(notNull);

            // set length for VarChar
            // set Precision if specified on numbers.
          }


        }
        //ScrollMessageBox::information(0, "Schema", QString("Table %1\n\n%2").arg(tableName, bigFieldString));
        //return *m_tableMap;
        /**
                ret = query.exec(ddl);
                if (!ret) {
                    QMessageBox msgBox;
                    msgBox.setText(tr("Schema Error creating table '%1'").arg(tableNameRegExp.cap(1)));
                    msgBox.setInformativeText(tr("Error:%1\nDDL:%2").arg(m_db.lastError().text(), ddl));
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    msgBox.setDefaultButton(QMessageBox::Ok);
                    msgBox.exec();
                    query.exec(QString("DROP TABLE %1").arg(tableNameRegExp.cap(1)));
                }
                **/
      }
    }
  }
  return *m_tableMap;
}


QString StampDB::getClosestTableName(const QString& aName)
{
  if (openDB())
  {

    QStringList tables = m_db.tables(QSql::Tables);
    // ??
    ScrollMessageBox::information(0, "Tables", tables.join("\n"));
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
  QString tmp;
  int i;
  for (i=0; i<record.count(); ++i)
  {
    fieldNames << record.fieldName(i);
    tmp += record.fieldName(i) + "\n";
  }

  // TODO ?? Remove
  ScrollMessageBox::information(nullptr, "FIELDS", tmp);

  if (fieldNames.size() == 0)
  {
    ScrollMessageBox::information(nullptr, "WARN", QString(tr("Table %1 has no fields.")).arg(useTableName));
    return false;
  }

  tmp = "";

  QList<int> csvColumnIndex = reader.getHeaderIndexByName(fieldNames);
  int numberOfColumnMatches = 0;
  for (int iCol=0; iCol<csvColumnIndex.size(); ++iCol)
  {
    tmp += QString("(%1) => %2 \n").arg(fieldNames[iCol]).arg(csvColumnIndex[iCol]);
    if (csvColumnIndex[iCol] >= 0)
    {
      ++numberOfColumnMatches;
    }
  }
  // TODO ?? Remove
  ScrollMessageBox::information(nullptr, "FIELDS", tmp);

  if (numberOfColumnMatches == 0)
  {
    ScrollMessageBox::information(nullptr, "WARN", QString(tr("No column names in the CSV file match column names in Table %1.")).arg(useTableName));
    return false;
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
    }
  }
  q2 += ")";

  QSqlQuery q;
  if (!q.prepare(q1 + q2))
  {
    ScrollMessageBox::information(nullptr, "ERROR", QString(tr("Prepare statement failed for (%1).")).arg(q1+q2));
    return false;
  }

  bool useTransactions = m_db.driver()->hasFeature(QSqlDriver::Transactions);
  if (useTransactions && !m_db.transaction())
  {
    ScrollMessageBox::information(nullptr, "ERROR", tr("Failed to begin a transaction."));
    return false;
  }
  int iRow;
  for (iRow=0; iRow < reader.countLines() || reader.readNextRecord(false); ++iRow)
  {
    const CSVLine& readLine = reader.getLine(iRow);
    qDebug("Adding line");

    for (int iCol=0; i<csvColumnIndex.size(); ++iCol)
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
          // TODO
          // Insert this into the prepared statement
        }
        else
        {
          // TODO
          // Insert a null value!
        }
      }
    }
  }

  if (useTransactions && !m_db.commit())
  {
    ScrollMessageBox::information(nullptr, "ERROR", QString(tr("Failed to begin end the transaction after reading %1 rows.")).arg(iRow));
    return false;
  }
  else
  {
    ScrollMessageBox::information(nullptr, "INFO", QString(tr("Added %1 rows to table %2.")).arg(iRow).arg(useTableName));
  }

  return true;
}


