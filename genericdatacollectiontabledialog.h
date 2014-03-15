#ifndef GENERICDATACOLLECTIONTABLEDIALOG_H
#define GENERICDATACOLLECTIONTABLEDIALOG_H

#include "genericdatacollection.h"

#include <QDialog>

class QTableView;
class GenericDataCollectionTableModel;

class GenericDataCollectionTableDialog : public QDialog
{
  Q_OBJECT
public:
  explicit GenericDataCollectionTableDialog(GenericDataCollection& data, QWidget *parent = nullptr);

  /*! \brief Destructor saves dialog geometry. */
  virtual ~GenericDataCollectionTableDialog();
signals:

public slots:

private:
  /*! \brief Set list columns, delegates, and initial values. */
  void buildDialog();

  /*! \brief Get the index of the current row. */
  int getSelectedRow() const;

  /*! \brief Enable/disable buttons based on dialog values. */
  void enableButtons();

  /*! \brief Return True if a row is currently selected. */
  bool isRowSelected() const;

  //QPushButton* m_upButton;
  //QPushButton* m_downButton;
  //QPushButton* m_addButton;
  //QPushButton* m_deleteButton;

  /*! \brief Identifies the columns and the types. */
  GenericDataCollection& m_dataCollection;
  QTableView* m_tableView;
  //QLineEdit* m_configFilePath;
  GenericDataCollectionTableModel* m_tableModel;
  TypeMapper m_mapper;
};

#endif // GENERICDATACOLLECTIONTABLEDIALOG_H
