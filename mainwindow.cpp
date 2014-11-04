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

#include <QMessageBox>
#include <QStringList>
#include <QFileDialog>
#include <QCoreApplication>
#include <QSettings>
#include <QXmlStreamWriter>
#include <QInputDialog>

#if defined(__GNUC__)
#if __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 6)
#include "nullptr.h"
#endif
#endif

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

  menu = menuBar()->addMenu(tr("&Tools"));
  menu->addAction(tr("&Create DB"), this, SLOT(createDB()));
  menu->addAction(tr("Create &Schema"), this, SLOT(createSchema()));
  menu->addAction(tr("&View Schema"), this, SLOT(getSchema()));
  menu->addAction(tr("&Import CSV"), this, SLOT(readCSV()));
  menu->addAction(tr("&Export CSV"), this, SLOT(exportCSV()));
  menu->addAction(tr("&SQL Window"), this, SLOT(openSQLWindow()));
  menu->addAction(tr("Configure"), this, SLOT(configure()));
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
  qDebug(qPrintable(QString("File Read Path:(%1)").arg(fileReadPath)));

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
    qDebug(qPrintable(QString("Setting Path:(%1)").arg(fileInfo.absolutePath())));
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
          for (int k=0; k<readLine.count(); ++k)
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
    for (int i=0; i<tableNames.count(); ++i)
    {
        if (QFile::exists(writeDir.filePath(tableNames.at(i) + ".csv"))) {
            existingFiles.append(tableNames.at(i) + ".csv");
        }
    }
    if (QFile::exists(writeDir.filePath("stamps.ddl"))) {
        existingFiles.append("stamps.ddl");
    }
    if (existingFiles.count() > 0)
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
      // QString tableName("country");
      // QString tableName("inventory");
      QString tableName("stamplocation");
      // QString tableName("stamplocation");
      GenericDataCollection* gdo = m_db->readTableName(tableName);

      qDebug(qPrintable(QString("On return, number of rows = %1").arg(gdo->rowCount())));

      DescribeSqlTables schema = DescribeSqlTables::getStampSchema();
      GenericDataCollectionTableDialog dlg(tableName, *gdo, *m_db, schema);
      dlg.exec();
  }
}

#include <QXmlStreamReader>

void MainWindow::editTable()
{
  //GenericDataCollection gdc;
  //gdc.makeDummy();
  //TableSortFieldDialog dlg(&gdc);
  //dlg.exec();

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


      QString tableName = QInputDialog::getItem(this, tr("Choose Table"), tr("Table"), tableNames, 0, false, &ok);
      //int maxID = m_db->getMaxId(tableName);
      //ScrollMessageBox::information(this, "Max ID", QString("Max ID is %1").arg(maxID));
      if (ok && !tableName.isEmpty()) {
        //GenericDataCollection* data = m_db->readTableName(tableName);
        GenericDataCollection* data = m_db->readTableBySchema(tableName);
        if (data != nullptr)
        {
          GenericDataCollectionTableDialog dlg(tableName, *data, *m_db, schema);
          dlg.exec();
          delete data;
        }
      }
  }
}
