#include "genericdatacollectiontablefilterdialog.h"
#include "genericdatacollectiontabledialog.h"
#include "genericdatacollectionstablemodel.h"


#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QGroupBox>
#include <QRadioButton>

GenericDataCollectionTableFilterDialog::GenericDataCollectionTableFilterDialog(GenericDataCollectionTableDialog* tableDialog, QWidget *parent) :
  QDialog(parent), m_tableDialog(tableDialog),
  m_findValueLineEdit(nullptr), m_replaceValueLineEdit(nullptr), m_matchCaseCB(nullptr), m_matchEntireCellCB(nullptr),
  m_regularExpressionCB(nullptr), m_selectionOnlyCB(nullptr)
{
  if (m_tableDialog != nullptr)
  {
    buildDialog();
  }
}


// filter string, regexp, wild card
// Filter role (view, edit)
// Filter column (drop down)
// set locale aware
// case sensitive

void GenericDataCollectionTableFilterDialog::buildDialog()
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

  QGroupBox* filterType = new QGroupBox();
  m_rbString = new QRadioButton(tr("String"));
  m_rbRegExp = new QRadioButton(tr("Regular Expression"));
  m_rbWild = new QRadioButton(tr("Wild Card"));


  QPushButton* button;
  QVBoxLayout *vLayout;
  QVBoxLayout *vLayout2;
  //QHBoxLayout *hLayout;
  QFormLayout *fLayout = new QFormLayout;

  vLayout = new QVBoxLayout();
  m_findValueLineEdit = new QLineEdit();
  m_replaceValueLineEdit = new QLineEdit();

  //hLayout = new QHBoxLayout();
  vLayout->addWidget(new QLabel(tr("Search for")));
  button = new QPushButton(tr("Find"));
  connect(button, SIGNAL(released()), this, SLOT(find()));
  QHBoxLayout *hLayout2 = new QHBoxLayout();
  hLayout2->addWidget(m_findValueLineEdit);
  hLayout2->addWidget(button);
  vLayout->addLayout(hLayout2);

  vLayout->addWidget(new QLabel(tr("Replace with")));
  vLayout2 = new QVBoxLayout();
  button = new QPushButton(tr("Replace"));
  connect(button, SIGNAL(released()), this, SLOT(replace()));
  vLayout2->addWidget(button);
  button = new QPushButton(tr("Replace All"));
  connect(button, SIGNAL(released()), this, SLOT(replaceAll()));
  vLayout2->addWidget(button);
  hLayout2 = new QHBoxLayout();
  hLayout2->addWidget(m_replaceValueLineEdit);
  hLayout2->addLayout(vLayout2);
  vLayout->addLayout(hLayout2);

  hLayout2 = new QHBoxLayout();
  vLayout2 = new QVBoxLayout();
  m_matchCaseCB = new QCheckBox(tr("Match case"));
  vLayout2->addWidget(m_matchCaseCB);
  m_regularExpressionCB = new QCheckBox(tr("Regular expression"));
  vLayout2->addWidget(m_regularExpressionCB);
  hLayout2->addLayout(vLayout2);

  vLayout2 = new QVBoxLayout();
  m_matchEntireCellCB = new QCheckBox(tr("Match cell only"));
  vLayout2->addWidget(m_matchEntireCellCB);
  m_selectionOnlyCB = new QCheckBox(tr("Current Selection Only"));
  vLayout2->addWidget(m_selectionOnlyCB);
  hLayout2->addLayout(vLayout2);

  vLayout->addLayout(hLayout2);
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  vLayout->addWidget(buttonBox);

  /**
  hLayout2 = new QHBoxLayout();
  m_matchCaseCB = new QCheckBox(tr("Match case"));
  hLayout2->addWidget(m_matchCaseCB);
  m_matchEntireCellCB = new QCheckBox(tr("Match cell only"));
  hLayout2->addWidget(m_matchEntireCellCB);
  vLayout->addLayout(hLayout2);

  hLayout2 = new QHBoxLayout();
  m_regularExpressionCB = new QCheckBox(tr("Regular expression"));
  hLayout2->addWidget(m_regularExpressionCB);
  m_selectionOnlyCB = new QCheckBox(tr("Current Selection Only"));
  hLayout2->addWidget(m_selectionOnlyCB);
  vLayout->addLayout(hLayout2);
**/


  //vLayout = new QVBoxLayout();
  //vLayout->addWidget(new QLabel(tr("Find: ")));
  //hLayout->addLayout(vLayout);
  //vLayout->addWidget(new QLabel(tr("Replace: ")));
  //hLayout->addLayout(vLayout);

  //hLayout->addLayout(vLayout);

  //vLayout = new QVBoxLayout();
  //vLayout->addWidget(m_findValueLineEdit);
  //vLayout->addWidget(m_replaceValueLineEdit);
  //hLayout->addLayout(vLayout);

  fLayout->addRow(vLayout);
  setLayout(fLayout);


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

void GenericDataCollectionTableFilterDialog::enableButtons()
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

void GenericDataCollectionTableFilterDialog::setCaseSensitive(const Qt::CaseSensitivity sensitivity)
{
  if (m_matchCaseCB != nullptr)
  {
    m_matchCaseCB->setChecked(sensitivity == Qt::CaseSensitive);
  }
}

Qt::CaseSensitivity GenericDataCollectionTableFilterDialog::getCaseSensitivity() const
{
  return (m_matchCaseCB != nullptr && m_matchCaseCB->isChecked()) ? Qt::CaseSensitive : Qt::CaseInsensitive;
}


#include "genericdatacollectionstableproxy.h"

void GenericDataCollectionTableFilterDialog::clearFilter()
{

}

void GenericDataCollectionTableFilterDialog::applyFilter()
{
  QString searchString = m_findValueLineEdit->text();

  GenericDataCollectionsTableProxy* pm = m_tableDialog->getProxyModel();
  if (pm != nullptr)
  {
    pm->setFilterCaseSensitivity(Qt::CaseInsensitive);
    pm->setFilterKeyColumn(4);
    pm->setFilterFixedString(searchString);
  }
  /**
  QSortFilterProxyModel proxy;
  proxy.setSourceModel(m_tableDialog->getTableModel());
  proxy.setFilterCaseSensitivity(Qt::CaseInsensitive);
  proxy.setFilterRole(Qt::DisplayRole);

  int num_rows = m_tableDialog->getTableModel()->rowCount();
  int num_cols = m_tableDialog->getTableModel()->columnCount();
  //QModelIndex matchingIndex = proxy.mapFromSource(proxy.index(num_rows,num_cols));
  for (int i=0; i<num_cols; ++i)
  {
    proxy.setFilterKeyColumn(i);
    proxy.setFilterFixedString(searchString);
    proxy.invalidate();
  QModelIndex matchingIndex = proxy.mapToSource(proxy.index(num_rows,0));

   if(matchingIndex.isValid())
   {
       QMessageBox::information(this, "Find", QString("Found in col %1").arg(i));
   }
   else
   {
       //QMessageBox::information(this, "Find", QString("Not Found in col %1").arg(i));
   }
  }
  **/
}
