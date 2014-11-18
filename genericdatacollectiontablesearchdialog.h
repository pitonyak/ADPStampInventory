#ifndef GENERICDATACOLLECTIONTABLESEARCHDIALOG_H
#define GENERICDATACOLLECTIONTABLESEARCHDIALOG_H

#include <QDialog>
#include <QVariant>
#include <QString>

class QLineEdit;
class GenericDataCollectionTableDialog;
class QCheckBox;

class GenericDataCollectionTableSearchDialog : public QDialog
{
    Q_OBJECT
public:
  explicit GenericDataCollectionTableSearchDialog(GenericDataCollectionTableDialog* tableDialog, QWidget *parent = nullptr);

  //Requires a reference to the parent dialog.

  void setCaseSensitive(const Qt::CaseSensitivity sensitivity);
  void setCaseSensitive() { setCaseSensitive(Qt::CaseSensitive); }
  void setCaseInsensitive() { setCaseSensitive(Qt::CaseInsensitive); }
  Qt::CaseSensitivity getCaseSensitivity() const;
  bool isCaseSensitive() const {return getCaseSensitivity() == Qt::CaseSensitive; }

signals:

public slots:
  void find();
  void replace();
  void replaceAll();

private:
  void buildDialog();

  void enableButtons();

  GenericDataCollectionTableDialog* m_tableDialog;
  QLineEdit* m_findValueLineEdit;
  QLineEdit* m_replaceValueLineEdit;
  QCheckBox* m_matchCaseCB;
  QCheckBox* m_matchEntireCellCB;
  QCheckBox* m_regularExpressionCB;
  QCheckBox* m_selectionOnlyCB;

  QVariant m_findValue;
  QVariant m_replaceValue;
};

#endif // GENERICDATACOLLECTIONTABLESEARCHDIALOG_H
