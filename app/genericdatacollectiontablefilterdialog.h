#ifndef GENERICDATACOLLECTIONTABLEFILTERDIALOG_H
#define GENERICDATACOLLECTIONTABLEFILTERDIALOG_H

#include <QDialog>
#include <QVariant>
#include <QString>
#include <QModelIndex>

class QLineEdit;
class GenericDataCollectionTableDialog;
class QCheckBox;
class QRadioButton;

class GenericDataCollectionTableFilterDialog : public QDialog
{
  Q_OBJECT
public:
  explicit GenericDataCollectionTableFilterDialog(GenericDataCollectionTableDialog* tableDialog, QWidget *parent = nullptr);

  void setCaseSensitive(const Qt::CaseSensitivity sensitivity);
  void setCaseSensitive() { setCaseSensitive(Qt::CaseSensitive); }
  void setCaseInsensitive() { setCaseSensitive(Qt::CaseInsensitive); }
  Qt::CaseSensitivity getCaseSensitivity() const;
  bool isCaseSensitive() const {return getCaseSensitivity() == Qt::CaseSensitive; }

signals:

public slots:
  void applyFilter();
  void clearFilter();

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

  QRadioButton* m_rbString;
  QRadioButton* m_rbRegExp;
  QRadioButton* m_rbWild;

};

#endif // GENERICDATACOLLECTIONTABLEFILTERDIALOG_H
