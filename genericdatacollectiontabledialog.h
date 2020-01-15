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
class GenericDataCollectionTableSearchDialog;
class SearchOptions;

//**************************************************************************
/*! \class GenericDataCollectionTableDialog
 * \brief Generic dialog for editing a single table.
 *
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2012-2019
 **************************************************************************/

class GenericDataCollectionTableDialog : public QDialog
{
  Q_OBJECT
public:
  /*! \brief Constructor
   *
   *  \param [in] tableName Name of the table that will be edited.
   *  \param [in, out] data This is essentially the table that will be edited.
   *  \param [in, out] db Encapsulates data acccess.
   *  \param [in, out] schema For a set of tables.
   *  \param [in, out] tables Data for all of the related tables.
   *  \param [in, out] defaultSourceId Used only if it is relevant because the bookvalue field was updated. So only used for a single table listing what a stamp is worth.
   *  \param [in, out] parent The object's owner. The parent's destructor destroys this object.
   */
  explicit GenericDataCollectionTableDialog(const QString& tableName, GenericDataCollection& data, StampDB& db, DescribeSqlTables& schema, GenericDataCollections *tables, int defaultSourceId = -1, QWidget *parent=nullptr);

  /*! \brief Destructor saves dialog geometry. */
  virtual ~GenericDataCollectionTableDialog();

  /*! \brief Save the dialogs current state / size. */
  void virtual saveState();

  // Sadly, this cannot be virtual because it is called from the constructor.
  // I only pulled this out so that it would be in an easily seen separated location.
  void restoreState();

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
   * TODO: is this even used? Regular expressions and other things do work!
   * see GenenicDataCollectionsTableProxy
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

  /*! \brief Search method called from the search dialog.
   *
   *  \param [in] options Search options used to direct the sesarch.
   *
   *  \param [in] includeCurrent If true, the current cell is included in a forward search.
   *
   *  \return True if something is found.
   */
  bool doFind(const SearchOptions& options, const bool includeCurrent);

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

  bool genericSearch(const bool findNext, const bool findPrevious, const bool findDialog);


  /*! \brief Call save state before the destructor is called. */
  void clickedOK();

  /*! \brief Verify cancel with unsaved changes. */
  void clickedCancel();

protected:
  /*! \brief Handle special key press events such as F3 (find next) */
  virtual void keyPressEvent(QKeyEvent* evt);

private:

  void privateRowDuplicator(const bool autoIncrement, const bool appendChar=false, const char charToAppend='a');

  /*! \brief Copy cell from the same column to the current row.
   *
   *  \param [in] rowsDown How many rows down (> 0) or up (< 0) from current row to copy.
   */
  void copyCell(const int rowsDown);

  /*! \brief Increment current cell
   *
   *  \param [in] incrementValue
   */
  void incrementCell(const int incrementValue);


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

  void selectCell(const QModelIndex& index);

  virtual void displayHelp();

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

  GenericDataCollectionTableSearchDialog* m_searchWindow;
  //QSortFilterProxyModel* m_proxyModel;

  /*! \brief We need this here so that we can persist the DB. */
  StampDB& m_db;

  /*! \brief Describes all the tables. */
  DescribeSqlTables& m_schema;

  /*! \brief Used only if it is relevant because the bookvalue field was updated. So only used for a single table listing what a stamp is worth. */
  int m_defaultSourceId = -1;
};

#endif // GENERICDATACOLLECTIONTABLEDIALOG_H
