#ifndef SQLDIALOG_H
#define SQLDIALOG_H

#include <QDialog>

class QPlainTextEdit;
class QTableWidget;
class QStatusBar;
class StampDB;

class SQLDialog : public QDialog
{
  Q_OBJECT
public:
  explicit SQLDialog(StampDB& db, QWidget *parent = 0);
  virtual ~SQLDialog();

  QString getSqlText() const;

signals:
  
public slots:
  void closeButtonPressed();
  void sqlButtonPressed();

private:
  StampDB& m_db;
  QPlainTextEdit* m_textEdit;
  QTableWidget* m_tableWidget;
  QStatusBar* m_statusBar;

};

#endif // SQLDIALOG_H
