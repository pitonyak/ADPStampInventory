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
  QDialog(parent), m_upButton(nullptr), m_downButton(nullptr), m_addButton(nullptr), m_deleteButton(nullptr),
  m_dataCollection(dataCollection), m_tableView(nullptr), m_configFilePath(nullptr), m_tableModel(nullptr)
{
  buildDialog();
}

TableSortFieldDialog::~TableSortFieldDialog()
{
  QSettings settings;
  settings.setValue(Constants::Settings_SortFieldDlgGeometry, saveGeometry());
  settings.setValue(Constants::SortFieldConfigDialogLastConfigPath, getConfigFilePath());

    if (m_tableView != nullptr)
    {
      QString s;
      for (int i=0; i<TableSortFieldTableModel::numColumns; ++i)
      {
        if (s.length() > 0)
        {
          s = s + QString(",%1").arg(m_tableView->columnWidth(i));
        }
        else
        {
          s = QString("%1").arg(m_tableView->columnWidth(i));
        }
      }
      settings.setValue(Constants::SortFieldConfigDialogRoutingColumnWidths, s);
    }

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
    enableButtons();
  }
}

void TableSortFieldDialog::copyRow()
{
  m_tableModel->copyRow(getSelectedRow());
  enableButtons();
}

void TableSortFieldDialog::addRow()
{
  if (m_dataCollection == nullptr) {
    return;
  }
  for (int i=0; i<m_dataCollection->getPropertyNameCount(); ++i)
  {
    if (!m_tableModel->hasFieldName(m_dataCollection->getPropertyName(i)))
    {
      TableSortField field;
      field.setFieldName(m_dataCollection->getPropertyName(i));
      field.setFieldIndex(i);
      field.setFieldType(m_mapper.variantTypeToMetaType(m_dataCollection->getPropertyTypeVariant(i)));
      m_tableModel->insertRow(getSelectedRow(), field);
      enableButtons();
      return;
    }
  }
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
        QList<TableSortField> list;
        TableSortField::read(list, reader);
        m_tableModel->clear();
        m_tableModel->add(list);
        enableButtons();
      }
      file.close();
    }
  }
}

void TableSortFieldDialog::selectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
  (void)selected;
  (void)deselected;
  enableButtons();
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
        TableSortField::write(m_tableModel->getList(), writer);
      }
      file.close();
    }
  }
}

void TableSortFieldDialog::rowUp()
{
  m_tableModel->moveRowUp(getSelectedRow());
  enableButtons();
}

void TableSortFieldDialog::rowDown()
{
  m_tableModel->moveRowDown(getSelectedRow());
  enableButtons();
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
  hLayout->addLayout(vLayout);

  vLayout = new QVBoxLayout();
  vLayout->addWidget(m_configFilePath);
  hLayout->addLayout(vLayout);

  button = new QPushButton(tr("Load"));
  connect(button, SIGNAL(clicked()), this, SLOT(loadConfiguration()));
  hLayout->addWidget(button);

  button = new QPushButton(tr("Save"));
  connect(button, SIGNAL(clicked()), this, SLOT(saveConfiguration()));
  hLayout->addWidget(button);

  fLayout->addRow(hLayout);

  hLayout = new QHBoxLayout();
  vLayout = new QVBoxLayout();
  vLayout->addWidget(new QLabel(tr("Sort Fields:")));
  button = new QPushButton(tr("Add"));
  connect(button, SIGNAL(clicked()), this, SLOT(addRow()));
  vLayout->addWidget(button);
  m_addButton = button;

  button = new QPushButton(tr("Delete"));
  connect(button, SIGNAL(clicked()), this, SLOT(delSelectedRow()));
  vLayout->addWidget(button);
  m_deleteButton = button;

  button = new QPushButton(tr("Up"));
  connect(button, SIGNAL(clicked()), this, SLOT(rowUp()));
  vLayout->addWidget(button);
  m_upButton = button;

  button = new QPushButton(tr("Down"));
  connect(button, SIGNAL(clicked()), this, SLOT(rowDown()));
  vLayout->addWidget(button);
  m_downButton = button;

  vLayout->addStretch();
  hLayout->addLayout(vLayout);

  m_tableView = new QTableView();
  m_tableModel = new TableSortFieldTableModel(m_dataCollection);
  m_tableView->setModel(m_tableModel);

  // Parameter sets the owning parent that will delete the delegate.
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
  restoreGeometry(settings.value(Constants::Settings_SortFieldDlgGeometry).toByteArray());
  setConfigFilePath(settings.value(Constants::SortFieldConfigDialogLastConfigPath).toString());
  QString s = settings.value(Constants::SortFieldConfigDialogRoutingColumnWidths).toString();
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
  connect(m_tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
  enableButtons();
}

void TableSortFieldDialog::enableButtons()
{
  if (m_dataCollection != nullptr)
  {
    int count = m_tableModel->count();
    int row = count > 0 ? getSelectedRow() : -1;
    m_upButton->setEnabled(row > 0);
    m_downButton->setEnabled(row >= 0 && row < (count - 1));
    m_deleteButton->setEnabled(row >= 0);
    m_addButton->setEnabled(count < m_dataCollection->getPropertyNameCount());
  }
}

