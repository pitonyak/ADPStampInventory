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

  // Copy 1 cell from n above
  void copyCellFrom1Above();
  void copyCellFrom2Above();
  void copyCellFrom3Above();
  void copyCellFrom4Above();
  void copyCellFrom5Above();
  void copyCellFrom6Above();
  void copyCellFrom7Above();
  void copyCellFrom8Above();
  void copyCellFrom9Above();

  // Copy 1 cell from n below
  void copyCellFrom1Below();
  void copyCellFrom2Below();
  void copyCellFrom3Below();
  void copyCellFrom4Below();
  void copyCellFrom5Below();
  void copyCellFrom6Below();
  void copyCellFrom7Below();
  void copyCellFrom8Below();
  void copyCellFrom9Below();

  // Copy n cells from n above
  void copyCellsFrom1Above();
  void copyCellsFrom2Above();
  void copyCellsFrom3Above();
  void copyCellsFrom4Above();
  void copyCellsFrom5Above();
  void copyCellsFrom6Above();
  void copyCellsFrom7Above();
  void copyCellsFrom8Above();
  void copyCellsFrom9Above();

  // Copy n cells from n below
  void copyCellsFrom1Below();
  void copyCellsFrom2Below();
  void copyCellsFrom3Below();
  void copyCellsFrom4Below();
  void copyCellsFrom5Below();
  void copyCellsFrom6Below();
  void copyCellsFrom7Below();
  void copyCellsFrom8Below();
  void copyCellsFrom9Below();

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
   * Each row copied (one cell) creates one undo operation.
   * If a book value is updated, the default value source (catalogue) is updated.
   * If there is a last updated column, that is updated as well.
   *
   *  \param [in] rowsDown How many rows down (> 0) or up (< 0) from current row to copy.
   *  \param [in] numCells How many cells to copy.
   */
  void copyCells(const int rowsDown, const int numCells);

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


inline void GenericDataCollectionTableDialog::copyCellFrom1Above()
{
  copyCells(-1, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom2Above()
{
  copyCells(-2, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom3Above()
{
  copyCells(-3, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom4Above()
{
  copyCells(-4, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom5Above()
{
  copyCells(-5, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom6Above()
{
  copyCells(-6, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom7Above()
{
  copyCells(-7, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom8Above()
{
  copyCells(-8, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom9Above()
{
  copyCells(-9, 1);
}


inline void GenericDataCollectionTableDialog::copyCellFrom1Below()
{
  copyCells(1, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom2Below()
{
  copyCells(2, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom3Below()
{
  copyCells(3, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom4Below()
{
  copyCells(4, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom5Below()
{
  copyCells(5, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom6Below()
{
  copyCells(6, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom7Below()
{
  copyCells(7, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom8Below()
{
  copyCells(8, 1);
}
inline void GenericDataCollectionTableDialog::copyCellFrom9Below()
{
  copyCells(9, 1);
}

inline void GenericDataCollectionTableDialog::copyCellsFrom1Above()
{
  copyCells(-1, 1);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom2Above()
{
  copyCells(-2, 2);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom3Above()
{
  copyCells(-3, 3);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom4Above()
{
  copyCells(-4, 4);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom5Above()
{
  copyCells(-5, 5);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom6Above()
{
  copyCells(-6, 6);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom7Above()
{
  copyCells(-7, 7);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom8Above()
{
  copyCells(-8, 8);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom9Above()
{
  copyCells(-9, 9);
}


inline void GenericDataCollectionTableDialog::copyCellsFrom1Below()
{
  copyCells(1, 1);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom2Below()
{
  copyCells(2, 2);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom3Below()
{
  copyCells(3, 3);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom4Below()
{
  copyCells(4, 4);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom5Below()
{
  copyCells(5, 5);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom6Below()
{
  copyCells(6, 6);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom7Below()
{
  copyCells(7, 7);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom8Below()
{
  copyCells(8, 8);
}
inline void GenericDataCollectionTableDialog::copyCellsFrom9Below()
{
  copyCells(9, 9);
}



#endif // GENERICDATACOLLECTIONTABLEDIALOG_H
