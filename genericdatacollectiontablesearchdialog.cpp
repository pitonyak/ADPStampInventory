#include "genericdatacollectiontablesearchdialog.h"

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>

GenericDataCollectionTableSearchDialog::GenericDataCollectionTableSearchDialog(QWidget *parent) :
    QDialog(parent)
{
}

void GenericDataCollectionTableSearchDialog::buildDialog()
{
  setWindowTitle(tr("Search Table"));

  // Radio buttons for search:
  // Column (enable column selector)
  // table
  // selected

  // Check boxes for:
  // case sensitive
  // entire cell

  // Search value (based on what I want to find)
  // Replace value (baesd on what I want to find)

  QPushButton* button;
  QVBoxLayout *vLayout;
  QHBoxLayout *hLayout;
  QFormLayout *fLayout = new QFormLayout;

  m_findValueLineEdit = new QLineEdit();
  m_replaceValueLineEdit = new QLineEdit();

  hLayout = new QHBoxLayout();

  vLayout = new QVBoxLayout();
  vLayout->addWidget(new QLabel(tr("Find: ")));
  hLayout->addLayout(vLayout);
  vLayout->addWidget(new QLabel(tr("Replace: ")));
  hLayout->addLayout(vLayout);

  hLayout->addLayout(vLayout);

  vLayout = new QVBoxLayout();
  vLayout->addWidget(m_findValueLineEdit);
  vLayout->addWidget(m_replaceValueLineEdit);
  hLayout->addLayout(vLayout);

  fLayout->addRow(hLayout);


  /**
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
    for (int i=0; i<list.size() && i<TableSortFieldTableModel::numColumns; ++i)
    {
      int width = list[i].toInt(&ok);
      if (ok && width > 0)
      {
        m_tableView->setColumnWidth(i, width);
      }
    }
  }
  connect(m_tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
  **/
  enableButtons();
}

void GenericDataCollectionTableSearchDialog::enableButtons()
{
    /**
  if (m_dataCollection != nullptr)
  {
    int count = m_tableModel->size();
    int row = count > 0 ? getSelectedRow() : -1;
    m_upButton->setEnabled(row > 0);
    m_downButton->setEnabled(row >= 0 && row < (count - 1));
    m_deleteButton->setEnabled(row >= 0);
    m_addButton->setEnabled(count < m_dataCollection->getPropertyNameCount());
  }
  **/
}
