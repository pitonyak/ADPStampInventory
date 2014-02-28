#include "tablesortfielddialog.h"
#include "constants.h"
#include "linkbackfilterdelegate.h"
#include "checkboxonlydelegate.h"

#include <QSettings>
#include <QTableView>
#include <QLineEdit>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>

TableSortFieldDialog::TableSortFieldDialog(const GenericDataCollection *dataCollection, QWidget *parent) :
  QDialog(parent), m_dataCollection(dataCollection), m_tableView(nullptr), m_configFilePath(nullptr), m_tableModel(nullptr)
{
}

TableSortFieldDialog::~TableSortFieldDialog()
{
  QSettings settings;
  settings.setValue(Constants::Settings_SortFieldDlgGeometry, saveGeometry());

  // TODO: Save column widths as I do in logconfigdialog.cpp

}

void TableSortFieldDialog::setConfigFilePath(const QString& path)
{
  if (m_configFilePath != nullptr)
  {
    m_configFilePath->setText(path);
  }
}

QString TableSortFieldDialog::getConfigFilePath() const
{
  return (m_configFilePath != nullptr) ? m_configFilePath->text() : "";
}

bool TableSortFieldDialog::isRowSelected() const
{
  return m_tableModel != nullptr && m_tableView != nullptr && m_tableModel->count() > 0 && m_tableView->currentIndex().row() >= 0;
}

int TableSortFieldDialog::getSelectedRow() const
{
  return m_tableView->currentIndex().row();
}

void TableSortFieldDialog::delSelectedRow()
{
  if (isRowSelected())
  {
    m_tableModel->removeRow(getSelectedRow());
  }
}

void TableSortFieldDialog::copyRow()
{
  m_tableModel->copyRow(getSelectedRow());
}

void TableSortFieldDialog::addRow()
{
  TableSortField field;
  m_tableModel->insertRow(getSelectedRow(), field);
}

void TableSortFieldDialog::loadConfiguration()
{
  QString fileExtension;
  QString s = QFileDialog::getOpenFileName(this, tr("Select Configuration File To Open"), getConfigFilePath(), tr("XML(*.xml);;Text(*.txt);;All(*.*)"), &fileExtension, QFileDialog::DontUseNativeDialog);
  if (s.length() > 0)
  {
    setConfigFilePath(s);
    QFile file(s);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QMessageBox::warning(this, tr("ERROR"), tr("Failed to open file for reading."));
    }
    else
    {
      QXmlStreamReader reader(&file);
      if (reader.hasError())
      {
        QMessageBox::warning(this, tr("ERROR"), tr("Failed to create output stream for reading."));
      }
      else
      {
        // TODO:Read the file
        //SimpleLoggerADP logger;
        //logger.read(reader);
        //configureDialog(logger);
      }
      file.close();
    }
  }
}

void TableSortFieldDialog::saveConfiguration()
{
  QString fileExtension;
  QString s = QFileDialog::getSaveFileName(this, tr("Save To"), getConfigFilePath(), tr("XML(*.xml);;Text(*.txt);;All(*.*)"), &fileExtension, QFileDialog::DontUseNativeDialog);
  if (s.length() > 0)
  {
    if (!s.contains('.'))
    {
      if (fileExtension.compare(tr("XML(*.xml)"), Qt::CaseInsensitive) == 0)
      {
        s = s + ".xml";
      }
      else if (fileExtension.compare(tr("Text(*.txt)"), Qt::CaseInsensitive) == 0)
      {
        s = s + ".txt";
      }
    }
    setConfigFilePath(s);
    QFile file(s);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      QMessageBox::warning(this, tr("ERROR"), tr("Failed to open file for writing."));
    }
    else
    {
      QXmlStreamWriter writer(&file);
      if (writer.hasError())
      {
        QMessageBox::warning(this, tr("ERROR"), tr("Failed to create output stream for writing logger."));
      }
      else
      {
        writer.setAutoFormatting(true);
        // TODO:
        //SimpleLoggerADP logger;
        //configureLogger(logger);
        //logger.write(writer);
      }
      file.close();
    }
  }
}


void TableSortFieldDialog::buildDialog()
{
  setWindowTitle(tr("Configure Logging"));
  QPushButton* button;
  QVBoxLayout *vLayout;
  QHBoxLayout *hLayout;
  QFormLayout *fLayout = new QFormLayout;

  m_configFilePath = new QLineEdit();

  hLayout = new QHBoxLayout();

  vLayout = new QVBoxLayout();
  vLayout->addWidget(new QLabel(tr("Configuration File: ")));
  vLayout->addWidget(new QLabel(tr("Log File: ")));
  hLayout->addLayout(vLayout);

  vLayout = new QVBoxLayout();
  vLayout->addWidget(m_configFilePath);
  hLayout->addLayout(vLayout);

  vLayout = new QVBoxLayout();
  button = new QPushButton(tr("Load"));
  connect(button, SIGNAL(clicked()), this, SLOT(loadLogConfiguration()));
  vLayout->addWidget(button);
  button = new QPushButton(tr("Select"));
  connect(button, SIGNAL(clicked()), this, SLOT(selectLogFile()));
  vLayout->addWidget(button);
  hLayout->addLayout(vLayout);

  vLayout = new QVBoxLayout();
  button = new QPushButton(tr("Save"));
  connect(button, SIGNAL(clicked()), this, SLOT(saveLogConfiguration()));
  vLayout->addWidget(button);
  vLayout->addStretch();
  hLayout->addLayout(vLayout);

  fLayout->addRow(hLayout);

  hLayout = new QHBoxLayout();
  vLayout = new QVBoxLayout();
  vLayout->addWidget(new QLabel(tr("Routings:")));
  button = new QPushButton(tr("Edit"));
  connect(button, SIGNAL(clicked()), this, SLOT(editSelectedRouting()));
  vLayout->addWidget(button);
  button = new QPushButton(tr("Add"));
  connect(button, SIGNAL(clicked()), this, SLOT(addRouting()));
  vLayout->addWidget(button);
  button = new QPushButton(tr("Copy"));
  connect(button, SIGNAL(clicked()), this, SLOT(copyRouting()));
  vLayout->addWidget(button);
  button = new QPushButton(tr("Delete"));
  connect(button, SIGNAL(clicked()), this, SLOT(delSelectedRouting()));
  vLayout->addWidget(button);
  vLayout->addStretch();
  hLayout->addLayout(vLayout);

  m_tableView = new QTableView();
  m_tableModel = new TableSortFieldTableModel();
  m_tableView->setModel(m_tableModel);

  LinkBackFilterDelegate* delegate = new LinkBackFilterDelegate(m_tableView);
  m_tableView->setItemDelegate(delegate);

  CheckBoxOnlyDelegate * cboDelegate = new CheckBoxOnlyDelegate(m_tableView);
  m_tableView->setItemDelegateForColumn(TableSortFieldTableModel::ascendingColumn, cboDelegate);
  m_tableView->setItemDelegateForColumn(TableSortFieldTableModel::caseColumn, cboDelegate);

  hLayout->addWidget(m_tableView);
  fLayout->addRow(hLayout);


  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  fLayout->addRow(buttonBox);

  setLayout(fLayout);

  QSettings settings;
  restoreGeometry(settings.value("Settings_FilterFieldDlgGeometry").toByteArray());
  setConfigFilePath(settings.value("SortFieldConfigDialogLastConfigPath").toString());
  QString s = settings.value("SortFieldConfigDialogRoutingColumnWidths").toString();
  if (s.length() > 0)
  {
    QStringList list = s.split(',');
    bool ok = true;
    for (int i=0; i<list.count() && i<TableSortFieldTableModel::numColumns; ++i)
    {
      int width = list[i].toInt(&ok);
      if (ok && width > 0)
      {
        m_tableView->setColumnWidth(i, width);
      }
    }
  }
}

void TableSortFieldDialog::initialize()
{
#if 0
  ui->setupUi(this);
  ui->componentTableView->setModel(&m_messageComponentTableModel);
  LinkBackFilterDelegate* delegate = new LinkBackFilterDelegate(ui->componentTableView);
  ui->componentTableView->setItemDelegate(delegate);
  ui->componentTableView->setColumnWidth(m_messageComponentTableModel.fieldColumn, 240);
  ui->componentTableView->setColumnWidth(m_messageComponentTableModel.stringColumn, 240);

  connect(ui->componentTableView->selectionModel(), SIGNAL(currentChanged ( const QModelIndex &, const QModelIndex &)), this, SLOT(currentMessageCategoryRowChanged ( const QModelIndex &, const QModelIndex &)));
  connect(ui->copyCompButton, SIGNAL(clicked(bool)), this, SLOT( copyMessageCategory()));
  connect(ui->insertCompButton, SIGNAL(clicked(bool)), this, SLOT(insertMessageCategory()));
  connect(ui->upCompButton, SIGNAL(clicked(bool)), this, SLOT(upMessageCategory()));
  connect(ui->downCompButton, SIGNAL(clicked(bool)), this, SLOT(downMessageCategory()));
  connect(ui->delCompButton, SIGNAL(clicked(bool)), this, SLOT(delMessageCategory()));

  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(closeRequested()));
  connect(ui->testButton, SIGNAL(clicked(bool)), this, SLOT(testMessage()));

  ui->locCaseSensitive->addItem(tr("Case Sensitive"), Qt::CaseSensitive);
  ui->locCaseSensitive->addItem(tr("Case Insensitive"), Qt::CaseInsensitive);
  ui->locCaseSensitive->setToolTip(tr("Set location regular expression case sensitivity."));

  ui->msgCaseSensitive->addItem(tr("Case Sensitive"), Qt::CaseSensitive);
  ui->msgCaseSensitive->addItem(tr("Case Insensitive"), Qt::CaseInsensitive);
  ui->msgCaseSensitive->setToolTip(tr("Set location regular expression case sensitivity."));

  ui->locSyntax->addItem(tr("Perl No Greedy"), QRegExp::RegExp);
  ui->locSyntax->addItem(tr("Perl Greedy"), QRegExp::RegExp2);
  ui->locSyntax->addItem(tr("Wildcard"), QRegExp::Wildcard);
  ui->locSyntax->addItem(tr("Wildcard Unix"), QRegExp::WildcardUnix);
  ui->locSyntax->addItem(tr("String"), QRegExp::FixedString);
  ui->locSyntax->addItem(tr("W3C XML"), QRegExp::W3CXmlSchema11);
  ui->locSyntax->setToolTip(tr("Set location regular expression pattern syntax."));

  ui->msgSyntax->addItem(tr("Perl No Greedy"), QRegExp::RegExp);
  ui->msgSyntax->addItem(tr("Perl Greedy"), QRegExp::RegExp2);
  ui->msgSyntax->addItem(tr("Wildcard"), QRegExp::Wildcard);
  ui->msgSyntax->addItem(tr("Wildcard Unix"), QRegExp::WildcardUnix);
  ui->msgSyntax->addItem(tr("String"), QRegExp::FixedString);
  ui->msgSyntax->addItem(tr("W3C XML"), QRegExp::W3CXmlSchema11);
  ui->locSyntax->setToolTip(tr("Set unformatted message regular expression pattern syntax."));

  ui->locRegExpEdit->setToolTip(tr("Match location filename:line_number must match the regular expression; use to filter by filename or line number."));
  ui->msgRegExpEdit->setToolTip(tr("Match based on the unformatted message text."));

  ui->nameEdit->setToolTip(tr("User recognizable name for this object to pick it out in a list."));
#endif
  enableButtons();
}

void TableSortFieldDialog::enableButtons()
{
  //int count = m_messageComponentTableModel.messageComponentCount();
  //int row = count > 0 ? getSelectedCatRow() : -1;
  //bool b = count > 0 && row >= 0;
  //ui->copyCompButton->setEnabled(b);
  //ui->delCompButton->setEnabled(b);
  //ui->upCompButton->setEnabled(b && row > 0);
  //ui->downCompButton->setEnabled(b && row < (count - 1));
}

