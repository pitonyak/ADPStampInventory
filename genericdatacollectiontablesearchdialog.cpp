#include "genericdatacollectiontablesearchdialog.h"
#include "genericdatacollectiontabledialog.h"
#include "genericdatacollectionstablemodel.h"
#include "constants.h"

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
#include <QRadioButton>
#include <QGroupBox>
#include <QSettings>

GenericDataCollectionTableSearchDialog::GenericDataCollectionTableSearchDialog(GenericDataCollectionTableDialog *tableDialog, QWidget *parent) :
    QDialog(parent), m_tableDialog(tableDialog),
    m_findValueLineEdit(nullptr), m_replaceValueLineEdit(nullptr), m_matchCaseCB(nullptr), m_WrapCB(nullptr),
    m_Backwards(nullptr), m_AllColumns(nullptr),
    m_rbMatchAll(nullptr), m_rbContains(nullptr), m_rbStartsWith(nullptr), m_rbEndsWith(nullptr),
    m_rbString(nullptr), m_rbRegExp(nullptr), m_rbWildCard(nullptr)
{
  if (m_tableDialog != nullptr)
  {
    buildDialog();
  }
}

void GenericDataCollectionTableSearchDialog::buildDialog()
{
  setWindowTitle(tr("Search Table"));

  QPushButton* button;
  QVBoxLayout *vLayout;
  QVBoxLayout *vLayout2;
  QFormLayout *fLayout = new QFormLayout;

  vLayout = new QVBoxLayout();
  m_findValueLineEdit = new QLineEdit();
  m_replaceValueLineEdit = new QLineEdit();

  vLayout->addWidget(new QLabel(tr("Search Text")));
  button = new QPushButton(tr("&Find"));
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
  m_WrapCB = new QCheckBox(tr("Wrap"));
  vLayout2->addWidget(m_WrapCB);
  hLayout2->addLayout(vLayout2);

  vLayout2 = new QVBoxLayout();
  m_Backwards = new QCheckBox(tr("Search Previous"));
  vLayout2->addWidget(m_Backwards);
  m_AllColumns = new QCheckBox(tr("All Columns"));
  vLayout2->addWidget(m_AllColumns);
  hLayout2->addLayout(vLayout2);

  vLayout->addLayout(hLayout2);

  QGroupBox *groupBox = new QGroupBox(tr("Match Type"));

  QVBoxLayout *vLayoutRBMatchExtent = new QVBoxLayout();
  m_rbMatchAll = new QRadioButton("Match All");
  m_rbContains = new QRadioButton("Contains");
  m_rbStartsWith = new QRadioButton("Starts With");
  m_rbEndsWith = new QRadioButton("Ends With");
  vLayoutRBMatchExtent->addWidget(m_rbMatchAll);
  vLayoutRBMatchExtent->addWidget(m_rbContains);
  vLayoutRBMatchExtent->addWidget(m_rbStartsWith);
  vLayoutRBMatchExtent->addWidget(m_rbEndsWith);
  groupBox->setLayout(vLayoutRBMatchExtent);

  hLayout2 = new QHBoxLayout();
  hLayout2->addWidget(groupBox);

  QVBoxLayout *vLayoutRBMatchType = new QVBoxLayout();
  groupBox = new QGroupBox(tr("How to search"));
  m_rbString = new QRadioButton("String");
  m_rbRegExp = new QRadioButton("Regular Expression");
  m_rbWildCard = new QRadioButton("Wild Card");
  vLayoutRBMatchType->addWidget(m_rbString);
  vLayoutRBMatchType->addWidget(m_rbRegExp);
  vLayoutRBMatchType->addWidget(m_rbWildCard);
  groupBox->setLayout(vLayoutRBMatchType);

  hLayout2->addWidget(groupBox);
  vLayout->addLayout(hLayout2);

  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(okPressed()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  vLayout->addWidget(buttonBox);

  fLayout->addRow(vLayout);
  setLayout(fLayout);
  focusFindField();
  enableButtons();
}

void GenericDataCollectionTableSearchDialog::focusFindField()
{
  QMetaObject::invokeMethod(m_findValueLineEdit, "setFocus", Qt::QueuedConnection);
  QMetaObject::invokeMethod(m_findValueLineEdit, "selectAll", Qt::QueuedConnection);
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

void GenericDataCollectionTableSearchDialog::setCaseSensitive(const bool b)
{
  if (m_matchCaseCB != nullptr)
  {
    m_matchCaseCB->setChecked(b);
  }
}

bool GenericDataCollectionTableSearchDialog::isCaseSensitive() const
{
  return (m_matchCaseCB != nullptr && m_matchCaseCB->isChecked());
}


QString GenericDataCollectionTableSearchDialog::getFindValue() const
{
  return (m_findValueLineEdit != nullptr) ? m_findValueLineEdit->text() : "";
}

void GenericDataCollectionTableSearchDialog::setFindValue(const QString& s)
{
  if (m_findValueLineEdit != nullptr)
  {
    m_findValueLineEdit->setText(s);
  }
}

QString GenericDataCollectionTableSearchDialog::getReplaceValue() const
{
  return (m_replaceValueLineEdit != nullptr) ? m_replaceValueLineEdit->text() : "";
}

void GenericDataCollectionTableSearchDialog::setReplaceValue(const QString& s)
{
  if (m_replaceValueLineEdit != nullptr)
  {
    m_replaceValueLineEdit->setText(s);
  }
}

bool GenericDataCollectionTableSearchDialog::isMatchAll() const
{
  return (m_rbMatchAll != nullptr) ? m_rbMatchAll->isChecked() : false;
}

bool GenericDataCollectionTableSearchDialog::isContains() const
{
  return (m_rbContains != nullptr) ? m_rbContains->isChecked() : false;
}

bool GenericDataCollectionTableSearchDialog::isStartsWith() const
{
  return (m_rbStartsWith != nullptr) ? m_rbStartsWith->isChecked() : false;
}

bool GenericDataCollectionTableSearchDialog::isEndsWith() const
{
  return (m_rbEndsWith != nullptr) ? m_rbEndsWith->isChecked() : false;
}

bool GenericDataCollectionTableSearchDialog::isMatchAsString() const
{
  return (m_rbString != nullptr) ? m_rbString->isChecked() : false;
}

bool GenericDataCollectionTableSearchDialog::isRegularExpression() const
{
  return (m_rbRegExp != nullptr) ? m_rbRegExp->isChecked() : false;
}

bool GenericDataCollectionTableSearchDialog::isWildCard() const
{
  return (m_rbWildCard != nullptr) ? m_rbWildCard->isChecked() : false;
}

bool GenericDataCollectionTableSearchDialog::isWrap() const
{
  return (m_WrapCB != nullptr) ? m_WrapCB->isChecked() : false;
}

bool GenericDataCollectionTableSearchDialog::isBackwards() const
{
  return (m_Backwards != nullptr) ? m_Backwards->isChecked() : false;
}

bool GenericDataCollectionTableSearchDialog::isAllColumns() const
{
  return (m_AllColumns != nullptr) ? m_AllColumns->isChecked() : false;
}


void GenericDataCollectionTableSearchDialog::setWrap(const bool b)
{
  if (m_WrapCB != nullptr) m_WrapCB->setChecked(b);
}

void GenericDataCollectionTableSearchDialog::setBackwards(const bool b)
{
  if (m_Backwards != nullptr) m_Backwards->setChecked(b);
}

void GenericDataCollectionTableSearchDialog::setAllColumns(const bool b)
{
  if (m_AllColumns != nullptr) m_AllColumns->setChecked(b);
}

void GenericDataCollectionTableSearchDialog::setMatchAll(const bool b)
{
  if (m_rbMatchAll != nullptr) m_rbMatchAll->setChecked(b);
}

void GenericDataCollectionTableSearchDialog::setContains(const bool b)
{
  if (m_rbContains != nullptr) m_rbContains->setChecked(b);
}

void GenericDataCollectionTableSearchDialog::setStartsWith(const bool b)
{
  if (m_rbStartsWith != nullptr) m_rbStartsWith->setChecked(b);
}

void GenericDataCollectionTableSearchDialog::setEndsWith(const bool b)
{
  if (m_rbEndsWith != nullptr) m_rbEndsWith->setChecked(b);
}

void GenericDataCollectionTableSearchDialog::setMatchAsString(const bool b)
{
  if (m_rbString != nullptr) m_rbString->setChecked(b);
}

void GenericDataCollectionTableSearchDialog::setRegularExpression(const bool b)
{
  if (m_rbRegExp != nullptr) m_rbRegExp->setChecked(b);
}

void GenericDataCollectionTableSearchDialog::setWildCard(const bool b)
{
  if (m_rbWildCard != nullptr) m_rbWildCard->setChecked(b);
}

void GenericDataCollectionTableSearchDialog::set(const SearchOptions& options)
{
  setMatchAll(options.isMatchEntireString());
  setMatchAsString(options.isMatchAsString());
  setCaseSensitive(options.isCaseSensitive());
  setBackwards(options.isBackwards());
  setAllColumns(options.isAllColumns());
  setWrap(options.isWrap());
  setContains(options.isContains());
  setStartsWith(options.isStartsWith());
  setEndsWith(options.isEndsWith());
  setRegularExpression(options.isRegularExpression());
  setWildCard(options.isWildCard());

  setFindValue(options.getFindValue());
  setReplaceValue(options.getReplaceValue());
}

SearchOptions GenericDataCollectionTableSearchDialog::getOptions() const
{
  SearchOptions options;
  if (isMatchAll()) options.setMatchEntireString();
  if (isMatchAsString()) options.setMatchAsString();

  options.setCaseSensitive(isCaseSensitive());
  options.setBackwards(isBackwards());
  options.setAllColumns(isAllColumns());
  options.setWrap(isWrap());

  if (isContains()) options.setContains();
  if (isStartsWith()) options.setStartsWith();
  if (isEndsWith()) options.setEndsWith();
  if (isRegularExpression()) options.setRegularExpression();
  if (isWildCard()) options.setWildCard();

  options.setFindValue(getFindValue());
  options.setReplaceValue(getReplaceValue());
  return options;
}


#include "genericdatacollectionstableproxy.h"

void GenericDataCollectionTableSearchDialog::find()
{
  saveSettings();
  if (m_tableDialog != nullptr)
  {
    m_tableDialog->doFind(getOptions(), false);
  }

  // I think that this works from here ****
  /**
  QString searchString = m_findValueLineEdit->text();

  GenericDataCollectionsTableProxy* pm = m_tableDialog->getProxyModel();
  if (pm != nullptr)
  {
    pm->setFilterCaseSensitivity(Qt::CaseInsensitive);
    pm->setFilterKeyColumn(4);
    pm->setFilterFixedString(searchString);
  }
  ***/
  // To Here *********

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

void GenericDataCollectionTableSearchDialog::replace()
{

}

void GenericDataCollectionTableSearchDialog::replaceAll()
{

}

void GenericDataCollectionTableSearchDialog::saveSettings() const
{
  QSettings settings;
  SearchOptions options = getOptions();
  settings.setValue(Constants::Settings_SearchFindValue, options.getFindValue());
  settings.setValue(Constants::Settings_SearchReplaceValue, options.getReplaceValue());
  settings.setValue(Constants::Settings_SearchOptions, options.serializeSettings());
}

void GenericDataCollectionTableSearchDialog::restoreSettings()
{
  QSettings settings;
  SearchOptions options;
  options.setFindValue(settings.value(Constants::Settings_SearchFindValue, "").toString());
  options.setReplaceValue(settings.value(Constants::Settings_SearchReplaceValue, "").toString());

  QString dfltOptions = settings.value(Constants::Settings_SearchOptions, "").toString();
  if (!dfltOptions.isEmpty()) {
    options.deserializeSettings(dfltOptions);
  }
  set(options);
}

void GenericDataCollectionTableSearchDialog::okPressed()
{
  // doFind() saves the settings
  // saveSettings();
  // Change the behavior when we actually support replace.
  find();
  accept();
}
