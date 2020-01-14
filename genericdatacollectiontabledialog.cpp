#include "genericdatacollectiontabledialog.h"
#include "genericdatacollectiontablemodel.h"
#include "genericdatacollectionstablemodel.h"
#include "linkbackfilterdelegate.h"
#include "checkboxonlydelegate.h"
#include "constants.h"
#include "stampdb.h"
#include "describesqltables.h"
#include "genericdatacollectionstableproxy.h"
#include "genericdatacollectiontablesearchdialog.h"
#include "globals.h"

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
#include <QKeyEvent>
#include <QHeaderView>
#include <QDebug>
#include <QScopedPointer>

GenericDataCollectionTableDialog::GenericDataCollectionTableDialog(const QString& tableName, GenericDataCollection &data, StampDB &db, DescribeSqlTables& schema, GenericDataCollections *tables, int defaultSourceId, QWidget *parent) :
  QDialog(parent),
  m_duplicateButton(nullptr), m_duplicateButtonIncrement(nullptr),
  m_duplicateButtonAppendLowerA(nullptr), m_duplicateButtonAppendUpperA(nullptr),
  m_addButton(nullptr), m_deleteButton(nullptr), m_undoButton(nullptr),
  m_saveChangesButton(nullptr), m_searchButton(nullptr),
  m_table(data), m_tables(tables), m_tableView(nullptr),
  m_tableName(tableName), m_tableModel(nullptr),m_searchWindow(nullptr),
  m_db(db), m_schema(schema), m_defaultSourceId(defaultSourceId)
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
  m_tableModel = new GenericDataCollectionsTableModel(true, m_tableName, *m_tables, m_schema, m_defaultSourceId);

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

  qDebug() << "Num columns " << m_table.getPropertyNameCount();

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
  vLayout->addWidget(m_tableView, 0, nullptr);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(clickedOK()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(clickedCancel()));

  hLayout = new QHBoxLayout();
  m_searchButton = new QPushButton(tr("Find"));
  connect(m_searchButton, SIGNAL(clicked()), this, SLOT(searchDialog()));
  hLayout->addWidget(m_searchButton);
  vLayout->addLayout(hLayout);

  hLayout = new QHBoxLayout();
  m_addButton = new QPushButton(tr("Add"));
  hLayout->addWidget(m_addButton);
  m_deleteButton = new QPushButton(tr("Delete"));
  hLayout->addWidget(m_deleteButton);
  m_duplicateButton = new QPushButton(tr("Duplicate"));
  hLayout->addWidget(m_duplicateButton);
  m_duplicateButtonIncrement = new QPushButton(tr("Duplicate +"));
  hLayout->addWidget(m_duplicateButtonIncrement);

  m_duplicateButtonAppendLowerA = new QPushButton(tr("Duplicate a"));
  hLayout->addWidget(m_duplicateButtonAppendLowerA);
  m_duplicateButtonAppendUpperA = new QPushButton(tr("Duplicate A"));
  hLayout->addWidget(m_duplicateButtonAppendUpperA);


  m_undoButton = new QPushButton(tr("Undo"));
  hLayout->addWidget(m_undoButton);
  m_saveChangesButton = new QPushButton(tr("Save Changes"));
  hLayout->addWidget(m_saveChangesButton);
  hLayout->addWidget(buttonBox);

  connect(m_addButton, SIGNAL(clicked()), this, SLOT(addRow()));
  connect(m_deleteButton, SIGNAL(clicked()), this, SLOT(deleteRow()));
  connect(m_duplicateButton, SIGNAL(clicked()), this, SLOT(duplicateRow()));
  connect(m_duplicateButtonIncrement, SIGNAL(clicked()), this, SLOT(duplicateRowAutoIncrement()));

  connect(m_duplicateButtonAppendLowerA, SIGNAL(clicked()), this, SLOT(duplicateRowAddLowerA()));
  connect(m_duplicateButtonAppendUpperA, SIGNAL(clicked()), this, SLOT(duplicateRowAddUpperA()));

  connect(m_undoButton, SIGNAL(clicked()), this, SLOT(undoChange()));
  connect(m_saveChangesButton, SIGNAL(clicked()), this, SLOT(saveChanges()));

  vLayout->addLayout(hLayout);

  setLayout(vLayout);

  restoreState();
  connect(m_tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
  enableButtons();
}

void GenericDataCollectionTableDialog::selectionChanged( const QItemSelection & selected, const QItemSelection & deselected )
{
  (void)selected;
  (void)deselected;
  enableButtons();
}

int GenericDataCollectionTableDialog::getSelectedRow() const
{
  return (m_tableModel != nullptr && m_tableView != nullptr && m_tableModel->rowCount() > 0) ? m_tableView->currentIndex().row() : -1;
}


bool GenericDataCollectionTableDialog::isRowSelected() const
{
  return getSelectedRow() >= 0;
}

void GenericDataCollectionTableDialog::disableButtons()
{
    m_duplicateButton->setEnabled(false);
    m_duplicateButtonIncrement->setEnabled(false);
    m_duplicateButtonAppendLowerA->setEnabled(false);
    m_duplicateButtonAppendUpperA->setEnabled(false);
    m_addButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
    m_undoButton->setEnabled(false);
    m_saveChangesButton->setEnabled(false);
    m_searchButton->setEnabled(false);
}

void GenericDataCollectionTableDialog::enableButtons()
{
  bool somethingSelected = isRowSelected();
  m_duplicateButton->setEnabled(somethingSelected);
  m_duplicateButtonIncrement->setEnabled(somethingSelected);
  m_duplicateButtonAppendLowerA->setEnabled(somethingSelected);
  m_duplicateButtonAppendUpperA->setEnabled(somethingSelected);
  m_addButton->setEnabled(true);
  m_deleteButton->setEnabled(somethingSelected);
  m_undoButton->setEnabled(!m_tableModel->trackerIsEmpty());
  m_saveChangesButton->setEnabled(!m_tableModel->trackerIsEmpty());
  m_searchButton->setEnabled(true);
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
  disableButtons();
  m_tableModel->saveTrackedChanges(m_tableName, m_table, m_db.getDB(), m_schema);
  enableButtons();
}

void GenericDataCollectionTableDialog::duplicateRow()
{
  // autoIncrement, setUpdated
  privateRowDuplicator(false, true);
}

void GenericDataCollectionTableDialog::duplicateRowAutoIncrement()
{
  // autoIncrement, setUpdated
  privateRowDuplicator(true, true);
}

void GenericDataCollectionTableDialog::duplicateRowAddLowerA()
{
  // autoIncrement, setUpdated, appendChar, charToAppend
  privateRowDuplicator(false, true, true, 'a');
}

void GenericDataCollectionTableDialog::duplicateRowAddUpperA()
{
  // autoIncrement, setUpdated, appendChar, charToAppend
  privateRowDuplicator(false, true, true, 'A');
}

void GenericDataCollectionTableDialog::incrementCell(const int incrementValue)
{
  QModelIndex index = m_tableView->selectionModel()->currentIndex();
  QModelIndex mappedIndex = m_proxyModel->mapToSource(index);
  m_tableModel->incrementCell(mappedIndex, incrementValue, true);
}

void GenericDataCollectionTableDialog::copyCell(const int rowsDown)
{
  QModelIndex toIndex = m_tableView->selectionModel()->currentIndex();
  QModelIndex fromIndex = m_proxyModel->getIndexByRowCol(toIndex.row() + rowsDown, toIndex.column());

  qDebug() << "To (" << toIndex.row() << ", " << toIndex.column() << ")";
  qDebug() << "From (" << fromIndex.row() << ", " << fromIndex.column() << ")";

  QModelIndex toMappedIndex = m_proxyModel->mapToSource(toIndex);
  QModelIndex fromMappedIndex = m_proxyModel->mapToSource(fromIndex);

  qDebug() << "Mapped To (" << toMappedIndex.row() << ", " << toMappedIndex.column() << ")";
  qDebug() << "Mapped From (" << fromMappedIndex.row() << ", " << fromMappedIndex.column() << ")";

  m_tableModel->copyCell(fromMappedIndex, toMappedIndex);
  enableButtons();
}

void GenericDataCollectionTableDialog::privateRowDuplicator(const bool autoIncrement, const bool setUpdated, const bool appendChar, const char charToAppend)
{
  QModelIndexList proxyList = m_tableView->selectionModel()->selectedIndexes();
  QModelIndexList mappedList;
  for (int i=0; i<proxyList.size(); ++i) {
    mappedList.append(m_proxyModel->mapToSource(proxyList.at(i)));
  }
  QList<int> addedIds = m_tableModel->duplicateRows(mappedList, autoIncrement, setUpdated, appendChar, charToAppend);
  if (!addedIds.isEmpty())
  {
    int row = m_tableModel->getIndexOf(addedIds.first());
    if (row >= 0) {
      // Select the first inserted Id.
      QModelIndex tableIndex = m_tableModel->getIndexByRowCol(row, 1);
      if (tableIndex.isValid())
      {
        selectCell(m_proxyModel->mapFromSource(tableIndex));
      }
    }
  }
  enableButtons();
}

void GenericDataCollectionTableDialog::selectCell(const QModelIndex& index)
{
  if (index.isValid())
  {
    m_tableView->scrollTo(index);
    m_tableView->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    m_tableView->setCurrentIndex(index);
  }
}

void GenericDataCollectionTableDialog::displayHelp()
{
  QMessageBox::about(this, "Supported Keys", "F1 - Help\nF2 - Edit cell\nF3 - Find Next\nShift+F3 - Find Previous\nF10 - Increment current cell\nShift+F10 - Decrement current cell\nCtrl+D - Copy value from column above\nCtrl+d - Copy value from column below\nCtrl+E - Copy value from 2 rows above\nCtrl+e - Copy value from 2 rows below\nESC - Cancel");
}

void GenericDataCollectionTableDialog::restoreState()
{
  QScopedPointer<QSettings> pSettings(getQSettings());
  restoreGeometry(pSettings->value(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgGeometry).arg(m_tableName)).toByteArray());
  QString s = pSettings->value(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgColumnWidths).arg(m_tableName)).toString();
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
  // Restore sort column order.
  s = pSettings->value(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgSorting).arg(m_tableName)).toString();
  if (s.length() > 0)
  {
    QStringList list = s.split(',');
    if (list.size() == 2) {
      bool ok = true;
      int sortColumn = list[0].toInt(&ok);
      if (ok) {
        //Qt:SortOrder order = list[0].toInt(&ok);
        int order = list[0].toInt(&ok);
        if (ok) {
          m_tableView->horizontalHeader()->setSortIndicator(sortColumn, (Qt::SortOrder) order);
        }
      }
    }
  }

}

void GenericDataCollectionTableDialog::saveState()
{
  // This code used to live in the desructor, but, sometime around September 2014, this code
  // started failing in the destructor because  the the m_dataCollection no longer contained data
  // by the time that this objects destructor was called. So, now this is called before the destructor.
  // save the dialog state.
  QScopedPointer<QSettings> pSettings(getQSettings());
  pSettings->setValue(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgGeometry).arg(m_tableName), saveGeometry());

    if (m_tableView != nullptr)
    {
      QString s = "";

      if (m_tableView->isSortingEnabled()) {
        int order = m_tableView->horizontalHeader()->sortIndicatorOrder();
        int sortColumn = m_tableView->horizontalHeader()->sortIndicatorSection();
        s = QString("%1,%2").arg(sortColumn).arg(order);
      }
      pSettings->setValue(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgSorting).arg(m_tableName), s);
      s = "";

      for (int i=0; i<m_table.getPropertyNameCount(); ++i)
      {
        //qDebug(qPrintable(QString("(%1)(%2)").arg(i).arg(m_tableView->columnWidth(i))));
        if (s.length() > 0)
        {
          s.append(',');
        }
        s.append(QString("%1").arg(m_tableView->columnWidth(i)));
      }
      qDebug() << "(" << m_tableName << ")(" << s << ")";
      pSettings->setValue(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgColumnWidths).arg(m_tableName), s);
    }
}

void GenericDataCollectionTableDialog::clickedOK()
{
  saveState();
  saveChanges();
  accept();
}

void GenericDataCollectionTableDialog::keyPressEvent(QKeyEvent* evt)
{
  bool handled = false;

  if ((evt->key() == Qt::Key_F) && ((evt->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) == Qt::ControlModifier))
  {
    genericSearch(false, false, true);
    handled = true;
  }
  else if ((evt->key() == Qt::Key_F3) && (evt->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) == 0)
  {
    if ((evt->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier)
    {
      // Find previous
      genericSearch(false, true, false);
    }
    else
    {
      // Find Next
      genericSearch(true, false, false);
    }
    handled = true;
  }
  else if(evt->key() == Qt::Key_Escape) {
    clickedCancel();
    handled = true;
  }
  else if ((evt->key() == Qt::Key_D) && (evt->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) == Qt::ControlModifier)
  {
    int rowsDown = (evt->modifiers() & Qt::ShiftModifier) == 0 ? 1 : -1;
    copyCell(rowsDown);
    handled = true;
  }
  else if ((evt->key() == Qt::Key_E) && (evt->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) == Qt::ControlModifier)
  {
    int rowsDown = (evt->modifiers() & Qt::ShiftModifier) == 0 ? 2 : -2;
    copyCell(rowsDown);
    handled = true;
  }
  else if ((evt->key() == Qt::Key_F10) && (evt->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier)) == 0)
  {
    int incValue = (evt->modifiers() & Qt::ShiftModifier) == 0 ? 1 : -1;
    incrementCell(incValue);
    handled = true;
  }
  else if ((evt->key() == Qt::Key_F1) && (evt->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier | Qt::ShiftModifier)) == 0)
  {
    displayHelp();
    handled = true;
  }

  if (!handled) {
    QDialog::keyPressEvent(evt);
  }
}

QModelIndex GenericDataCollectionTableDialog::find(const QString& s, const bool searchForward)
{
  // Determine a start index and go from there!
  qDebug() << "current row (" << m_tableView->currentIndex().row() << ") (" << m_tableView->currentIndex().column() << ")";
  if (m_tableView->currentIndex().row() >= 0)
  {
    return find(s, m_tableView->currentIndex(), searchForward);
  }
  else if (m_tableModel->rowCount() > 0)
  {
    // return find(s, m_proxyModel->createIndex(0, 0), searchForward);
  }

  return QModelIndex();
}

QModelIndex GenericDataCollectionTableDialog::find(const QString& s, const QModelIndex& startIndex, const bool searchForward)
{
  // TODO: Deal with unused parameter searchForward.
  qDebug("In ::find()");
  QModelIndex matchIndex;

  // Supported match flags
  // MatchExactly = 0, Performs QVariant-based matching.
  // MatchContains = 1,
  // MatchStartsWith = 2,
  // MatchEndsWith = 3,
  // MatchRegExp = 4,
  // MatchWildcard = 5,
  // MatchFixedString = 8, Performs string-based matching.
  // MatchCaseSensitive = 16,
  // MatchWrap = 32,
  // MatchRecursive = 64 Searches the entire hierarchy, but I do not have heirarchical data.

  Qt::MatchFlags matchFlags = Qt::MatchWrap | Qt::MatchContains;
  if (startIndex.isValid())
  {
    QModelIndexList list = m_proxyModel->match(startIndex, Qt::DisplayRole, s, 1, matchFlags);
    if (list.count() > 0)
    {
      matchIndex = list.at(0);
      qDebug() << "Found (" << m_proxyModel->data(matchIndex).toString() << ")";
    }
    else
    {
      qDebug() << "Did not find anything (" << s << ") (" << m_proxyModel->data(startIndex).toString() << ")";
    }
  }
  else
  {
    //QModelIndexList list = m_proxyModel->match(m_proxyModel->createIndex(0, 0), Qt::DisplayRole, s, 1, matchFlags);
    //if (list.count() > 0)
    //{
    //  matchIndex = list.at(0);
    //  qDebug(qPrintable(QString("Found (%1)").arg(m_proxyModel->data(matchIndex).toString())));
    //}
  }
//    for (int i=currentRow; i<m_proxyModel->rowCount(); ++i) {
//      QModelIndex currentIndex = m_proxyModel->mapToSource(m_proxyModel->createIndex(i, currentCol));
//    }
  return matchIndex;
}

bool GenericDataCollectionTableDialog::genericSearch(const bool findNext, const bool findPrevious, const bool findDialog)
{
  bool foundSomething = false;

  QScopedPointer<QSettings> pSettings(getQSettings());
  QString dfltFind = pSettings->value(Constants::Settings_SearchFindValue, "").toString();
  QString dfltReplace = pSettings->value(Constants::Settings_SearchReplaceValue, "").toString();
  QString dfltOptions = pSettings->value(Constants::Settings_SearchOptions, "").toString();

  SearchOptions options;
  options.setFindValue(dfltFind);
  options.setReplaceValue(dfltReplace);

  if (!dfltOptions.isEmpty()) {
    options.deserializeSettings(dfltOptions);
  }

  if (findDialog)
  {
    if (m_searchWindow == nullptr)
    {
      // Will be destroyed when the parent object is destroyed.
      m_searchWindow = new GenericDataCollectionTableSearchDialog(this, this);
    }

    m_searchWindow->set(options);
    m_searchWindow->setWindowFlags(m_searchWindow->windowFlags() | Qt::WindowStaysOnTopHint);
    m_searchWindow->show();
    m_searchWindow->raise();
    m_searchWindow->activateWindow();
    //m_searchWindow->setFocus();
    /**
    GenericDataCollectionTableSearchDialog* dlg = new GenericDataCollectionTableSearchDialog(this, this);
    dlg->set(options);
    dlg->setWindowFlags(dlg->windowFlags() | Qt::WindowStaysOnTopHint);
    dlg->show();
    dlg->raise();
    dlg->activateWindow();
    //dlg->setModal(true);
    dlg->setFocus();
    // ??? int rc = dlg->exec();
    int rc = -1;
    if (rc == QDialog::Accepted)
    {
      options = dlg->getOptions();
      pSettings->setValue(Constants::Settings_SearchFindValue, options.getFindValue());
      pSettings->setValue(Constants::Settings_SearchReplaceValue, options.getReplaceValue());
      pSettings->setValue(Constants::Settings_SearchOptions, options.serializeSettings());

      QModelIndexList list = m_proxyModel->search(m_tableView->currentIndex(), options);

      if (list.count() > 0)
      {
        QModelIndex matchIndex = list.at(0);
        qDebug(qPrintable(QString("Found (%1)").arg(m_proxyModel->data(matchIndex).toString())));
        selectCell(matchIndex);
        foundSomething = true;
      }
      else
      {
        qDebug(qPrintable(QString("Did not find anything ")));
      }
    }
    delete dlg;
    ***/
  }
  else
  {
    int currentRow = m_tableView->currentIndex().row();
    int currentColumn = m_tableView->currentIndex().column();

    if (findNext)
    {
      currentRow = (currentRow + 1) < m_proxyModel->rowCount() ? currentRow + 1 : 0;
      if (options.isBackwards())
      {
        options.setBackwards(false);
      }
    }
    else if (findPrevious)
    {
      //currentRow = (currentRow > 0) ? currentRow - 1 : m_proxyModel->rowCount() - 1;
      if (!options.isBackwards())
      {
        options.setBackwards(true);
      }
    }
    else
    {
      qDebug("Expected findNext, findPrevious, or searchDialog to be true");
      return false;
    }

    qDebug() << "Search from row " << currentRow << " col " << currentColumn;
    QModelIndex startIndex = m_proxyModel->index(currentRow, currentColumn);
    QModelIndexList list = m_proxyModel->search(startIndex, options);

    if (list.count() > 0)
    {
      QModelIndex matchIndex = list.at(0);
      qDebug() << "Found (" << m_proxyModel->data(matchIndex).toString() << ")";
      selectCell(matchIndex);
      foundSomething = true;
    }
    else
    {
      qDebug() << "Did not find anything ";
    }
  }
  return foundSomething;
}

bool GenericDataCollectionTableDialog::doFind(const SearchOptions& options, const bool includeCurrent)
{
  if (includeCurrent)
  {
    QModelIndexList list = m_proxyModel->search(m_tableView->currentIndex(), options);

    if (list.count() > 0)
    {
      QModelIndex matchIndex = list.at(0);
      qDebug() << "Found (" << m_proxyModel->data(matchIndex).toString() << ")";
      selectCell(matchIndex);
      return true;
    }
  }
  else
  {
    int currentRow = m_tableView->currentIndex().row();
    int currentColumn = m_tableView->currentIndex().column();
    if (!options.isBackwards())
    {
      currentRow = (currentRow + 1) < m_proxyModel->rowCount() ? currentRow + 1 : 0;
    }
    QModelIndex startIndex = m_proxyModel->index(currentRow, currentColumn);
    QModelIndexList list = m_proxyModel->search(startIndex, options);

    if (list.count() > 0)
    {
      QModelIndex matchIndex = list.at(0);
      qDebug() << "Found (" << m_proxyModel->data(matchIndex).toString() << ")";
      selectCell(matchIndex);
      return true;
    }
  }
  return false;
}


void GenericDataCollectionTableDialog::searchDialog()
{
  genericSearch(false, false, true);
  /***
  qDebug("in ::searchDialog()");
  GenericDataCollectionTableSearchDialog* dlg = new GenericDataCollectionTableSearchDialog(this, nullptr);

  QScopedPointer<QSettings> pSettings(getQSettings());
  QString dfltFind = pSettings->value(Constants::Settings_SearchFindValue, "").toString();
  QString dfltReplace = pSettings->value(Constants::Settings_SearchReplaceValue, "").toString();
  QString dfltOptions = pSettings->value(Constants::Settings_SearchOptions, "").toString();
  SearchOptions options;

  if (!dfltOptions.isEmpty()) {
    options.deserializeSettings(dfltOptions);
    options.setFindValue(dfltFind);
    options.setReplaceValue(dfltReplace);
    dlg->set(options);
  } else {
    dlg->setFindValue(dfltFind);
    dlg->setReplaceValue(dfltReplace);
  }

  int rc = dlg->exec();
  if (rc == QDialog::Accepted)
  {
    options = dlg->getOptions();
    pSettings->setValue(Constants::Settings_SearchFindValue, options.getFindValue());
    pSettings->setValue(Constants::Settings_SearchReplaceValue, options.getReplaceValue());
    pSettings->setValue(Constants::Settings_SearchOptions, options.serializeSettings());

    QModelIndexList list = m_proxyModel->search(m_tableView->currentIndex(), options);

    if (list.count() > 0)
    {
      QModelIndex matchIndex = list.at(0);
      qDebug(qPrintable(QString("Found (%1)").arg(m_proxyModel->data(matchIndex).toString())));
      selectCell(matchIndex);
    }
    else
    {
      qDebug(qPrintable(QString("Did not find anything ")));
    }
  }
  delete dlg;
**/
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


