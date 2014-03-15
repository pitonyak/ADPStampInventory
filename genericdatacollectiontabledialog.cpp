#include "genericdatacollectiontabledialog.h"
#include "genericdatacollectiontablemodel.h"
#include "linkbackfilterdelegate.h"
#include "checkboxonlydelegate.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTableView>
#include <QLabel>
#include <QPushButton>

GenericDataCollectionTableDialog::GenericDataCollectionTableDialog(GenericDataCollection &data, QWidget *parent) :
  QDialog(parent), m_dataCollection(data), m_tableView(nullptr), m_tableModel(nullptr)
{
  buildDialog();
}


GenericDataCollectionTableDialog::~GenericDataCollectionTableDialog()
{
  // TODO: save the dialog state.
}

void GenericDataCollectionTableDialog::buildDialog()
{
  setWindowTitle(tr("Edit Table"));
  QPushButton* button;
  QVBoxLayout *vLayout;
  QHBoxLayout *hLayout;
  //QFormLayout *fLayout = new QFormLayout;
  //QGridLayout * gLayout = new QGridLayout();

  // Build the primary table.
  m_tableView = new QTableView();
  m_tableModel = new GenericDataCollectionTableModel(m_dataCollection);
  m_tableView->setModel(m_tableModel);

  qDebug(qPrintable(QString("Num columns %1").arg(m_dataCollection.getPropertyNameCount())));

  // Parameter sets the owning parent that will delete the delegate.
  LinkBackFilterDelegate* delegate = new LinkBackFilterDelegate(m_tableView);
  m_tableView->setItemDelegate(delegate);

  // TODO: For all columns that are boolean, use this delegate.
  CheckBoxOnlyDelegate * cboDelegate = nullptr;
  for (int i=m_dataCollection.getPropertyNameCount() - 1; i>=0; --i) {
    if (m_dataCollection.getPropertyTypeMeta(i) == QMetaType::Bool) {
      if (cboDelegate == nullptr) {
        cboDelegate = new CheckBoxOnlyDelegate(m_tableView);
      }
      m_tableView->setItemDelegateForColumn(i, cboDelegate);
    }
  }

  hLayout = new QHBoxLayout();
  hLayout->addWidget(new QPushButton(tr("Ok")));
  //hLayout->addWidget(m_tableView);
  vLayout = new QVBoxLayout();
  vLayout->addWidget(m_tableView, 0, 0);
  vLayout->addLayout(hLayout);
  //fLayout->addRow(new QLabel(tr("Table")), m_tableView);
  //fLayout->addRow(hLayout);

  // Will hold the bottom buttons.
  //??hLayout = new QHBoxLayout();

  //button = new QPushButton(tr("Load"));
  //connect(button, SIGNAL(clicked()), this, SLOT(loadConfiguration()));
  //hLayout->addWidget(button);

  //setLayout(fLayout);
  setLayout(vLayout);

  /**
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
  **/

  //QSizePolicy sp = m_tableView->sizePolicy();
  //qDebug(qPrintable(QString("Expanding Flag:%1 Expanding:%2 GrowFlag:%3").arg(sp.ExpandFlag).arg(sp.Expanding).arg(sp.GrowFlag)));
  //qDebug(qPrintable(QString("Horizontal:%1 Vertical:%2 StretchV:%3 StretchH:%4").arg(sp.horizontalPolicy()).arg(sp.verticalPolicy()).arg(sp.verticalStretch()).arg(sp.horizontalStretch())));

}
