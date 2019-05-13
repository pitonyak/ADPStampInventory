#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stampdb.h"
#include "scrollmessagebox.h"
#include "csvreaderdialog.h"
#include "csvreader.h"
#include "csvwriter.h"
#include "constants.h"
#include "sqldialog.h"
#include "genericdatacollectiontabledialog.h"
#include "describesqltables.h"
#include "genericdatacollections.h"

#include <QDebug>
#include <QMessageBox>
#include <QSqlRecord>
#include <QSqlField>
#include <QStringList>
#include <QFileDialog>
#include <QCoreApplication>
#include <QSettings>
#include <QXmlStreamWriter>
#include <QInputDialog>

#include <QSqlQuery>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow),
  m_db(nullptr)
{
  initializeSettings();
  ui->setupUi(this);
  setupMenuBar();
  setWindowTitle(tr("Stamp Inventory"));
  QSettings settings;
  restoreGeometry(settings.value(Constants::Settings_MainWindowGeometry).toByteArray());
}

MainWindow::~MainWindow()
{
  QSettings settings;
  settings.setValue(Constants::Settings_MainWindowGeometry, saveGeometry());
  delete ui;
}

void MainWindow::setupMenuBar()
{
  // Use the menu bar from the main window, we do not need to create one.
  QMenu *menu = menuBar()->addMenu(tr("&File"));
  //QAction *action = nullptr;

  //QAction *action = menu->addAction(tr("Save layout..."));
  //connect(action, SIGNAL(triggered()), this, SLOT(saveLayout()));
  //
  //action = menu->addAction(tr("Load layout..."));
  //connect(action, SIGNAL(triggered()), this, SLOT(loadLayout()));
  //
  //action = menu->addAction(tr("Switch layout direction"));
  //connect(action, SIGNAL(triggered()), this, SLOT(switchLayoutDirection()));
  //
  //menu->addSeparator();

  menu->addAction(tr("&Quit"), this, SLOT(close()));

  menu = menuBar()->addMenu(tr("&Edit"));
  menu->addAction(tr("&Test"), this, SLOT(testing()));

  menu = menuBar()->addMenu(tr("&Tools"));
  menu->addAction(tr("&Create DB"), this, SLOT(createDB()));
  menu->addAction(tr("Create &Schema"), this, SLOT(createSchema()));
  menu->addAction(tr("&View Schema"), this, SLOT(getSchema()));
  menu->addAction(tr("&Import CSV"), this, SLOT(readCSV()));
  menu->addAction(tr("&Export CSV"), this, SLOT(exportCSV()));
  menu->addAction(tr("&SQL Window"), this, SLOT(openSQLWindow()));
  menu->addAction(tr("Configure"), this, SLOT(configure()));
  menu->addAction(tr("Add Missing Values"), this, SLOT(addMissingBookValues()));
  menu->addAction(tr("Edit Table"), this, SLOT(editTable()));

  menu = menuBar()->addMenu(tr("&Help"));


  /**
    mainWindowMenu = menuBar()->addMenu(tr("Main window"));

    action = mainWindowMenu->addAction(tr("Animated docks"));
    action->setCheckable(true);
    action->setChecked(dockOptions() & AnimatedDocks);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(setDockOptions()));

    action = mainWindowMenu->addAction(tr("Allow nested docks"));
    action->setCheckable(true);
    action->setChecked(dockOptions() & AllowNestedDocks);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(setDockOptions()));

    action = mainWindowMenu->addAction(tr("Allow tabbed docks"));
    action->setCheckable(true);
    action->setChecked(dockOptions() & AllowTabbedDocks);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(setDockOptions()));

    action = mainWindowMenu->addAction(tr("Force tabbed docks"));
    action->setCheckable(true);
    action->setChecked(dockOptions() & ForceTabbedDocks);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(setDockOptions()));

    action = mainWindowMenu->addAction(tr("Vertical tabs"));
    action->setCheckable(true);
    action->setChecked(dockOptions() & VerticalTabs);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(setDockOptions()));

    QMenu *toolBarMenu = menuBar()->addMenu(tr("Tool bars"));
    for (int i = 0; i < toolBars.count(); ++i)
        toolBarMenu->addMenu(toolBars.at(i)->menu);

    dockWidgetMenu = menuBar()->addMenu(tr("&Dock Widgets"));
    **/
}

void MainWindow::createDB()
{
  createDBWorker();
}

bool MainWindow::createDBWorker()
{
  if (m_db == nullptr) {
    QSettings settings;
    QString dbName = settings.value(Constants::Settings_DBName, "").toString();
    if (dbName.isEmpty()) {
      dbName = "stamps.db.sqlite";
      settings.setValue(Constants::Settings_DBName, dbName);
    }
    QString path = settings.value(Constants::Settings_DBPath, "").toString();
    if (path.isEmpty()) {
      path = QFileDialog::getExistingDirectory(nullptr, "Select DB Path", path);
      if (path.isEmpty()) {
        return false;
      }
      settings.setValue(Constants::Settings_DBPath, path);
    } else {

    }
    m_db = new StampDB(this);
    m_db->pathToDB(QDir::cleanPath(path + QDir::separator() + dbName));
  }
  if (!m_db->openDB()) {
    QMessageBox msgBox;
    msgBox.setText(tr("Error creating database '%1'\n Would you like to set select a new path and try again?").arg(m_db->pathToDB()));
    msgBox.setInformativeText(m_db->lastError().text());
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int pressed_btn = msgBox.exec();

    delete m_db;
    m_db = nullptr;

    // There was an error, so give the user the chance to select the location.
    if (pressed_btn == QMessageBox::Yes) {
      QSettings settings;
      QString path = settings.value(Constants::Settings_DBPath, "").toString();
      if (!path.isEmpty()) {
        settings.setValue(Constants::Settings_DBPath, "");
      }
      bool rc = createDBWorker();
      if (!rc) {
        // Failure, so simply restore the initial value.
        settings.setValue(Constants::Settings_DBPath, path);
      }
      return rc;
    }

    return false;
  }
  return true;
}

void MainWindow::createSchema()
{
  if (!createDBWorker()) {
    return;
  }
  m_db->createSchema();
}

void MainWindow::initializeSettings()
{
}

void MainWindow::getSchema() {
  if (createDBWorker()) {
    QString s = m_db->getSchema();
    ScrollMessageBox::information(this, "Schema", s);
  }
}

void MainWindow::readCSV()
{
  QString defaultExtension = tr("CSV files (*.csv)");
  QSettings settings;
  QString lastReadDir = settings.value(Constants::Settings_LastCSVDirOpen).toString();
  QString fileReadPath = QFileDialog::getOpenFileName(nullptr, "Import CSV", lastReadDir, tr("Text files (*.txt);;CSV files (*.csv);;All files (*.*)"), &defaultExtension);
  if (fileReadPath.isEmpty()) {
    // Nothing to do
    return;
  }
  qDebug() << tr("File Read Path:(") << fileReadPath << ")";

  QFileInfo fileInfo(fileReadPath);
  if (!fileInfo.exists()) {
    QMessageBox msgBox;
    msgBox.setText(tr("File does not exist: %1").arg(fileReadPath));
    //msgBox.setInformativeText();
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
    return;
  }

  // The file exists, so it cannot be empty.
  if (lastReadDir.compare(fileInfo.absolutePath()) != 0)
  {
    qDebug() << tr("Setting Path:(") << fileInfo.absolutePath() << ")";
    settings.setValue(Constants::Settings_LastCSVDirOpen, fileInfo.absolutePath());
  }
  CSVReader reader(TypeMapper::PreferSigned | TypeMapper::PreferInt);
  if (!reader.setStreamFromPath(fileReadPath))
  {
    ScrollMessageBox::information(this, "ERROR", QString(tr("Read: Failed to open CSV file %1")).arg(fileReadPath));
  }
  else if (!reader.readHeader())
  {
    ScrollMessageBox::information(this, "ERROR", QString(tr("Failed to read CSV header from %1")).arg(fileReadPath));
  }
  else
  {
    reader.readNLines(200);
    reader.guessColumnTypes();
    CSVReaderDialog dlg(&reader, this);
    if (dlg.exec() == QDialog::Rejected)
    {
      return;
    }
    // This next code, if enabled, writes the CSV file.
#if 0
    QString fileWritePath = QFileDialog::getSaveFileName(nullptr, "Export CSV", lastReadDir, tr("Text files (*.txt);;CSV files (*.csv);;All files (*.*)"), &defaultExtension);
    if (!fileWritePath.isEmpty())
    {
      CSVWriter writer;
      if (!writer.setStreamFromPath(fileWritePath))
      {
        ScrollMessageBox::information(this, "ERROR", QString(tr("Write: Failed to open CSV file %1")).arg(fileWritePath));
      }
      else
      {
        writer.setHeader(reader.getHeader());
        writer.writeHeader();
        int maxLines = 100;
        for (int i=0; i<maxLines && reader.readNextRecord(false); ++i)
        {
          const CSVLine& readLine = reader.getLine(i);
          CSVLine newLine;
          for (int k=0; k<readLine.size(); ++k)
          {
            const CSVColumn& c = readLine[k];
            QVariant v = c.toVariant();
            newLine.append(CSVColumn(v.toString(), c.isQualified(), c.getType()));
          }
          //writer.write(reader.getLine(i));
          writer.write(newLine);
        }
        ScrollMessageBox::information(this, "INFO", reader.toString(false));
      }
    }
#endif

    //??ScrollMessageBox::information(0, "Schema", QString("Table %1\n\n%2").arg(tableName, bigFieldString));
    if (!createDBWorker()) {
      // An error message was probably already displayed
      ScrollMessageBox::information(this, "ERROR", tr("Failed to create or access the database."));
      return;
    }

    // The previous line worked, so this should not be null.
    if (m_db != nullptr && !m_db->loadCSV(reader, fileInfo.baseName()))
    {
      ScrollMessageBox::information(this, "ERROR", tr("Failed to load the CSV file into the database."));
    }
  }
}

void MainWindow::exportCSV()
{
    if (!createDBWorker()) {
        return;
    }

    QSettings settings;
    QString lastWritePath = settings.value(Constants::Settings_LastCSVDirWrite).toString();
    if (lastWritePath.isEmpty()) {
        lastWritePath = settings.value(Constants::Settings_LastCSVDirOpen).toString();
    }

    QString fileWritePath = QFileDialog::getExistingDirectory(nullptr, tr("Export To CSV"), lastWritePath);
    if (fileWritePath.isEmpty()) {
        return;
    }
    QDir writeDir = fileWritePath;

    QStringList tableNames = m_db->getTableNames();
    QStringList existingFiles;
    for (int i=0; i<tableNames.size(); ++i)
    {
        if (QFile::exists(writeDir.filePath(tableNames.at(i) + ".csv"))) {
            existingFiles.append(tableNames.at(i) + ".csv");
        }
    }
    if (QFile::exists(writeDir.filePath("stamps.ddl"))) {
        existingFiles.append("stamps.ddl");
    }
    if (existingFiles.size() > 0)
    {
        if (ScrollMessageBox::question(this, "WARNING", QString(tr("OVerwrite the following files:\n%1")).arg(existingFiles.join("\n"))) != QDialogButtonBox::Yes)
        {
            return;
        }
    }
    settings.setValue(Constants::Settings_LastCSVDirWrite, writeDir.canonicalPath());

    m_db->exportToCSV(writeDir.canonicalPath(), false);
}


void MainWindow::openSQLWindow()
{
  if (createDBWorker())
  {
    SQLDialog sqlDialog(*m_db);
    sqlDialog.exec();
  }
}

#include "genericdatacollection.h"
#include "tablesortfielddialog.h"

void MainWindow::configure()
{
  if (createDBWorker()) {

      GenericDataCollection gdc;
      gdc.makeDummy();
      TableSortFieldDialog dlg(&gdc);
      dlg.exec();

      // QString tableName("country");
      // QString tableName("inventory");
      QString tableName("stamplocation");
      // QString tableName("stamplocation");
      /**
      GenericDataCollection* gdo = m_db->readTableName(tableName);

      qDebug(qPrintable(QString("On return, number of rows = %1").arg(gdo->rowCount())));

      DescribeSqlTables schema = DescribeSqlTables::getStampSchema();

      GenericDataCollection* data = m_db->readTableBySchema("catalog");

      QSqlQuery query(m_db->getDB());
      QString s = QString("UPDATE %1 SET %2=:%2, updated=:updated WHERE %3=:id").arg("catalog").arg("releasedate").arg("id");
      query.prepare(s);

      for (int i=0; i<data->rowCount(); ++i)
      {
        GenericDataObject* row = data->getObjectByRow (i);
        QString scott = row->getString("scott");
        int id = row->getInt("id");
        int countryId = row->getInt("countryid");
        QDate rd = row->getDate("releasedate");
        bool makeChange = false;
        QString leadingNum = "";
        for (int idx = 0; idx < scott.length() && scott.at(idx).isDigit(); ++idx)
        {
          leadingNum.append(scott.at(idx));
        }
        int leadingInt = leadingNum.isEmpty() ? 0 : leadingNum.toInt();

        makeChange = countryId == 5 && leadingInt < 2000 && leadingInt > 299 && (row->getDate("releasedate").year() >= 2000);
        makeChange = (row->getDate("releasedate").year() >= 2000);

        if (makeChange)
        {
          qDebug(qPrintable(QString("Changing %4 (%1) from %2 to %3").arg(id).arg(rd.toString("MM/dd/yyyy")).arg(rd.addYears(-100).toString("MM/dd/yyyy")).arg(scott)));
          makeChange = false;
        }
        if (makeChange)
        {
          QDate d = row->getDate("releasedate").addYears(-100);
          query.bindValue(":id", row->getInt("id"));
          QDateTime now = QDateTime::currentDateTime();
          query.bindValue(":updated", now);
          query.bindValue(":releasedate", d);
          if (!query.exec())
          {
            qDebug(qPrintable(QString("Failed to update row for id ").arg(row->getInt("id"))));
            return;
          }
        }

      }
**/
      //GenericDataCollectionTableDialog dlg(tableName, *gdo, *m_db, schema);
      //dlg.exec();
  }
}


void MainWindow::testing() {
  if (createDBWorker()) {
    QString sql = "select id, (\"id\" || ' - ' || \"description\") As x from valuesource where ID > 0 order by valuesource.year DESC ";
    QList<QSqlRecord> records;

    QString keyField;
    QHash<int, int> keys;

    if (!m_db->executeQuery(sql, records, keyField, keys)) {
      QMessageBox::warning(this, tr("ERROR"), tr("Failed to execute SQL."));
      return;
    } else if (records.size() < 1) {
      QMessageBox::warning(this, tr("No Records Found"), tr("No catalog entries found without values."));
      return;
    }

    QStringList items;
    for (QSqlRecord r : records) {
      if (!r.isNull(1)) {
        items << r.field(1).value().toString();
      }
    }
    bool ok;
    QString item = QInputDialog::getItem(this, tr("Select Catalog To Use"),
                                             tr("Catalog:"), items, 0, false, &ok);
    if (ok && !item.isEmpty())
      for (QSqlRecord r : records) {
        if (!r.isNull(1) && r.field(1).value().toString() == item) {
          qDebug() << r.field(0).value().toInt();
        }
      }
  }
}

void MainWindow::addMissingBookValues()
{
  if (createDBWorker()) {
    // Get stamps with no entry in "book values"
    QString sql = "select catalog.id from catalog left outer join bookvalues on catalog.id = bookvalues.catalogid where bookvalues.catalogid IS NULL";
    QList<QSqlRecord> records;

    // We only want the IDs, so, these fields will be ignored.
    QString keyField;
    QHash<int, int> keys;

    if (!m_db->executeQuery(sql, records, keyField, keys)) {
      QMessageBox::warning(this, tr("ERROR"), tr("Failed to execute SQL to find missing entries."));
      return;
    } else if (records.size() < 1) {
      QMessageBox::warning(this, tr("No Records Found"), tr("No catalog entries found without values."));
      return;
    }
    if (ScrollMessageBox::question(this, "Question", QString(tr("Add %1 stamps to the book values table?")).arg(records.size())) != QDialogButtonBox::Yes)
    {
        return;
    }

    // Set the sourceID based on the latest year that we have
    // select id from valuesource order by valuesource.year DESC limit 1
    // TODO; Create a dialog to prompt for the value source to use.
    int sourceId = m_db->selectValueSourceId(this);

    QList<QSqlRecord> ignoredRecords;
    int added = 0;
    int failed = 0;

    // Do the work!
    for (int i=0; i<records.size() && failed == 0; ++i) {
      int catalogId = records[i].value("id").toInt();
      QString q1 = QString("INSERT INTO bookvalues (Catalogid, sourceid, valuetypeid, bookvalue) VALUES (%1, %2, 1, 0)").arg(catalogId).arg(sourceId);
      QString q2 = QString("INSERT INTO bookvalues (Catalogid, sourceid, valuetypeid, bookvalue) VALUES (%1, %2, 2, 0)").arg(catalogId).arg(sourceId);
      if (!m_db->executeQuery(q1, ignoredRecords, keyField, keys)) {
        ++failed;
        QMessageBox::warning(this, tr("ERROR"), q1);
      } else {
        ++added;
        if (!m_db->executeQuery(q2, ignoredRecords, keyField, keys)) {
          ++failed;
          QMessageBox::warning(this, tr("ERROR"), q2);
        } else {
          ++added;
        }
      }
    }
    ScrollMessageBox::question(this, "Done", QString(tr("Added %1 entries to the book values table. %2 failed.")).arg(added).arg(failed));
  }
}

#include <QXmlStreamReader>

void MainWindow::editTable()
{

  DescribeSqlTables schema = DescribeSqlTables::getStampSchema();
  /**
  QString s;
  QXmlStreamWriter writer(&s);
  writer.setAutoFormatting(true);
  writer.writeStartDocument();
  schema.writeXml(writer);
  writer.writeEndDocument();

  qDebug(qPrintable(s));

  QXmlStreamReader reader(s);
  DescribeSqlTables schema2 = DescribeSqlTables::readXml(reader);

  QString s2;
  QXmlStreamWriter writer2(&s2);
  writer2.setAutoFormatting(true);
  writer2.writeStartDocument();
  schema2.writeXml(writer2);
  writer2.writeEndDocument();

  if (s.compare(s2) != 0) {
      qDebug("Failure, s != s2");
      qDebug(qPrintable(s2));
  } else {
      qDebug("Yeah!, s == s2");
      qDebug(qPrintable(s2));
  }
  **/

  if (createDBWorker()) {
      QStringList tableNames = schema.getTableNames();
      tableNames.sort(Qt::CaseInsensitive);
      bool ok;
      //QString s = schema.getDDL(true).join("\n\n");
      //ScrollMessageBox::information(this, "Schema", s);

      QSettings settings;
      int focusedNameIndex = 0;
      if (tableNames.size() > 0) {
        QString startName = settings.value(Constants::Settings_GenericDataCollectionLastEditedTable, tableNames.at(0)).toString();
        focusedNameIndex = tableNames.indexOf(startName);
        if (focusedNameIndex < 0) {
          focusedNameIndex = 0;
        }
      }

      QString tableName = QInputDialog::getItem(this, tr("Choose Table"), tr("Table"), tableNames, focusedNameIndex, false, &ok);

      //int maxID = m_db->getMaxId(tableName);
      //ScrollMessageBox::information(this, "Max ID", QString("Max ID is %1").arg(maxID));
      if (ok && !tableName.isEmpty()) {
        settings.setValue(Constants::Settings_GenericDataCollectionLastEditedTable, tableName);
        //GenericDataCollection* data = m_db->readTableName(tableName);
        //GenericDataCollection* data = m_db->readTableBySchema(tableName);
        GenericDataCollections* data = m_db->readTableWithLinks(tableName);
        Q_ASSERT_X(data != nullptr, "MainWindow::editTable", "Returned data is null");

        //??ScrollMessageBox::information(this, "Supported Tables", data->getNames().join("\n"));

        Q_ASSERT_X(data->contains(tableName), "MainWindow::editTable", "Table not returned");
        if (data != nullptr)
        {
          int defaultSourceId = -1;
          if (tableName.compare("bookvalues", Qt::CaseInsensitive) == 0) {
            defaultSourceId = m_db->selectValueSourceId(this);
          }
          //GenericDataCollectionTableDialog dlg(tableName, *data, *m_db, schema);
          GenericDataCollectionTableDialog dlg(tableName, *data->getTable(tableName), *m_db, schema, data, defaultSourceId);
          dlg.exec();
          delete data;
        }
      }
  }
}
