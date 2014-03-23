#include "genericdatacollectiontabledialog.h"
#include "genericdatacollectiontablemodel.h"
#include "linkbackfilterdelegate.h"
#include "checkboxonlydelegate.h"
#include "constants.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTableView>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QSettings>
#include <QItemSelection>

GenericDataCollectionTableDialog::GenericDataCollectionTableDialog(const QString& name, GenericDataCollection &data, QWidget *parent) :
  QDialog(parent),
  m_duplicateButton(nullptr), m_addButton(nullptr), m_deleteButton(nullptr), m_undoButton(nullptr),
  m_dataCollection(data), m_tableView(nullptr), m_name(name), m_tableModel(nullptr)
{
  buildDialog();
}

GenericDataCollectionTableDialog::~GenericDataCollectionTableDialog()
{
  // save the dialog state.
  QSettings settings;
  settings.setValue(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgGeometry).arg(m_name), saveGeometry());

    if (m_tableView != nullptr)
    {
      QString s;
      for (int i=0; i<m_dataCollection.getPropertyNameCount(); ++i)
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
      settings.setValue(QString("%1_%2").arg(Constants::Settings_GenericDataCollectionDlgColumnWidths).arg(m_name), s);
    }
}

void GenericDataCollectionTableDialog::buildDialog()
{
  setWindowTitle(tr("Edit Table %1").arg(m_name));
  QVBoxLayout *vLayout;
  QHBoxLayout *hLayout;

  // Build the primary table.
  m_tableView = new QTableView();
  m_tableModel = new GenericDataCollectionTableModel(m_dataCollection);
  m_tableView->setModel(m_tableModel);

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
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
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
  hLayout->addWidget(buttonBox);

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
}
