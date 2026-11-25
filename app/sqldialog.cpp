#include "sqldialog.h"
#include "constants.h"
#include "globals.h"
#include "scrollmessagebox.h"
#include "stampdb.h"
#include "csvwriter.h"

#include <QDialogButtonBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScopedPointer>
#include <QSettings>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStatusBar>
#include <QTableWidget>
#include <QTextBlock>
#include <QTextCursor>
#include <QVBoxLayout>

#include <QApplication>
#include <QClipboard>
#include <QKeySequence>
#include <QShortcut>
#include <set>

void setupTableCopyHandler(QTableWidget* table) {

    auto* copyShortcut = new QShortcut(QKeySequence::Copy, table);
    copyShortcut->setContext(Qt::WidgetShortcut); // Only triggers when table has focus

    QObject::connect(copyShortcut, &QShortcut::activated, table, [table]() {
        const QList<QTableWidgetSelectionRange> ranges = table->selectedRanges();
        if (ranges.isEmpty()) {
            return;
        }

        QStringList headers;
        for (int col = 0; col < table->columnCount(); ++col) {
            QTableWidgetItem* headerItem = table->horizontalHeaderItem(col);
            if (headerItem) {
                headers.append(headerItem->text());
            } else {
                // Fallback to the underlying model's header data (e.g., default "1", "2", etc.)
                headers.append(table->model()->headerData(col, Qt::Horizontal).toString());
            }
        }

        QString csvString;
        CSVWriter writer;
        writer.setStreamWriteToString(&csvString);
        // CSV Headers
        CSVLine headerLine;
        QStringList fieldNames;
        QStringList intHeader = {"quantity", "id"};
        QStringList dateHeader = {"releasedate", "purchasedate"};
        QStringList dateTimeHeader = {"updated"};
        QStringList floatHeader = {"facevalue", "paid", "bookvalue",
                                   "valuemultiplier"};
        // Collect unique, sorted row and column indices across all selection ranges
        std::set<int> rows;
        std::set<int> cols;
        for (const auto& range : ranges) {
            for (int r = range.topRow(); r <= range.bottomRow(); ++r) {
                rows.insert(r);
            }
            for (int c = range.leftColumn(); c <= range.rightColumn(); ++c) {
                cols.insert(c);
                QString fieldName = headers[c];
                if (intHeader.contains(fieldName)) {
                    headerLine.append(CSVColumn(fieldName, false, QMetaType::Long));
                } else if (floatHeader.contains(fieldName)) {
                    headerLine.append(CSVColumn(fieldName, false, QMetaType::Float16));
                } else if (dateHeader.contains(fieldName)) {
                    headerLine.append(CSVColumn(fieldName, false, QMetaType::QDate));
                } else if (dateTimeHeader.contains(fieldName)) {
                    headerLine.append(CSVColumn(fieldName, false, QMetaType::QDateTime));
                } else {
                    headerLine.append(CSVColumn(fieldName, true, QMetaType::QString));
                }
            }
        }
        if (cols.size() > 1) {
            writer.setHeader(headerLine);
            writer.writeHeader();
        }

        for (auto rIt = rows.begin(); rIt != rows.end(); ++rIt) {
            CSVLine line;
            for (auto cIt = cols.begin(); cIt != cols.end(); ++cIt) {
                if (const QTableWidgetItem* item = table->item(*rIt, *cIt)) {
                    QString fieldName = headers[*cIt];
                    QString fieldValue = item->text();
                    if (intHeader.contains(fieldName)) {
                        line.append(CSVColumn(fieldValue, false, QMetaType::Long));
                    } else if (floatHeader.contains(fieldName)) {
                        line.append(CSVColumn(fieldValue, false, QMetaType::Float16));
                    } else if (dateHeader.contains(fieldName)) {
                        line.append(CSVColumn(fieldValue, false, QMetaType::QDate));
                    } else if (dateTimeHeader.contains(fieldName)) {
                        line.append(CSVColumn(fieldValue, false, QMetaType::QDateTime));
                    } else {
                        line.append(CSVColumn(fieldValue, true, QMetaType::QString));
                    }
                }
            }
            writer.write(line);
        }

        QApplication::clipboard()->setText(csvString);
    });
}


SQLDialog::SQLDialog(StampDB &db, QWidget *parent)
    : QDialog(parent), m_db(db), m_textEdit(nullptr), m_tableWidget(nullptr),
      m_statusBar(nullptr) {

  QScopedPointer<QSettings> pSettings(getQSettings());

  QVBoxLayout *vBox = new QVBoxLayout();
  QDialogButtonBox *buttonBox =
      new QDialogButtonBox(QDialogButtonBox::Close, Qt::Horizontal);

  // Sadly, new lines are removed when the string is restored.... very sad!
  QStringList sqlList =
      pSettings->value(Constants::Settings_LastSQLDialogString, "")
          .toString()
          .split("\n");
  m_textEdit = new QPlainTextEdit();

  for (int i = 0; i < sqlList.size(); ++i) {
    if (i > 0) {
      m_textEdit->appendPlainText(sqlList[i]);
    } else {
      m_textEdit->setPlainText(sqlList[i]);
    }
  }

  vBox->addWidget(m_textEdit);

  QPushButton *sqlExecuteButton = new QPushButton(tr("&Execute SQL"));
  buttonBox->addButton(sqlExecuteButton, QDialogButtonBox::ActionRole);

  connect(sqlExecuteButton, SIGNAL(clicked()), this, SLOT(sqlButtonPressed()));
  // connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(closeButtonPressed()));
  vBox->addWidget(buttonBox);

  m_tableWidget = new QTableWidget(1, 1);
  vBox->addWidget(m_tableWidget);
  setupTableCopyHandler(m_tableWidget);

  m_statusBar = new QStatusBar();
  vBox->addWidget(m_statusBar);

  setLayout(vBox);
  restoreGeometry(
      pSettings->value(Constants::Settings_SQLDialogGeometry).toByteArray());

  m_statusBar->showMessage(tr("Ready!"));
}

SQLDialog::~SQLDialog() {
  QScopedPointer<QSettings> pSettings(getQSettings());
  pSettings->setValue(Constants::Settings_SQLDialogGeometry, saveGeometry());
  pSettings->setValue(Constants::Settings_LastSQLDialogString, getSqlText());
}

QString SQLDialog::getSqlText() const {
  return (m_textEdit != nullptr) ? m_textEdit->toPlainText() : "";
}

void SQLDialog::closeButtonPressed() { accept(); }

void SQLDialog::sqlButtonPressed() {
  QTextCursor cursor = m_textEdit->textCursor();
  if (cursor.hasComplexSelection()) {
    // TODO: Error? Iterate over selections
    // ScrollMessageBox::information(this, "Simple Selection",
    // cursor.selectedText());
    ScrollMessageBox::information(this, "Complex Selection", "??");
  } else if (cursor.hasSelection()) {
    executeSql(cursor.selectedText());
  } else {
    executeSql(cursor.block().text());
  }
}

void SQLDialog::executeSql(const QString &sqlString) {
  QSqlDatabase &db = m_db.getDB();
  QSqlQuery query(db);
  if (!query.exec(sqlString)) {
    m_statusBar->showMessage(query.lastError().text());
  } else {
    if (query.isSelect()) {
      if (m_tableWidget == nullptr) {
        m_statusBar->showMessage(
            tr("Select not displayed because table widget is null"));
      } else {
        QSqlRecord rec = query.record();

        int numCols = rec.count();
        int numRows = query.size() + 1;

        m_tableWidget->clear();
        m_tableWidget->setColumnCount(numCols);
        m_tableWidget->setRowCount(numRows);

        QStringList fieldNames;
        int i;
        for (i = 0; i < rec.count(); ++i) {
          fieldNames << rec.fieldName(i);
        }
        m_tableWidget->setHorizontalHeaderLabels(fieldNames);

        int row = 0;
        while (query.isActive() && query.next()) {
          for (int col = 0; col < numCols; ++col) {
            QTableWidgetItem *newItem =
                new QTableWidgetItem(query.value(col).toString());
            if (numRows <= row) {
              numRows = row + 1;
              m_tableWidget->setRowCount(numRows);
            }
            m_tableWidget->setItem(row, col, newItem);
          }
          ++row;
          // ScrollMessageBox::information(this, "Got a row!", QString("Have row
          // %1 and size is %2").arg(row).arg(query.size()));
        }
        m_tableWidget->resizeColumnsToContents();
        m_statusBar->showMessage(QString(tr("Read %1 records")).arg(row));
      }
    } else {
      int numAffected = query.numRowsAffected();
      if (numAffected >= 0) {
        m_statusBar->showMessage(
            QString(tr("Successful query with %1 rows affected"))
                .arg(numAffected));
      } else {
        m_statusBar->showMessage(QString(tr("Successful query!")));
      }
    }
    // Handle things here!
  }
}

