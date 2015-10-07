#ifndef GENERICDATACOLLECTIONTABLESEARCHDIALOG_H
#define GENERICDATACOLLECTIONTABLESEARCHDIALOG_H

#include "searchoptions.h"

#include <QDialog>
#include <QVariant>
#include <QString>
#include <QModelIndex>

class QLineEdit;
class GenericDataCollectionTableDialog;
class QCheckBox;
class QRadioButton;

class GenericDataCollectionTableSearchDialog : public QDialog
{
    Q_OBJECT
public:
  explicit GenericDataCollectionTableSearchDialog(GenericDataCollectionTableDialog* tableDialog, QWidget *parent = nullptr);

  void set(const SearchOptions& options);
  SearchOptions getOptions() const;

  //Requires a reference to the parent dialog.

  void setCaseSensitive(const bool b);
  bool isCaseSensitive() const;

  QString getFindValue() const;
  void setFindValue(const QString& s);

  QString getReplaceValue() const;
  void setReplaceValue(const QString& s);

  bool isMatchAll() const;
  bool isContains() const;
  bool isStartsWith() const;
  bool isEndsWith() const;

  bool isMatchAsString() const;
  bool isRegularExpression() const;
  bool isWildCard() const;

  bool isWrap() const;
  bool isBackwards() const;
  bool isAllColumns() const;

  void setWrap(const bool b);
  void setBackwards(const bool b);
  void setAllColumns(const bool b);

  void setMatchAll(const bool b);
  void setContains(const bool b);
  void setStartsWith(const bool b);
  void setEndsWith(const bool b);

  void setMatchAsString(const bool b);
  void setRegularExpression(const bool b);
  void setWildCard(const bool b);

  void saveSettings() const;
  void restoreSettings();
  void focusFindField();


signals:

public slots:
  void find();
  void replace();
  void replaceAll();
  void okPressed();

private:
  void buildDialog();

  void enableButtons();

  GenericDataCollectionTableDialog* m_tableDialog;
  QLineEdit* m_findValueLineEdit;
  QLineEdit* m_replaceValueLineEdit;
  QCheckBox* m_matchCaseCB;
  QCheckBox* m_WrapCB;
  QCheckBox* m_Backwards;
  QCheckBox* m_AllColumns;

  QRadioButton* m_rbMatchAll;
  QRadioButton* m_rbContains;
  QRadioButton* m_rbStartsWith;
  QRadioButton* m_rbEndsWith;

  QRadioButton* m_rbString;
  QRadioButton* m_rbRegExp;
  QRadioButton* m_rbWildCard;


  QVariant m_findValue;
  QVariant m_replaceValue;
  QModelIndex m_firstIndex;
  QModelIndex m_lastIndex;
};

#endif // GENERICDATACOLLECTIONTABLESEARCHDIALOG_H
