#include "genericdatacollectiontabledialog.h"
#include "genericdatacollectiontablemodel.h"
#include "genericdatacollectionstablemodel.h"
#include "linkbackfilterdelegate.h"
#include "checkboxonlydelegate.h"
#include "constants.h"
#include "stampdb.h"
#include "describesqltables.h"
#include "genericdatacollectionstableproxy.h"

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
#include <QMessageBox>

GenericDataCollectionTableDialog::GenericDataCollectionTableDialog(const QString& tableName, GenericDataCollection &data, StampDB &db, DescribeSqlTables& schema, GenericDataCollections *tables, QWidget *parent) :
  QDialog(parent),
  m_duplicateButton(nullptr), m_duplicateButtonIncrement(nullptr),
  m_addButton(nullptr), m_deleteButton(nullptr), m_undoButton(nullptr),
  m_SaveChangesButton(nullptr),
  m_table(data), m_tables(tables), m_tableView(nullptr),
  m_tableName(tableName), m_tableModel(nullptr),
  m_db(db), m_schema(schema)
{
  buildDialog();
}

GenericDataCollectionTableDialog::~GenericDataCollectionTableDialog()
{
}

void GenericDataCollectionTableDialog::buildDialog()
{
  setWindowTitle(tr("Edit Table %1").arg(m_tableName));
  QVBoxLayout *vLayout;
  QHBoxLayout *hLayout;

  // Build the primary table.
  m_tableView = new QTableView();
  Q_ASSERT_X(m_tables != nullptr, "GenericDataCollectionTableDialog::buildDialog()", "m_tables is null");
  m_tableModel = new GenericDataCollectionsTableModel(true, m_tableName, *m_tables, m_schema);

  // I could use QSortFilterProxyModel(this), but I want to use a
  // "natural" sort, which recognizes numbers.
  // see http://qt-project.org/doc/qt-5/qsortfilterproxymodel.html
  // http://doc.qt.digia.com/4.6/itemviews-customsortfiltermodel.html
  m_proxyModel = new GenericDataCollectionsTableProxy(this);
  m_proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
  m_proxyModel->setNumericMode(true);
  m_proxyModel->setSourceModel(m_tableModel);

  m_tableView->setModel(m_proxyModel);
  m_tableView->setSortingEnabled(true);

  qDebug(qPrintable(QString("Num columns %1").arg(m_table.getPropertyNameCount())));

  // Parameter sets the owning parent that will delete the delegate.
  LinkBackFilterDelegate* delegate = new LinkBackFilterDelegate(m_tableView);
  delegate->setDateFormatString("MM/dd/yyyy");
  m_tableView->setItemDelegate(delegate);

  // For all columns that are boolean, use this delegate.
  CheckBoxOnlyDelegate * cboDelegate = nullptr;
  for (int i=m_table.getPropertyNameCount() - 1; i>=0; --i) {
    if (m_table.getPropertyTypeMeta(i) == QMetaType::Bool) {
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
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(clickedCancel()));

  hLayout = new QHBoxLayout();
  m_addButton = new QPushButton(tr("Add"));
  hLayout->addWidget(m_addButton);
  m_deleteButton = new QPushButton(tr("Delete"));
  hLayout->addWidget(m_deleteButton);
  m_duplicateButton = new QPushButton(tr("Duplicate"));
  hLayout->addWidget(m_duplicateButton);
  m_duplicateButtonIncrement = new QPushButton(tr("Duplicate+"));
  hLayout->addWidget(m_duplicateButtonIncrement);
  m_undoButton = new QPushButton(tr("Undo"));
  hLayout->addWidget(m_undoButton);
  m_SaveChangesButton = new QPushButton(tr("Save Changes"));
  hLayout->addWidget(m_SaveChangesButton);
  hLayout->addWidget(buttonBox);

  connect(m_addButton, SIGNAL(clicked()), this, SLOT(addRow()));
  connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteRow()));
  connect(m_duplicateButton, SIGNAL(clicked()), this, SLOT(duplicateRow()));
  connect(m_duplicateButtonIncrement, SIGNAL(clicked()), this, SLOT(duplicateRowAutoIncrement()));
  connect(m_undoButton, SIGNAL(clicked()), this, SLOT(undoChange()));
  connect(m_SaveChangesButton, SIGNAL(clicked()), this, SLOT(saveChanges()));

  vLayout->addLayout(hLayout);

  setLayout(vLayout);

  QSettings settings;
  restoreGeometry(settings.value(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgGeometry).arg(m_tableName)).toByteArray());
  QString s = settings.value(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgColumnWidths).arg(m_tableName)).toString();
  if (s.length() > 0)
  {
    QStringList list = s.split(',');
    bool ok = true;
    for (int i=0; i<list.size() && i<m_table.getPropertyNameCount(); ++i)
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
  m_duplicateButtonIncrement->setEnabled(somethingSelected);
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
  for (int i=0; i<proxyList.size(); ++i) {
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
  m_tableModel->saveTrackedChanges(m_tableName, m_table, m_db.getDB(), m_schema);
  enableButtons();
}

void GenericDataCollectionTableDialog::duplicateRow()
{
  privateRowDuplicator(false);
}

void GenericDataCollectionTableDialog::duplicateRowAutoIncrement()
{
  privateRowDuplicator(true);
}

void GenericDataCollectionTableDialog::privateRowDuplicator(const bool autoIncrement)
{
  QModelIndexList proxyList = m_tableView->selectionModel()->selectedIndexes();
  QModelIndexList mappedList;
  for (int i=0; i<proxyList.size(); ++i) {
    mappedList.append(m_proxyModel->mapToSource(proxyList.at(i)));
  }
  QList<int> addedIds = m_tableModel->duplicateRows(mappedList, autoIncrement);
  if (!addedIds.isEmpty())
  {
    int row = m_tableModel->getIndexOf(addedIds.first());
    if (row >= 0) {
      // Select the first inserted Id.
      QModelIndex tableIndex = m_tableModel->getIndexByRowCol(row, 1);
      if (tableIndex.isValid())
      {
        QModelIndex proxyIndex = m_proxyModel->mapFromSource(tableIndex);
        if (proxyIndex.isValid())
        {
          //m_tableView->selectRow(proxyIndex.row());
          m_tableView->clearSelection();
          m_tableView->selectionModel()->select(proxyIndex, QItemSelectionModel::Select);
        }
      }
    }
  }
  enableButtons();
}


void GenericDataCollectionTableDialog::saveState()
{
  // This code used to live in the desructor, but, sometime around September 2014, this code
  // started failing in the destructor because  the the m_dataCollection no longer contained data
  // by the time that this objects destructor was called. So, now this is called before the destructor.
  // save the dialog state.
  QSettings settings;
  settings.setValue(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgGeometry).arg(m_tableName), saveGeometry());

    if (m_tableView != nullptr)
    {
      QString s = "";

      if (m_tableView->isSortingEnabled()) {
          // TODO: How do I know which column is sorted?
      }

      for (int i=0; i<m_table.getPropertyNameCount(); ++i)
      {
        //qDebug(qPrintable(QString("(%1)(%2)").arg(i).arg(m_tableView->columnWidth(i))));
        if (s.length() > 0)
        {
          s.append(',');
        }
        s.append(QString("%1").arg(m_tableView->columnWidth(i)));
      }
      qDebug(qPrintable(QString("(%1)(%2)").arg(m_tableName).arg(s)));
      settings.setValue(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgColumnWidths).arg(m_tableName), s);
    }
}

void GenericDataCollectionTableDialog::clickedOK()
{
  saveState();
  saveChanges();
  accept();
}

void GenericDataCollectionTableDialog::clickedCancel()
{
    if (!m_tableModel->trackerIsEmpty())
    {
        QMessageBox::StandardButton rc = QMessageBox::warning(this, tr("title"), tr("There are unsaved changes, Exit anyway?"), QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel, QMessageBox::Cancel);
        if (rc == QMessageBox::Save)
        {
            clickedOK();
            return;
        }
        else if (rc == QMessageBox::Cancel)
        {
            return;
        }
    }
    reject();
}


