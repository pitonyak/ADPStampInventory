#include "genericdatacollectiontabledialog.h"
#include "genericdatacollectiontablemodel.h"
#include "linkbackfilterdelegate.h"
#include "checkboxonlydelegate.h"
#include "constants.h"
#include "stampdb.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTableView>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QSettings>
#include <QItemSelection>
#include <QSortFilterProxyModel>

GenericDataCollectionTableDialog::GenericDataCollectionTableDialog(const QString& name, GenericDataCollection &data, StampDB &db, QWidget *parent) :
  QDialog(parent),
  m_duplicateButton(nullptr), m_addButton(nullptr), m_deleteButton(nullptr), m_undoButton(nullptr),
  m_SaveChangesButton(nullptr),
  m_dataCollection(data), m_tableView(nullptr), m_name(name), m_tableModel(nullptr), m_db(db)
{
  buildDialog();
}

GenericDataCollectionTableDialog::~GenericDataCollectionTableDialog()
{
}

void GenericDataCollectionTableDialog::buildDialog()
{
  setWindowTitle(tr("Edit Table %1").arg(m_name));
  QVBoxLayout *vLayout;
  QHBoxLayout *hLayout;

  // Build the primary table.
  m_tableView = new QTableView();
  m_tableModel = new GenericDataCollectionTableModel(m_dataCollection);

  m_proxyModel = new QSortFilterProxyModel(this);
  m_proxyModel->setSourceModel(m_tableModel);

  //??m_tableView->setModel(m_tableModel);
  m_tableView->setModel(m_proxyModel);
  m_tableView->setSortingEnabled(true);

  qDebug(qPrintable(QString("Num columns %1").arg(m_dataCollection.getPropertyNameCount())));

  // Parameter sets the owning parent that will delete the delegate.
  LinkBackFilterDelegate* delegate = new LinkBackFilterDelegate(m_tableView);
  m_tableView->setItemDelegate(delegate);

  // For all columns that are boolean, use this delegate.
  CheckBoxOnlyDelegate * cboDelegate = nullptr;
  for (int i=m_dataCollection.getPropertyNameCount() - 1; i>=0; --i) {
    if (m_dataCollection.getPropertyTypeMeta(i) == QMetaType::Bool) {
      if (cboDelegate == nullptr) {
        cboDelegate = new CheckBoxOnlyDelegate(m_tableView);
      }
      m_tableView->setItemDelegateForColumn(i, cboDelegate);
    }
    //qDebug(qPrintable(QString("%1 is type %2").arg(m_dataCollection.getPropertyName(i)).arg(m_dataCollection.getPropertyTypeMeta(i))));
  }

  vLayout = new QVBoxLayout();
  vLayout->addWidget(m_tableView, 0, 0);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(clickedOK()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  hLayout = new QHBoxLayout();
  m_addButton = new QPushButton(tr("Add"));
  hLayout->addWidget(m_addButton);
  m_deleteButton = new QPushButton(tr("Delete"));
  hLayout->addWidget(m_deleteButton);
  m_duplicateButton = new QPushButton(tr("Duplicate"));
  hLayout->addWidget(m_duplicateButton);
  m_undoButton = new QPushButton(tr("Undo"));
  hLayout->addWidget(m_undoButton);
  m_SaveChangesButton = new QPushButton(tr("Save Changes"));
  hLayout->addWidget(m_SaveChangesButton);
  hLayout->addWidget(buttonBox);

  connect(m_addButton, SIGNAL(clicked()), this, SLOT(addRow()));
  connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteRow()));
  connect(m_duplicateButton, SIGNAL(clicked()), this, SLOT(duplicateRow()));
  connect(m_undoButton, SIGNAL(clicked()), this, SLOT(undoChange()));
  connect(m_SaveChangesButton, SIGNAL(clicked()), this, SLOT(saveChanges()));

  vLayout->addLayout(hLayout);

  setLayout(vLayout);

  QSettings settings;
  restoreGeometry(settings.value(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgGeometry).arg(m_name)).toByteArray());
  QString s = settings.value(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgColumnWidths).arg(m_name)).toString();
  if (s.length() > 0)
  {
    QStringList list = s.split(',');
    bool ok = true;
    for (int i=0; i<list.count() && i<m_dataCollection.getPropertyNameCount(); ++i)
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

void GenericDataCollectionTableDialog::selectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
  (void)selected;
  (void)deselected;
  enableButtons();
}

bool GenericDataCollectionTableDialog::isRowSelected() const
{
  return m_tableModel != nullptr && m_tableView != nullptr && m_tableModel->rowCount() > 0 && m_tableView->currentIndex().row() >= 0;
}

void GenericDataCollectionTableDialog::enableButtons()
{
  bool somethingSelected = isRowSelected();
  m_duplicateButton->setEnabled(somethingSelected);
  m_addButton->setEnabled(true);
  m_deleteButton->setEnabled(somethingSelected);
  m_undoButton->setEnabled(!m_tableModel->trackerIsEmpty());
  m_SaveChangesButton->setEnabled(!m_tableModel->trackerIsEmpty());
}

void GenericDataCollectionTableDialog::addRow()
{
  m_tableModel->addRow();
  enableButtons();
}

void GenericDataCollectionTableDialog::deleteRow()
{
  QModelIndexList proxyList = m_tableView->selectionModel()->selectedIndexes();
  QModelIndexList mappedList;
  for (int i=0; i<proxyList.count(); ++i) {
    mappedList.append(m_proxyModel->mapToSource(proxyList.at(i)));
  }
  m_tableModel->deleteRows(mappedList);
  enableButtons();
}

void GenericDataCollectionTableDialog::undoChange()
{
  m_tableModel->undoChange();
  enableButtons();
}

void GenericDataCollectionTableDialog::saveChanges()
{
  m_tableModel->saveTrackedChanges(m_name, m_dataCollection, m_db.getDB());
  enableButtons();
}

void GenericDataCollectionTableDialog::duplicateRow()
{
  qDebug("DuplicateRow()");

  QModelIndexList proxyList = m_tableView->selectionModel()->selectedIndexes();
  QModelIndexList mappedList;
  for (int i=0; i<proxyList.count(); ++i) {
    mappedList.append(m_proxyModel->mapToSource(proxyList.at(i)));
  }
  m_tableModel->duplicateRows(mappedList);
  // TODO: Select the first inserted one.
  enableButtons();
}

void GenericDataCollectionTableDialog::saveState()
{
  // This code used to live in the desructor, but, sometime around September 2014, this code
  // started failing in the destructor because  the the m_dataCollection no longer contained data
  // by the time that this objects destructor was called. So, now this is called before the destructor.
  // save the dialog state.
  QSettings settings;
  settings.setValue(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgGeometry).arg(m_name), saveGeometry());

    if (m_tableView != nullptr)
    {
      QString s;
      for (int i=0; i<m_dataCollection.getPropertyNameCount(); ++i)
      {
        //qDebug(qPrintable(QString("(%1)(%2)").arg(i).arg(m_tableView->columnWidth(i))));
        if (s.length() > 0)
        {
          s = s + QString(",%1").arg(m_tableView->columnWidth(i));
        }
        else
        {
          s = QString("%1").arg(m_tableView->columnWidth(i));
        }
      }
      //qDebug(qPrintable(QString("(%1)(%2)").arg(m_name).arg(s)));
      settings.setValue(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgColumnWidths).arg(m_name), s);
    }
}

void GenericDataCollectionTableDialog::clickedOK()
{
  // TODO: Push changes back into the database.
  saveState();
  accept();
}




