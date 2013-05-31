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
    ScrollMessageBox::information(this, "Simple Selection", cursor.selectedText());
  }
  else
  {
    // TODO: Get current line (paragraph) and process
    ScrollMessageBox::information(this, "No Selection", cursor.block().text());
  }
}
