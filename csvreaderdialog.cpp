#include "csvreaderdialog.h"
#include "constants.h"
#include "csvreader.h"
#include "csvline.h"
#include "globals.h"

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QMap>
#include <QTextEdit>
#include <QRadioButton>
#include <QGridLayout>
#include <QButtonGroup>
#include <QLineEdit>
#include <QTableWidget>
#include <QSettings>
#include <QDebug>
#include <QScopedPointer>

CSVReaderDialog::CSVReaderDialog(CSVReader* reader, QWidget *parent) :
  QDialog(parent),
  m_importGroupBox(nullptr), m_separatorOptionsGroupBox(nullptr), m_fieldsGroupBox(nullptr),
  m_characterSet(nullptr), m_firstRow(nullptr),
  m_separatedMap(nullptr),
  m_otherSeparatorBox(nullptr),
  m_otherSeparator(nullptr),
  m_mergeDelimiters(nullptr),
  m_textDelimiter(nullptr),
  m_fixedWidthRB(nullptr),
  m_separatedByRB(nullptr),
  m_textDelimiterLabel(nullptr),
  m_columnType(nullptr),
  m_columnTypeLabel(nullptr),
  m_tableWidget(nullptr),
  m_reader(reader)
{
  buildDialog();
  QScopedPointer<QSettings> pSettings(getQSettings());
  restoreGeometry(pSettings->value(Constants::Settings_CSVDialogGeometry).toByteArray());
}

CSVReaderDialog::~CSVReaderDialog()
{
  QScopedPointer<QSettings> pSettings(getQSettings());
  pSettings->setValue(Constants::Settings_CSVDialogGeometry, saveGeometry());
  if (m_separatedMap != nullptr)
  {
    delete m_separatedMap;
    m_separatedMap = nullptr;
  }
}

void CSVReaderDialog::buildDialog()
{
  QHBoxLayout *mainLayout = new QHBoxLayout();
  QVBoxLayout *vBox = new QVBoxLayout();

  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

  createImportGroupBox();
  createSeparatorOptionsGroupBox();
  creatFieldsGroupBox();
  vBox->addWidget(m_importGroupBox);
  vBox->addWidget(m_separatorOptionsGroupBox);
  vBox->addWidget(m_fieldsGroupBox);
  mainLayout->addLayout(vBox);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);
}

void CSVReaderDialog::createImportGroupBox()
{
  m_importGroupBox = new QGroupBox(tr("Import"));
  QFormLayout *layout = new QFormLayout;
  QStringList supportedCharacterSets;
  supportedCharacterSets << "Default"
                         << "Apple Roman"
                         << "Big5"
                         << "Big5-HKSCS"
                         << "CP949"
                         << "EUC-JP"
                         << "EUC-KR"
                         << "GB18030-0"
                         << "IBM 850"
                         << "IBM 866"
                         << "IBM 874"
                         << "ISO 2022-JP"
                         << "ISO 8859-1 to 10"
                         << "ISO 8859-13 to 16"
                         << "Iscii-Bng"
                         << "Iscii-Dev"
                         << "Iscii-Gjr"
                         << "Iscii-Knd"
                         << "Iscii-Mlm"
                         << "Iscii-Ori"
                         << "Iscii-Pnj"
                         << "Iscii-Tlg"
                         << "Iscii-Tml"
                         << "JIS X 0201"
                         << "JIS X 0208"
                         << "KOI8-R"
                         << "KOI8-U"
                         << "MuleLao-1"
                         << "ROMAN8"
                         << "Shift-JIS"
                         << "TIS-620"
                         << "TSCII"
                         << "UTF-8"
                         << "UTF-16"
                         << "UTF-16BE"
                         << "UTF-16LE"
                         << "UTF-32"
                         << "UTF-32BE"
                         << "UTF-32LE"
                         << "Windows-1250"
                         << "Windows-1251"
                         << "Windows-1252"
                         << "Windows-1253"
                         << "Windows-1254"
                         << "Windows-1255"
                         << "Windows-1256"
                         << "Windows-1257"
                         << "Windows-1258"
                         << "WINSAMI2";
  m_characterSet = new QComboBox();
  m_characterSet->addItems(supportedCharacterSets);
  layout->addRow(new QLabel(tr("Character set")), m_characterSet);
  int utfIdx = supportedCharacterSets.indexOf("Default");
  if (utfIdx >= 0)
  {
    m_characterSet->setCurrentIndex(utfIdx);
  }

  m_firstRow = new QSpinBox();
  m_firstRow->setToolTip(tr("Row 0 refers to the header record."));
  layout->addRow(new QLabel(tr("From row")), m_firstRow);
  m_importGroupBox->setLayout(layout);
}

void CSVReaderDialog::createSeparatorOptionsGroupBox()
{
  QVBoxLayout* vBoxMain = new QVBoxLayout();

  QGridLayout* grid = new QGridLayout(this);
  QButtonGroup* bg = new QButtonGroup(this);
  m_fixedWidthRB = new QRadioButton(tr("Fixed width"));
  m_separatedByRB = new QRadioButton(tr("Separated by"));
  m_separatedByRB->setChecked(true);
  bg->addButton(m_fixedWidthRB);
  bg->addButton(m_separatedByRB);

  connect(m_separatedByRB, SIGNAL(toggled(bool)), this, SLOT(separatorToggled(bool)));

  m_separatedMap = new QMap<QString, QCheckBox*>();
  m_otherSeparatorBox = new QCheckBox(tr("Other"));
  m_otherSeparator = new QLineEdit();
  m_mergeDelimiters = new QCheckBox(tr("Merge delimiters"));
  m_textDelimiter = new QComboBox();
  m_textDelimiter->addItem("\"");
  m_textDelimiter->addItem("'");

  vBoxMain->addWidget(m_fixedWidthRB);
  vBoxMain->addWidget(m_separatedByRB);

  QCheckBox* cBox = new QCheckBox(tr("Tab"));
  m_separatedMap->insert("\t", cBox);
  grid->addWidget(cBox, 0, 0, Qt::AlignLeft);

  cBox = new QCheckBox(tr("Comma"));
  m_separatedMap->insert(",", cBox);
  cBox->setChecked(true);
  grid->addWidget(cBox, 0, 1, Qt::AlignLeft);

  grid->addWidget(m_otherSeparatorBox, 0, 2, Qt::AlignLeft);
  grid->addWidget(m_otherSeparator, 0, 3, Qt::AlignLeft);

  cBox = new QCheckBox(tr("Semicolon"));
  m_separatedMap->insert(";", cBox);
  grid->addWidget(cBox, 1, 0, Qt::AlignLeft);

  cBox = new QCheckBox(tr("Space"));
  m_separatedMap->insert(" ", cBox);
  grid->addWidget(cBox, 1, 1, Qt::AlignLeft);

  grid->addWidget(m_mergeDelimiters, 2, 0, Qt::AlignLeft);
  m_textDelimiterLabel = new QLabel(tr("Text delimiter"));
  grid->addWidget(m_textDelimiterLabel, 2, 2, Qt::AlignLeft);
  grid->addWidget(m_textDelimiter, 2, 3, Qt::AlignLeft);
  m_textDelimiterLabel->setBuddy(m_textDelimiter);

  m_textDelimiter->setMinimumWidth(125);

  QGroupBox *gb2 = new QGroupBox();
  gb2->setLayout(grid);
  vBoxMain->addWidget(gb2);

  m_separatorOptionsGroupBox = new QGroupBox(tr("Separator"));
  m_separatorOptionsGroupBox->setLayout(vBoxMain);

}

void CSVReaderDialog::separatorToggled(bool enabled)
{
  QMapIterator<QString, QCheckBox*> i(*m_separatedMap);
  while (i.hasNext()) {
    i.next();
    i.value()->setEnabled(enabled);
  }

  m_otherSeparatorBox->setEnabled(enabled);
  m_otherSeparator->setEnabled(enabled);
  m_mergeDelimiters->setEnabled(enabled);
  m_textDelimiterLabel->setEnabled(enabled);
  m_textDelimiter->setEnabled(enabled);
}

void CSVReaderDialog::tableCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
  qDebug() << qPrintable(QString("(%1,%2)(%3,%4)").arg(currentRow).arg(currentColumn).arg(previousRow).arg(previousColumn));
  if (m_reader != nullptr)
  {
    // TODO: Add a listener to the drop down to push type changes back to the reader.
    // TODO: Track these changes as we move from one to another.
    //QMetaType::Type colType = m_reader->guessColumnType(currentColumn);
    QMetaType::Type colType = m_FieldType[currentColumn];
    if (!m_typeToTxt.contains(colType))
    {
      m_columnType->setCurrentIndex(0);
    }
    else
    {
      const QStringList& list = m_typeToTxt.value(colType);
      m_columnType->setCurrentIndex(m_typeNames.indexOf(list.first()));
    }
  }
}

void CSVReaderDialog::columnTypeChanged(const QString& text)
{
  //int iCol = getCurrentColumn();
  //?? Save the type for the column if it is a known type.
  qDebug() << qPrintable(QString("New type %1 for column %2").arg(text).arg(getCurrentColumn()));
}

int CSVReaderDialog::getCurrentColumn() const
{
  return (m_tableWidget != nullptr) ? m_tableWidget->currentColumn() : -1;
}

void CSVReaderDialog::tableSelectionChanged()
{
  qDebug() << "Table selection changed";
  QList<QTableWidgetSelectionRange> ranges = m_tableWidget->selectedRanges();
}


void CSVReaderDialog::creatFieldsGroupBox()
{
  QList<QMetaType::Type> typeList;
  typeList << QMetaType::Bool;
  m_txtToType.insert("Boolean", typeList);
  typeList.clear();

  typeList << QMetaType::Short;
  typeList << QMetaType::Int;
  m_txtToType.insert("Integer", typeList);
  typeList.clear();

  typeList << QMetaType::Long;
  typeList << QMetaType::LongLong;
  m_txtToType.insert("Long Integer", typeList);
  typeList.clear();

  typeList << QMetaType::UShort;
  typeList << QMetaType::UInt;
  m_txtToType.insert("Unsigned Integer", typeList);
  typeList.clear();

  typeList << QMetaType::ULong;
  typeList << QMetaType::ULongLong;
  m_txtToType.insert("Unsigned Long Integer", typeList);
  typeList.clear();

  typeList << QMetaType::QDate;
  m_txtToType.insert("Date", typeList);
  typeList.clear();

  typeList << QMetaType::QDateTime;
  m_txtToType.insert("Date / Time", typeList);
  typeList.clear();

  typeList << QMetaType::QTime;
  m_txtToType.insert("Time", typeList);
  typeList.clear();

  typeList << QMetaType::Float;
  typeList << QMetaType::Double;
  m_txtToType.insert("Float", typeList);
  typeList.clear();

  QStringList sList;
  sList << "Boolean";
  m_typeToTxt.insert(QMetaType::Bool, sList);
  sList.clear();

  sList << "Integer";
  m_typeToTxt.insert(QMetaType::Short, sList);
  m_typeToTxt.insert(QMetaType::Int, sList);
  sList.clear();

  sList << "Unsigned Integer";
  m_typeToTxt.insert(QMetaType::UShort, sList);
  m_typeToTxt.insert(QMetaType::UInt, sList);
  sList.clear();


  sList << "Long Integer";
  m_typeToTxt.insert(QMetaType::Long, sList);
  m_typeToTxt.insert(QMetaType::LongLong, sList);
  sList.clear();

  sList << "Unsigned Long Integer";
  m_typeToTxt.insert(QMetaType::ULong, sList);
  m_typeToTxt.insert(QMetaType::ULongLong, sList);
  sList.clear();

  sList << "Date";
  m_typeToTxt.insert(QMetaType::QDate, sList);
  sList.clear();

  sList << "Date / Time";
  m_typeToTxt.insert(QMetaType::QDateTime, sList);
  sList.clear();

  sList << "Time";
  m_typeToTxt.insert(QMetaType::QTime, sList);
  sList.clear();

  sList << "Float";
  m_typeToTxt.insert(QMetaType::Float, sList);
  m_typeToTxt.insert(QMetaType::Double, sList);
  sList.clear();


  m_typeNames << "Text";
  m_typeNames << "Boolean";
  m_typeNames << "Integer";
  m_typeNames << "Unsigned Integer";
  m_typeNames << "Long Integer";
  m_typeNames << "Unsigned Long Integer";
  m_typeNames << "Date";
  m_typeNames << "Time";
  m_typeNames << "Date / Time";
  m_typeNames << "Float";

  m_fieldsGroupBox = new QGroupBox(tr("Fields"));
  QVBoxLayout *vBox = new QVBoxLayout();
  QFormLayout *typeRow = new QFormLayout();
  m_columnTypeLabel = new QLabel(tr("Column type"));
  m_columnType = new QComboBox();
  m_columnTypeLabel->setBuddy(m_columnType);

  for (int i=0; i<m_typeNames.size(); ++i)
  {
    m_columnType->addItem(m_typeNames[i]);
  }

  /*
  m_columnType->addItem(tr("Date (MDY)"));
  m_columnType->addItem(tr("Date (DMY)"));
  m_columnType->addItem(tr("String"));
  m_columnType->addItem(tr("Integer"));
  m_columnType->addItem(tr("Float"));
  m_columnType->addItem(tr("Standard"));
  */

  typeRow->addRow(m_columnTypeLabel, m_columnType);
  vBox->addLayout(typeRow);
  configureFromReader(vBox);

  m_fieldsGroupBox->setLayout(vBox);
  if (m_tableWidget != nullptr)
  {
    connect(m_tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(tableSelectionChanged()));
    connect(m_tableWidget, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(tableCellChanged(int, int, int, int)));
  }
  if (m_columnType != nullptr)
  {
    connect(m_columnType, SIGNAL(currentTextChanged(QString)), this, SLOT(columnTypeChanged(QString)));
  }
}


void CSVReaderDialog::configureFromReader(QVBoxLayout* vBoxLayout)
{
  bool addToWidget = false;
  if (m_reader != nullptr)
  {
    if (m_tableWidget == nullptr)
    {
      m_tableWidget = new QTableWidget(m_reader->countLines(), m_reader->countHeaderColumns());
      addToWidget = (vBoxLayout != nullptr);
    }
    else
    {
      m_tableWidget->setRowCount(0);
      m_tableWidget->setColumnCount(0);
    }

    m_tableWidget->setRowCount(m_reader->countLines());
    m_tableWidget->setColumnCount(m_reader->countHeaderColumns());

    m_tableWidget->setHorizontalHeaderLabels(m_reader->getHeader().toStringList());
    for (int row=0; row<m_reader->countLines(); ++row)
    {
      const CSVLine& line = m_reader->getLine(row);
      for (int col=0; col<line.size(); ++col)
      {
        QTableWidgetItem *newItem = new  QTableWidgetItem(line[col].getValue());
        m_tableWidget->setItem(row, col, newItem);
      }
    }
    if (addToWidget){
      vBoxLayout->addWidget(m_tableWidget);
    }

    // Set the expected types.
    m_FieldType.clear();
    for (int i=0; i<m_reader->countHeaderColumns(); ++i)
    {
      // TODO: Set to use signed types rather than unsigned?
      m_FieldType.append(m_reader->guessColumnType(i));
    }
  }
}

