#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "stampdb.h"
#include "scrollmessagebox.h"
#include "csvreaderdialog.h"
#include "csvreader.h"
#include "csvwriter.h"
#include "constants.h"
#include "sqldialog.h"

#include <QMessageBox>
#include <QStringList>
#include <QFileDialog>
#include <QCoreApplication>
#include <QSettings>

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
}

MainWindow::~MainWindow()
{
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

  menu = menuBar()->addMenu(tr("&Database"));
  menu->addAction(tr("&Create DB"), this, SLOT(createDB()));
  menu->addAction(tr("Create &Schema"), this, SLOT(createSchema()));
  menu->addAction(tr("&View Schema"), this, SLOT(getSchema()));
  menu->addAction(tr("&Read CSV"), this, SLOT(readCSV()));
  menu->addAction(tr("&SQL Window"), this, SLOT(openSQLWindow()));

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
      path = QFileDialog::getExistingDirectory(this, "Select DB Path", path);
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
    m_db->tableMap();
  }
}

void MainWindow::readCSV()
{
  QString defaultExtension = tr("CSV files (*.csv)");
  QSettings settings;
  QString lastReadDir = settings.value(Constants::Settings_LastCSVDirOpen).toString();
  qDebug(qPrintable(QString("Read:(%1)").arg(lastReadDir)));
  QString fileReadPath = QFileDialog::getOpenFileName(this, "Import CSV", lastReadDir, tr("Text files (*.txt);;CSV files (*.csv);;All files (*.*)"), &defaultExtension);
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
    reader.readNLines(30);
    reader.guessColumnTypes();
    CSVReaderDialog dlg(&reader);
    dlg.exec();
    // This next code, if enabled, writes the CSV file.
#if 0
    QString fileWritePath = QFileDialog::getSaveFileName(this, "Export CSV", lastReadDir, tr("Text files (*.txt);;CSV files (*.csv);;All files (*.*)"), &defaultExtension);
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
      // TODO: Error message here!
      return;
    }

    QString tableName = m_db->getClosestTableName(fileInfo.baseName());
    if (tableName.isEmpty())
    {
      // TODO: Error message here! Table Not Found
      return;
    }

  }
}

void MainWindow::openSQLWindow()
{
  if (createDBWorker())
  {
    SQLDialog sqlDialog(*m_db);
    sqlDialog.exec();
  }
}
