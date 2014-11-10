#ifndef GENERICDATACOLLECTIONTABLEDIALOG_H
#define GENERICDATACOLLECTIONTABLEDIALOG_H

#include "genericdatacollection.h"

#include <QDialog>

class QTableView;
class GenericDataCollectionTableModel;
class GenericDataCollectionsTableModel;
class QItemSelection;
class StampDB;
class DescribeSqlTables;
class GenericDataCollections;
class GenericDataCollectionsTableProxy;

class GenericDataCollectionTableDialog : public QDialog
{
  Q_OBJECT
public:
  explicit GenericDataCollectionTableDialog(const QString& tableName, GenericDataCollection& data, StampDB& db, DescribeSqlTables& schema, GenericDataCollections *tables, QWidget *parent=nullptr);

  /*! \brief Destructor saves dialog geometry. */
  virtual ~GenericDataCollectionTableDialog();

  /*! \brief Save the dialogs current state / size. */
  void virtual saveState();
signals:

public slots:
  void selectionChanged( const QItemSelection & selected, const QItemSelection & deselected );
  void addRow();
  void deleteRow();
  void undoChange();
  void duplicateRow();
  void saveChanges();

  /*! \brief Call save state before the destructor is called. */
  void clickedOK();

private:
  /*! \brief Set list columns, delegates, and initial values. */
  void buildDialog();

  /*! \brief Get the index of the current row. */
  int getSelectedRow() const;

  /*! \brief Enable/disable buttons based on dialog values. */
  void enableButtons();

  /*! \brief Return True if a row is currently selected. */
  bool isRowSelected() const;

  /*! \brief Copy the selected rows */
  QPushButton* m_duplicateButton;
  /*! \brief Add a new row */
  QPushButton* m_addButton;
  /*! \brief Delete the selected rows */
  QPushButton* m_deleteButton;
  /*! \brief Undo the last change */
  QPushButton* m_undoButton;
  /*! \brief Persist pending changes to the DB */
  QPushButton* m_SaveChangesButton;

  /*! \brief Identifies the columns and the types. */
  GenericDataCollection& m_table;
  GenericDataCollections *m_tables;

  /*! \brief View presented to the user */
  QTableView* m_tableView;

  /*! \brief Used for saving and restoring dialog sizes. */
  QString m_tableName;

  /*! \brief Actual data model to which changes are made and such. */
  GenericDataCollectionsTableModel* m_tableModel;

  /*! \brief Used by the view to allow for sorting and similar. the actual table model is contained inside of this. */
  //QSortFilterProxyModel* m_proxyModel;
  GenericDataCollectionsTableProxy* m_proxyModel;

  /*! \brief We need this here so that we can persist the DB. */
  StampDB& m_db;

  DescribeSqlTables& m_schema;
};

#endif // GENERICDATACOLLECTIONTABLEDIALOG_H
