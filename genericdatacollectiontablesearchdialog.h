#ifndef GENERICDATACOLLECTIONTABLESEARCHDIALOG_H
#define GENERICDATACOLLECTIONTABLESEARCHDIALOG_H

#include <QDialog>
#include <QVariant>
#include <QString>

class QLineEdit;

class GenericDataCollectionTableSearchDialog : public QDialog
{
    Q_OBJECT
public:
  explicit GenericDataCollectionTableSearchDialog(QWidget *parent = nullptr);

  //Requires a reference to the parent dialog.

  void setCaseSensitive(const Qt::CaseSensitivity sensitivity) {m_caseSensitivity = sensitivity;}
  void setCaseSensitive() { setCaseSensitive(Qt::CaseSensitive); }
  void setCaseInsensitive() { setCaseSensitive(Qt::CaseInsensitive); }
  Qt::CaseSensitivity getCaseSensitivity() const { return m_caseSensitivity; }
  bool isCaseSensitive() const {return getCaseSensitivity() == Qt::CaseSensitive; }

signals:

public slots:

private:
  void buildDialog();

  void enableButtons();

  QLineEdit* m_findValueLineEdit;
  QLineEdit* m_replaceValueLineEdit;

  Qt::CaseSensitivity m_caseSensitivity;
  QVariant m_findValue;
  QVariant m_replaceValue;
};

#endif // GENERICDATACOLLECTIONTABLESEARCHDIALOG_H
