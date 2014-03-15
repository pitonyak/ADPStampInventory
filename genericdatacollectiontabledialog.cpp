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

GenericDataCollectionTableDialog::GenericDataCollectionTableDialog(const QString& name, GenericDataCollection &data, QWidget *parent) :
  QDialog(parent), m_dataCollection(data), m_tableView(nullptr), m_name(name), m_tableModel(nullptr)
{
  buildDialog();
}


GenericDataCollectionTableDialog::~GenericDataCollectionTableDialog()
{
  // save the dialog state.
  QSettings settings;
  settings.setValue(Constants::Constants::Settings_GenericDataCollectionDlgGeometry, saveGeometry());

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

  // For all columns that are boolean, use this delegate.
  CheckBoxOnlyDelegate * cboDelegate = nullptr;
  for (int i=m_dataCollection.getPropertyNameCount() - 1; i>=0; --i) {
    if (m_dataCollection.getPropertyTypeMeta(i) == QMetaType::Bool) {
      if (cboDelegate == nullptr) {
        cboDelegate = new CheckBoxOnlyDelegate(m_tableView);
      }
      m_tableView->setItemDelegateForColumn(i, cboDelegate);
    }
  }

  vLayout = new QVBoxLayout();
  vLayout->addWidget(m_tableView, 0, 0);


  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  vLayout->addWidget(buttonBox);

  setLayout(vLayout);


  QSettings settings;
  restoreGeometry(settings.value(Constants::Settings_GenericDataCollectionDlgGeometry).toByteArray());
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
  //connect(m_tableView->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)), this, SLOT(selectionChanged(const QItemSelection &, const QItemSelection &)));
  //enableButtons();
}
