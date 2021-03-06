#ifndef GENERICDATACOLLECTIONTABLEMODEL_H
#define GENERICDATACOLLECTIONTABLEMODEL_H

#include "genericdatacollection.h"
#include "changetracker.h"

#include <QAbstractTableModel>

class QSqlDatabase;
class DescribeSqlTables;

//**************************************************************************
/*! \class GenericDataCollectionTableModel
 *
 * \brief Table model for a GenericDataCollection; which means you are editing a single "table".
 *
 * Provides general editing capability, but, you cannot edit a column named "id".
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2013-2014
 ***************************************************************************/
class GenericDataCollectionTableModel : public QAbstractTableModel
{
  Q_OBJECT
public:
    /*! \brief Constructor
     *  \param [in, out] parent The object's owner. The parent's destructor destroys this object.
     */
  explicit GenericDataCollectionTableModel(GenericDataCollection& data, QObject *parent = nullptr);
  
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

    bool isTracking() const {return m_isTracking;}
    void setTracking(const bool isTracking) { m_isTracking = isTracking; }

    bool trackerIsEmpty() const { return m_changeTracker.isEmpty(); }
    QStack<ChangedObject<GenericDataObject>*> *popLastChange();

    // Return an ascending list of rows so that they can be deleted without
    // changing the row number of another row.
    void getRowsAscending(const QModelIndexList &list, QList<int> &rows) const;

    // Write tracked changes to the backing DB.
    bool saveTrackedChanges(const QString& tableName, const GenericDataCollection& data, QSqlDatabase& db, const DescribeSqlTables& schema);

signals:
  
public slots:
    void addRow();
    void deleteRows(const QModelIndexList& list);
    void undoChange();
    void duplicateRows(const QModelIndexList& list);

private:
    bool m_isTracking;
    GenericDataCollection& m_collection;
    ChangeTracker<GenericDataObject> m_changeTracker;
};

#endif // GENERICDATACOLLECTIONTABLEMODEL_H
