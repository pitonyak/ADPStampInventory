#include "tablesortfielddialog.h"
#include "constants.h"

#include <QSettings>

TableSortFieldDialog::TableSortFieldDialog(const GenericDataCollection *dataCollection, QWidget *parent) :
  QDialog(parent), m_dataCollection(dataCollection)
{
}

TableSortFieldDialog::~TableSortFieldDialog()
{
  QSettings settings;
  settings.setValue(Constants::Settings_SortFieldDlgGeometry, saveGeometry());
}

void TableSortFieldDialog::initialize()
{
#if 0
  ui->setupUi(this);
  ui->componentTableView->setModel(&m_messageComponentTableModel);
  LinkBackFilterDelegate* delegate = new LinkBackFilterDelegate(ui->componentTableView);
  ui->componentTableView->setItemDelegate(delegate);
  ui->componentTableView->setColumnWidth(m_messageComponentTableModel.fieldColumn, 240);
  ui->componentTableView->setColumnWidth(m_messageComponentTableModel.stringColumn, 240);

  connect(ui->componentTableView->selectionModel(), SIGNAL(currentChanged ( const QModelIndex &, const QModelIndex &)), this, SLOT(currentMessageCategoryRowChanged ( const QModelIndex &, const QModelIndex &)));
  connect(ui->copyCompButton, SIGNAL(clicked(bool)), this, SLOT( copyMessageCategory()));
  connect(ui->insertCompButton, SIGNAL(clicked(bool)), this, SLOT(insertMessageCategory()));
  connect(ui->upCompButton, SIGNAL(clicked(bool)), this, SLOT(upMessageCategory()));
  connect(ui->downCompButton, SIGNAL(clicked(bool)), this, SLOT(downMessageCategory()));
  connect(ui->delCompButton, SIGNAL(clicked(bool)), this, SLOT(delMessageCategory()));

  connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(closeRequested()));
  connect(ui->testButton, SIGNAL(clicked(bool)), this, SLOT(testMessage()));

  ui->locCaseSensitive->addItem(tr("Case Sensitive"), Qt::CaseSensitive);
  ui->locCaseSensitive->addItem(tr("Case Insensitive"), Qt::CaseInsensitive);
  ui->locCaseSensitive->setToolTip(tr("Set location regular expression case sensitivity."));

  ui->msgCaseSensitive->addItem(tr("Case Sensitive"), Qt::CaseSensitive);
  ui->msgCaseSensitive->addItem(tr("Case Insensitive"), Qt::CaseInsensitive);
  ui->msgCaseSensitive->setToolTip(tr("Set location regular expression case sensitivity."));

  ui->locSyntax->addItem(tr("Perl No Greedy"), QRegExp::RegExp);
  ui->locSyntax->addItem(tr("Perl Greedy"), QRegExp::RegExp2);
  ui->locSyntax->addItem(tr("Wildcard"), QRegExp::Wildcard);
  ui->locSyntax->addItem(tr("Wildcard Unix"), QRegExp::WildcardUnix);
  ui->locSyntax->addItem(tr("String"), QRegExp::FixedString);
  ui->locSyntax->addItem(tr("W3C XML"), QRegExp::W3CXmlSchema11);
  ui->locSyntax->setToolTip(tr("Set location regular expression pattern syntax."));

  ui->msgSyntax->addItem(tr("Perl No Greedy"), QRegExp::RegExp);
  ui->msgSyntax->addItem(tr("Perl Greedy"), QRegExp::RegExp2);
  ui->msgSyntax->addItem(tr("Wildcard"), QRegExp::Wildcard);
  ui->msgSyntax->addItem(tr("Wildcard Unix"), QRegExp::WildcardUnix);
  ui->msgSyntax->addItem(tr("String"), QRegExp::FixedString);
  ui->msgSyntax->addItem(tr("W3C XML"), QRegExp::W3CXmlSchema11);
  ui->locSyntax->setToolTip(tr("Set unformatted message regular expression pattern syntax."));

  ui->locRegExpEdit->setToolTip(tr("Match location filename:line_number must match the regular expression; use to filter by filename or line number."));
  ui->msgRegExpEdit->setToolTip(tr("Match based on the unformatted message text."));

  ui->nameEdit->setToolTip(tr("User recognizable name for this object to pick it out in a list."));
#endif
  enableButtons();
}

int TableSortFieldDialog::getSelectedRow() const
{
  //return ui->componentTableView->currentIndex().row();
  return 0;
}

void TableSortFieldDialog::enableButtons()
{
  //int count = m_messageComponentTableModel.messageComponentCount();
  //int row = count > 0 ? getSelectedCatRow() : -1;
  //bool b = count > 0 && row >= 0;
  //ui->copyCompButton->setEnabled(b);
  //ui->delCompButton->setEnabled(b);
  //ui->upCompButton->setEnabled(b && row > 0);
  //ui->downCompButton->setEnabled(b && row < (count - 1));
}

bool TableSortFieldDialog::isRowSelected() const
{
  //return m_messageComponentTableModel.messageComponentCount() > 0 && ui->componentTableView->currentIndex().row() >= 0;
  return 0;
}
