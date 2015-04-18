#ifndef GENERICDATACOLLECTIONTABLEDIALOG_H
#define GENERICDATACOLLECTIONTABLEDIALOG_H

#include "genericdatacollection.h"
#include "genericdatacollectionstablemodel.h"
#include "genericdatacollectiontablemodel.h"

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

  /*! \brief Find the next instance of the string "s" based on the current cursor position and column.
   * TODO: Should support extra search options such as regular expression, wild card, full match, partial match, etc.
   * TODO: Should support all column search.
   * TODO: Support searching by row.
   *
   *  \param [in] s String to find.
   *
   *  \param [in] searchForward If True, searches down, if false, searches up.
   *
   *  \return The location of the found item. Return an invalid index if not found.
   */
  QModelIndex find(const QString& s, const bool searchForward);

  /*! \brief Find the next instance of the string "s" based on the current cursor position and column.
   * TODO: Should support extra search options such as regular expression, wild card, full match, partial match, etc.
   *
   *  \param [in] s String to find.
   *
   *  \param [in] lastIndex Starting position from which to search. So, this is like find next or find previous.
   *
   *  \param [in] searchForward If True, searches down, if false, searches up.
   *
   *  \return The location of the found item. Return an invalid index if not found.
   */
  QModelIndex find(const QString& s, const QModelIndex& lastIndex, const bool searchForward);

  GenericDataCollectionsTableModel* getTableModel() { return m_tableModel; }
  GenericDataCollectionsTableProxy* getProxyModel() { return m_proxyModel; }

signals:

public slots:
  void selectionChanged( const QItemSelection & selected, const QItemSelection & deselected );
  void addRow();
  void deleteRow();
  void undoChange();
  void duplicateRow();
  void duplicateRowAutoIncrement();
  void duplicateRowAddLowerA();
  void duplicateRowAddUpperA();
  void saveChanges();
  void searchDialog();

  /*! \brief Call save state before the destructor is called. */
  void clickedOK();

  /*! \brief Verify cancel with unsaved changes. */
  void clickedCancel();

protected:
  virtual void keyPressEvent(QKeyEvent* evt);

private:

  void privateRowDuplicator(const bool autoIncrement, const bool setUpdated, const bool appendChar=false, const char charToAppend='a');

  /*! \brief Set list columns, delegates, and initial values. */
  void buildDialog();

  /*! \brief Get the index of the current row. */
  int getSelectedRow() const;

  /*! \brief Enable/disable buttons based on dialog values. */
  void enableButtons();

  /*! \brief disable ALL buttons. */
  void disableButtons();

  /*! \brief Return True if a row is currently selected. */
  bool isRowSelected() const;

  /*! \brief Copy the selected rows */
  QPushButton* m_duplicateButton;
  QPushButton* m_duplicateButtonIncrement;
  QPushButton* m_duplicateButtonAppendLowerA;
  QPushButton* m_duplicateButtonAppendUpperA;
  /*! \brief Add a new row */
  QPushButton* m_addButton;
  /*! \brief Delete the selected rows */
  QPushButton* m_deleteButton;
  /*! \brief Undo the last change */
  QPushButton* m_undoButton;
  /*! \brief Persist pending changes to the DB */
  QPushButton* m_saveChangesButton;
  QPushButton* m_searchButton;

  /*! \brief Identifies the columns and the types. */
  GenericDataCollection& m_table;
  GenericDataCollections *m_tables;

  /*! \brief View presented to the user */
  QTableView* m_tableView;

  /*! \brief Used for saving and restoring dialog sizes. */
  QString m_tableName;

  /*! \brief Actual data model to which changes are made and such. This is the source model for the proxy model. */
  GenericDataCollectionsTableModel* m_tableModel;

  /*! \brief Used by the view to allow for sorting and similar. the actual table model is contained inside of this. The m_tableModel is the source model for this proxy model. */
  GenericDataCollectionsTableProxy* m_proxyModel;
  //QSortFilterProxyModel* m_proxyModel;

  /*! \brief We need this here so that we can persist the DB. */
  StampDB& m_db;

  DescribeSqlTables& m_schema;
};

#endif // GENERICDATACOLLECTIONTABLEDIALOG_H
