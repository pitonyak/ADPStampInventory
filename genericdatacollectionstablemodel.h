#ifndef GENERICDATACOLLECTIONSTABLEMODEL_H
#define GENERICDATACOLLECTIONSTABLEMODEL_H

#include "genericdatacollections.h"
#include "describesqltable.h"
#include "changedobject.h"
#include "changetracker.h"
// #include "linkedfieldcache.h"
#include "linkedfieldselectioncache.h"

#include <QDialog>
#include <QAbstractTableModel>

class QTableView;
class GenericDataCollectionTableModel;
class QItemSelection;
class QSortFilterProxyModel;
class StampDB;
class DescribeSqlTables;
class GenericDataCollections;
class QSqlDatabase;


//**************************************************************************
/*! \class GenericDataCollectionsTableModel
 *
 * \brief Table model for a GenericDataCollections; which means you are editing a single "table" with links.
 *
 * Provides general editing capability, but, you cannot edit a column named "id".
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2014 - 2021
 ***************************************************************************/
class GenericDataCollectionsTableModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  /*! \brief Constructor
   *
   *  \param [in] tableName
   *  \param [in] tables
   *  \param [in] schema
   *  \param [in] defaultSourceId Used only if it is relevant because the bookvalue field was updated.
   *  \param [in, out] parent The object's owner. The parent's destructor destroys this object.
   */
  explicit GenericDataCollectionsTableModel(const bool useLinks, const QString& tableName, GenericDataCollections& tables, DescribeSqlTables& schema, int defaultSourceId = -1, QObject *parent = nullptr);

  //**************************************************************************
  /*! \brief Returns the flags used to describe the item identified by the index.
   *
   *  Flags determine whether the item can be checked, edited, and selected.
   *  \param [in] index Identifies the item of interest based on a row and column.
   *  \return Flags that apply to the item specified by the index.
   ***************************************************************************/
  Qt::ItemFlags flags( const QModelIndex &index ) const;

  //**************************************************************************
  /*! \brief Returns the data for the the item identified by the index.
   *  \param [in] index Identifies the item of interest based on a row and column.
   *  \param [in] role Identifies the role for which the data should be returned. Data used for display may differ from that used for editing, for example.
   *  \return Returns the data stored under the given role for the item referred to by the index.
   ***************************************************************************/
  QVariant data( const QModelIndex &index, int role = Qt::DisplayRole ) const;

  //**************************************************************************
  /*! \brief Sets the role data for the item at index to value.
   *  \param [in] index Identifies the item of interest based on a row and column.
   *  \param [in] value Data to set.
   *  \param [in] role Identifies the role for which the data should be set.
   *  \return Returns true if successful; otherwise returns false.
   ***************************************************************************/
  bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

  //**************************************************************************
  /*! \brief Returns the data for the given role and section in the header with the specified orientation.
   *  \param [in] section For horizontal headers, the section number corresponds to the column number. Similarly, for vertical headers, the section number corresponds to the row number.
   *  \param [in] orientation Specifies if the header is horizontal or vertical.
   *  \param [in] role Identifies the role for which the data should be set.
   *  \return Returns the data for the given role and section in the header with the specified orientation.
   ***************************************************************************/
  QVariant headerData( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

  //**************************************************************************
  /*! \brief Returns the number of rows under the given parent. When the parent is valid it means that rowCount is returning the number of children of parent.
   *
   *  When implementing a table based model, rowCount() should return 0 when the parent is valid.
   *  This implementation returns the number of MessageComponents when the parent is not valid.
   *  \param [in] parent Parent item when using a tree type model.
   *  \return Returns the number of rows under the given parent. When the parent is valid it means that rowCount is returning the number of children of parent.
   ***************************************************************************/
  int rowCount( const QModelIndex &parent = QModelIndex() ) const;

  //**************************************************************************
  /*! \brief Returns the number of columns for the children of the given parent. This means number of columns / properties.
   *
   *  \param [in] parent Parent item when using a tree type model.
   *  \return Returns the number of columns for the children of the given parent.
   ***************************************************************************/
  int columnCount( const QModelIndex &parent = QModelIndex() ) const;

  //**************************************************************************
  /*! \brief if true, then changes are tracked so that undo will work as expected.
   *
   *  \return True if change tracking is on and False otherwise.
   ***************************************************************************/
  bool isTracking() const {return m_isTracking;}

  //**************************************************************************
  /*! \brief Set change tracking on / off. This does NOT affect changes that are already tracked, they stay in the change queue.
   *
   *  \param [in] isTracking True / False to turn change tracking on / off.
   *  \return True if change tracking is on and False otherwise.
   ***************************************************************************/
  void setTracking(const bool isTracking) { m_isTracking = isTracking; }

  bool trackerIsEmpty() const { return m_changeTracker.isEmpty(); }
  QStack<ChangedObject<GenericDataObject>*> *popLastChange();

  //**************************************************************************
  /*! \brief Return an ascending list of rows so that they can be deleted without changing the row number of another row.
   *
   *  \param [in] list - rows to be sorted
   *  \param [in, out] rows - first this is cleared, then populated with indexes to traverse the list in sorted order.
   ***************************************************************************/
  void getRowsAscending(const QModelIndexList &list, QList<int> &rows) const;

  // Write tracked changes to the backing DB.
  bool saveTrackedChanges(const QString& tableName, GenericDataCollection& data, QSqlDatabase& db, const DescribeSqlTables& schema);

  //**************************************************************************
  /*! \brief Builds the display value such as "USA/123/Postal"
   *
   *  If a value comes from another link field, then that value is used as another link.
   *
   *  \param [in] tableName Name of the table from which to pull linked values.
   *  \param [in] id Key used to find the row from which values are pulled.
   *  \param [in] fields Field names from which to pull linked values.
   *  \return Display value such as "USA/123/Postal"
   ***************************************************************************/
  QString getLinkValues(const QString& tableName, const int id, const QStringList& fields) const;

  //**************************************************************************
  /*! \brief This does a lot of fancy things to pull cached values.
   *
   *  This confuses me a bit, not sure why we need it, but we use it so...
   *
   *  \param [in] tableName Name of the table from which to pull linked values.
   *  \param [in] fields Field names from which to pull linked values.
   *  \return Returns a list with the single edit value.
   ***************************************************************************/
  QStringList getLinkEditValues(const QString& tableName, QStringList fields) const;

  QString incrementScottNumber(const QString& scott) const;

  int getIndexOf(const int id) const;
  QModelIndex getIndexByRowCol(int row, int col) const;

  void copyCell(const int fromRow, const int fromCol, const int toRow, const int toCol);
  void copyCell(const QModelIndex& fromIndex, const QModelIndex& toIndex);

  //**************************************************************************
  /*! \brief Add a value to the specified cell. The purpose is really to add one.
   *
   *  \param [in] row - Row number to increment.
   *  \param [in] col - Column number to increment.
   *  \param [in] incrementValue - Number to add.
   ***************************************************************************/
  void incrementCell(const int row, const int col, int incrementValue);
  void incrementCell(const QModelIndex& index, int incrementValue);

signals:

public slots:
  void addRow();
  void deleteRows(const QModelIndexList& list);
  void undoChange();
  void incCell(const QModelIndex& index, int incrementValue);

  //**************************************************************************
  /*! \brief Duplicate the specified rows and return a list of the inserted IDs.
   *
   * As the rows are duplicated, the ID is always set to the current largest plus one.
   * The Scott number will attempt to increment based on the autoIncrement parameter.
   *
   *  \param [in] list Identifies the rows to duplicate.
   *  \param [in] autoIncrement If true, will attempt to increment the Scott number.
   *  \param [in] appendChar If true, append a character to the field if possible.
   *  \param [in] charToAppend Character to append if appendChar is true.
   *
   *  \return return a list of the inserted IDs in the order that they were inserted.
   ***************************************************************************/
  QList<int> duplicateRows(const QModelIndexList& list, const bool autoIncrement, const bool appendChar, const char charToAppend);

private:
  /*! The DescribeSqlTable object can be configured to list a field as linked to another table.
   * Setting this to true causes linked fields to be displayed as the linked value rather than as the key it is.
   */
  bool m_useLinks;

  /*! if true, then changes are tracked so that undo will work as expected. */
  bool m_isTracking;
  QString m_tableName;
  GenericDataCollections& m_tables;

  /*! Primary table of interest */
  GenericDataCollection* m_table;

  DescribeSqlTables& m_schemas;
  DescribeSqlTable m_schema;
  ChangeTracker<GenericDataObject> m_changeTracker;

  //LinkedFieldCache m_linkedFieldCache;
  /*! Stores values that can be used  in drop-downs.
   *  This essentially has the data for all the linked supporteing tables.
   *  Well, I think it does.
   */
  LinkedFieldSelectionCache m_linkCache;

  /*! Used when editing the value table. When a value is updated, the "source" is set. */
  int m_defaultSourceId = -1;
};

inline int GenericDataCollectionsTableModel::getIndexOf(const int id) const
{
  return (m_table != nullptr) ? m_table->getIndexOf(id) : -1;
}

#endif // GENERICDATACOLLECTIONSTABLEMODEL_H
