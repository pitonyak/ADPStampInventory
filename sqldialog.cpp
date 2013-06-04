#include "sqldialog.h"
#include "constants.h"
#include "stampdb.h"
#include "scrollmessagebox.h"

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

SQLDialog::SQLDialog(StampDB &db, QWidget *parent) :
  QDialog(parent), m_db(db), m_textEdit(nullptr), m_tableWidget(nullptr), m_statusBar(nullptr)
{
  QSettings settings;
  QVBoxLayout *vBox = new QVBoxLayout();
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal);

  // Sadly, new lines are removed when the string is restored.... very sad!
  QStringList sqlList = settings.value(Constants::Settings_LastSQLDialogString, "").toString().split("\n");
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
  restoreGeometry(settings.value(Constants::Settings_SQLDialogGeometry).toByteArray());

  m_statusBar->showMessage(tr("Ready!"));

}


SQLDialog::~SQLDialog()
{
  QSettings settings;
  settings.setValue(Constants::Settings_SQLDialogGeometry, saveGeometry());
  settings.setValue(Constants::Settings_LastSQLDialogString, getSqlText());
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
    ScrollMessageBox::information(this, "Complex Selection", "??");
  }
  else if (cursor.hasSelection())
  {
    // TODO: Handle selected text
    //ScrollMessageBox::information(this, "Simple Selection", cursor.selectedText());
    executeSql(cursor.selectedText());
  }
  else
  {
    // TODO: Get current line (paragraph) and process
    //ScrollMessageBox::information(this, "No Selection", cursor.block().text());
    executeSql(cursor.block().text());
  }
}


//select * from dealer
//select * from country

//INSERT INTO  'country' ('id', 'name', 'a3') VALUES (0, 'Unknown', '')
//INSERT INTO  'country' ('id', 'name', 'a3') VALUES (1, 'Canada', 'CAN')
//insert into dealer (id, updated, name, contact, address1, address2, address3, address4, phone, fax, comment, email, web) values (2,'2005-04-01T09:33:00','David Alex','David Alex','P.O. BOX 2176','Glenview, IL 60025','','','708-998-8147','','Beautiful stuff','','')

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
    m_statusBar->showMessage("query worked");
    if (query.isSelect() && m_tableWidget != nullptr)
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
    }
    // Handle things here!
  }
}

