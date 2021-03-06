#include "sqldialog.h"
#include "constants.h"
#include "stampdb.h"
#include "scrollmessagebox.h"
#include "globals.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QSettings>
#include <QStatusBar>
#include <QTableWidget>
#include <QTextCursor>
#include <QTextBlock>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QScopedPointer>

SQLDialog::SQLDialog(StampDB &db, QWidget *parent) :
  QDialog(parent), m_db(db), m_textEdit(nullptr), m_tableWidget(nullptr), m_statusBar(nullptr)
{

  QScopedPointer<QSettings> pSettings(getQSettings());

  QVBoxLayout *vBox = new QVBoxLayout();
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal);

  // Sadly, new lines are removed when the string is restored.... very sad!
  QStringList sqlList = pSettings->value(Constants::Settings_LastSQLDialogString, "").toString().split("\n");
  m_textEdit = new QPlainTextEdit();

  for (int i=0; i<sqlList.size(); ++i)
  {
    if (i>0)
    {
      m_textEdit->appendPlainText(sqlList[i]);
    }
    else
    {
      m_textEdit->setPlainText(sqlList[i]);
    }
  }

  vBox->addWidget(m_textEdit);

  QPushButton* sqlExecuteButton = new QPushButton(tr("&Execute SQL"));
  buttonBox->addButton(sqlExecuteButton, QDialogButtonBox::ActionRole);

  connect(sqlExecuteButton, SIGNAL(clicked()), this, SLOT(sqlButtonPressed()));
  //connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(closeButtonPressed()));
  vBox->addWidget(buttonBox);

  m_tableWidget = new QTableWidget(1, 1);
  vBox->addWidget(m_tableWidget);

  m_statusBar = new QStatusBar();
  vBox->addWidget(m_statusBar);

  setLayout(vBox);
  restoreGeometry(pSettings->value(Constants::Settings_SQLDialogGeometry).toByteArray());

  m_statusBar->showMessage(tr("Ready!"));
}


SQLDialog::~SQLDialog()
{
  QScopedPointer<QSettings> pSettings(getQSettings());
  pSettings->setValue(Constants::Settings_SQLDialogGeometry, saveGeometry());
  pSettings->setValue(Constants::Settings_LastSQLDialogString, getSqlText());
}


QString SQLDialog::getSqlText() const
{
  return (m_textEdit != nullptr) ? m_textEdit->toPlainText() : "";
}

void SQLDialog::closeButtonPressed()
{
  accept();
}

void SQLDialog::sqlButtonPressed()
{
  QTextCursor cursor = m_textEdit->textCursor();
  if (cursor.hasComplexSelection())
  {
    // TODO: Error? Iterate over selections
    //ScrollMessageBox::information(this, "Simple Selection", cursor.selectedText());
    ScrollMessageBox::information(this, "Complex Selection", "??");
  }
  else if (cursor.hasSelection())
  {
    executeSql(cursor.selectedText());
  }
  else
  {
    executeSql(cursor.block().text());
  }
}

void SQLDialog::executeSql(const QString& sqlString)
{
  QSqlDatabase& db = m_db.getDB();
  QSqlQuery query(db);
  if (!query.exec(sqlString))
  {
    m_statusBar->showMessage(query.lastError().text());
  }
  else
  {
    if (query.isSelect())
    {
      if (m_tableWidget == nullptr)
      {
        m_statusBar->showMessage(tr("Select not displayed because table widget is null"));
      }
      else
      {
        QSqlRecord rec = query.record();

        int numCols = rec.count();
        int numRows = query.size() + 1;

        m_tableWidget->clear();
        m_tableWidget->setColumnCount(numCols);
        m_tableWidget->setRowCount(numRows);

        QStringList fieldNames;
        int i;
        for (i=0; i<rec.count(); ++i)
        {
          fieldNames << rec.fieldName(i);
        }
        m_tableWidget->setHorizontalHeaderLabels(fieldNames);

        int row=0;
        while (query.isActive() && query.next())
        {
          for (int col=0; col<numCols; ++col)
          {
            QTableWidgetItem *newItem = new  QTableWidgetItem(query.value(col).toString());
            if (numRows <= row)
            {
              numRows = row + 1;
              m_tableWidget->setRowCount(numRows);
            }
            m_tableWidget->setItem(row, col, newItem);
          }
          ++row;
          //ScrollMessageBox::information(this, "Got a row!", QString("Have row %1 and size is %2").arg(row).arg(query.size()));
        }
        m_tableWidget->resizeColumnsToContents();
        m_statusBar->showMessage(QString(tr("Read %1 records")).arg(row));
      }
    }
    else
    {
      int numAffected = query.numRowsAffected();
      if (numAffected >= 0)
      {
        m_statusBar->showMessage(QString(tr("Successful query with %1 rows arrected")).arg(numAffected));
      }
      else
      {
        m_statusBar->showMessage(QString(tr("Successful query!")));
      }
    }
    // Handle things here!
  }
}

