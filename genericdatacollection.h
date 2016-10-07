#ifndef GENERICDATACOLLECTION_H
#define GENERICDATACOLLECTION_H

#include "genericdataobject.h"
#include "tablesortfield.h"
#include "typemapper.h"

#include <QObject>
#include <QStringList>
#include <QHash>
#include <QVariant>
#include <QMetaType>

class CSVWriter;

//**************************************************************************
/*! \class GenericDataCollection
 * \brief Collection of generic objects with named and typed properties.
 *
 * Think of this as a table
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2012-2016
 **************************************************************************/
class GenericDataCollection : public QObject
{
  Q_OBJECT
public:
  /*! \brief Constructor
   *
   *  The contained objects are expected to be owned by this object, which means
   *  that this object is set as its parent.
   *
   *  \param [in, out] parent The object's owner. The parent's destructor destroys this object.
   */
  explicit GenericDataCollection(QObject *parent = nullptr);

    /*! \brief Copy Constructor.
     *  \param [in] obj Object that is copied.
     */
    GenericDataCollection(const GenericDataCollection& obj);

    /*! Turn this into a "dummy" generic table with columns: "Id", "Name", "Date", "Time", "Double", and "Bool" with the obvious data types. */
    void makeDummy();

  /*! \brief Get a property name.
   *  \param [in] i Index of the property name of interest. Must be a valid index.
   *  \return Property at index i.
   */
  const QString getPropertyName(const int i) const;

  /*! \brief Get a property name using a case insensitive search
   *  \param [in] name Case insensitive property name desired.
   *  \return Property name used by the generic objects or "" if the property is not present.
   */
  QString getPropertyName(const QString& name) const;

  /*! \brief Get a properties index
   *  \param [in] name Case insensitive property name desired.
   *  \return Index of the property, or, -1 if the property is not present.
   */
  int getPropertyIndex(const QString& name) const;

  /*! \brief Get the property names.
   *  \return List of property names in index order.
   */
  const QStringList& getPropertNames() const;

  /*! \brief Determine if a particular property exists.
   *  \param [in] name Case insensitive property name desired.
   *  \return True if the property exists, false otherwise.
   */
  bool containsProperty(const QString& fieldName) const;

  /*! \brief Get the number of property names.
   *  \return Number of property names.
   */
  int getPropertyNameCount() const;

  /*! \brief Add a property name and type.
   *  \param [in] name Case insensitive property name desired.
   *  \param [in] pType Property type for this name.
   *  \return True if the property is added, and false if it already exists.
   */
  bool appendPropertyName(const QString& name, const QMetaType::Type pType);

  /*! \brief Get a properties type based on its name.
   *  \param [in] name Case insensitive property name desired.
   *  \return Properties type if the name exists, QMetaType::Void if it does not.
   */
  QMetaType::Type getPropertyTypeMeta(const QString& name) const;

  /*! \brief Get a properties type based on its index.
   *  \param [in] i Index of the property name (the column).
   *  \return Properties type and QMetaType::Void if the index is out of bounds.
   */
  QMetaType::Type getPropertyTypeMeta(const int i) const;


  /*! \brief Add an object with the specified integer ID. Null objects are ignored. This class owns and deletes the object. The sorted ID list is not updated.
   * The largest ID is set if the ID is greater than the currently set largest ID.
   *  \param [in] id Objects integer ID.
   *  \param [in, out] obj Pointer to an object.
   */
  void appendObject(const int id, GenericDataObject* obj);

  /*! \brief Delete an object from the list based on its ID. The sorted ID list is not updated.
   *  \param [in] id Objects integer ID.
   */
  void removeObject(const int id);

  void removeRow(const int i);
  void insertRow(const int i, GenericDataObject* obj);

  // TODO: Deal with the sorted list.

  bool exportToCSV(CSVWriter& writer) const;

  int getObjectCount() const;

  /*! \brief Determine if an object with the specified ID exists.
   *  \param [in] id Unique object identifier.
   *  \return True if the specified object exists.
   */
  bool containsObject(const int id) const;

  /*! \brief Get a generic data object with the specified ID.
   *  \param [in] id Unique object identifier.
   *  \return The object if it exists and null if it does not.
   */
  const GenericDataObject* getObjectById (const int id) const;

  /*! \brief Get a generic data object with the specified ID.
   *  \param [in] id Unique object identifier.
   *  \return The object if it exists and null if it does not.
   */
  GenericDataObject* getObjectById (const int id);

  /*! \brief Find the "row" for this object ID.
   *  \param [in] id Unique object identifier.
   *  \return Index of this object, or -1 if not found.
   */
  int getIndexOf(const int id) const;


  /*! \brief Get a generic data object with the specified ID.
   *  \param [in] row Row number of an object.
   *  \return The object if it exists and null if it does not.
   */
  const GenericDataObject* getObjectByRow (const int row) const;

  /*! \brief Get a generic data object by row number.
   *  \param [in] row Row number of an object.
   *  \return The object if it exists and null if it does not.
   */
  GenericDataObject* getObjectByRow (const int row);

  /*! \return Get the largest ID known to this class. The DB is not verified. */
  int getLargestId() const;
  void setLargestId(const int n);

  bool containsValue(const int id, const QString& name) const;

  /*! \brief Get the property as a string.
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \return Return the property as a string value. Return "" if the property does not exist.
   */
  QString getString(const int id, const QString& name) const;

  /*! \brief Get the property as a string.
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as a string value. Return defaultValue if the property does not exist.
   */
  QString getString(const int id, const QString& name, const QString& defaultValue) const;


  /*! \brief Get the property as an int.
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \return Return the property as an int value or defaultValue if the property does not exist.
   */
  int getInt(const int id, const QString& name, const int defaultValue = -1) const;

  /*! \brief Get the property as a double;
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as a double value or defaultValue if the property does not exist.
   */
  double getDouble(const int id, const QString& name, const double defaultValue = 0.0) const;

  /*! \brief Get the property as a QDate.
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \return Return the property as an Date value or the current date if the property does not exist.
   */
  QDate getDate(const int id, const QString& name) const;

  /*! \brief Get the property as a QDate
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as an Date value or defaultValue if the property does not exist.
   */
  QDate getDate(const int id, const QString& name, const QDate& defaultValue) const;

  /*! \brief Get the property as a QDateTime
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \return Return the property as an DateTime value or the current date and time if the property does not exist.
   */
  QDateTime getDateTime(const int id, const QString& name) const;

  /*! \brief Get the property as a QDateTime
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as an DateTime value or defaultValue if the property does not exist.
   */
  QDateTime getDateTime(const int id, const QString& name, const QDateTime& defaultValue) const;

  /*! \brief Count the number of times a property has a specific value.
   *  \param [in] name Case Insensitive name of the property of interest.
   *  \param [in] compareValue Value against which to compare.
   *  \param [in] sensitive Case sensitivity, default is NOT case sensitive.
   *  \return Number of times an object has a property with the specified value.
   */
  int countValues(const QString& name, const QString& compareValue, const Qt::CaseSensitivity sensitive = Qt::CaseInsensitive) const;

  /*! \brief Find the first object instance that contains a named property with the specified value.
   *  \param [in] name Case Insensitive name of the property of interest.
   *  \param [in] compareValue Value against which to compare.
   *  \param [in] sensitive Case sensitivity, default is NOT case sensitive.
   *  \return A pointer to the first object with the specified value or nullptr if it does not exist.
   */
  const GenericDataObject* getObjectByValue(const QString& name, const QString& compareValue, const Qt::CaseSensitivity sensitive = Qt::CaseInsensitive) const;

  void clear();

  void clearSortFields();
  bool addSortField(const QString& name, const Qt::SortOrder order=Qt::AscendingOrder, Qt::CaseSensitivity sensitive=Qt::CaseInsensitive);
  int getSortFieldCount() const;
  TableSortField* getSortField(const int i) const;
  const QList<TableSortField*>& getSortFields() const;

  /*! \brief Assignment operator. The copied objects set the parent to be this object.
   *  \param [in] obj Object that is be copied.
   */
  const GenericDataCollection& operator=(const GenericDataCollection& obj);

  /*! number of objects. Same as object count. */
  int rowCount() const;
  void sort();

  bool isTrackChanges() const { return m_trackChanges; }
  void setTrackChanges (const bool b) { m_trackChanges = b; }

  // This will set the ID to be 1 more than the greatest ID present.
  GenericDataObject* createEmptyObject() const;

signals:

public slots:

private:
  /*! \brief largest used ID. */
  int m_largestId;

  bool m_trackChanges;

  /*! \brief In-order list of property names using what ever case is desired. This list is assumed to not have duplicate names based on case. */
  QStringList m_propertyNames;

  /*! \brief In-order list of property types. */
  QList<QMetaType::Type> m_metaTypes;

  /*! \brief Map an integer ID to a specific object. */
  QHash<int, GenericDataObject*> m_objects;

  /*! \brief Provides a fast way to map to the actual property name in a case insensitive way. */
  QHash<QString, int> m_LowerCasePropertyNameMap;

  /*! \brief List of field names against which an object should be sorted. */
  QList<TableSortField*> m_sortFields;

  // TODO: Deal with sync issues!
  /*! \brief IDs in some sorted order for record traversal. So, you sort this list and then traverse it. */
  QList<int> m_sortedIDs;

  TypeMapper m_mapper;
};

inline const QStringList& GenericDataCollection::getPropertNames() const
{
  return m_propertyNames;
}

inline bool GenericDataCollection::containsProperty(const QString& fieldName) const
{
  return m_LowerCasePropertyNameMap.contains(fieldName.toLower());
}

inline int GenericDataCollection::getPropertyNameCount() const
{
  return m_propertyNames.size();
}

inline QMetaType::Type GenericDataCollection::getPropertyTypeMeta(const int i) const
{
  Q_ASSERT_X(0 >= 0 && i < m_metaTypes.size(), "getPropertyTypeMeta", qPrintable(QString("Index %1 out of range, must be in [0, %2)").arg(i).arg(m_metaTypes.size())));
  return m_metaTypes.at(i);
}

inline int GenericDataCollection::getObjectCount() const
{
  return m_objects.size();
}

inline bool GenericDataCollection::containsObject(const int id) const
{
  return m_objects.contains(id);
}

inline const GenericDataObject* GenericDataCollection::getObjectById (const int id) const
{
  return containsObject(id) ? m_objects.value(id) : nullptr;
}

inline GenericDataObject* GenericDataCollection::getObjectById (const int id)
{
  return containsObject(id) ? m_objects.value(id) : nullptr;
}

inline int GenericDataCollection::getIndexOf(const int id) const
{
  return m_sortedIDs.indexOf(id);
}

inline const GenericDataObject* GenericDataCollection::getObjectByRow(const int row) const
{
  return (0 <= row && row < m_sortedIDs.size()) ? m_objects.value(m_sortedIDs.value(row)) : nullptr;
}

inline GenericDataObject* GenericDataCollection::getObjectByRow (const int row)
{
  return (0 <= row && row < m_sortedIDs.size()) ? m_objects.value(m_sortedIDs.value(row)) : nullptr;
}

inline int GenericDataCollection::getLargestId() const
{
  return m_largestId;
}

inline void GenericDataCollection::setLargestId(const int n)
{
  m_largestId = n;
}

inline bool GenericDataCollection::containsValue(const int id, const QString& name) const
{
  return containsObject(id) ? m_objects.value(id)->containsValue(name) : false;
}

inline QString GenericDataCollection::getString(const int id, const QString& name) const
{
  return containsObject(id) ? m_objects.value(id)->getString(name) : "";
}

inline QString GenericDataCollection::getString(const int id, const QString& name, const QString& defaultValue) const
{
  return containsObject(id) ? m_objects.value(id)->getString(name) : defaultValue;
}

inline int GenericDataCollection::getInt(const int id, const QString& name, const int defaultValue) const
{
  return containsObject(id) ? m_objects.value(id)->getInt(name) : defaultValue;
}

inline double GenericDataCollection::getDouble(const int id, const QString& name, const double defaultValue) const
{
  return containsObject(id) ? m_objects.value(id)->getDouble(name) : defaultValue;
}

inline QDate GenericDataCollection::getDate(const int id, const QString& name) const
{
  return containsObject(id) ? m_objects.value(id)->getDate(name) : QDate::currentDate();
}

inline QDate GenericDataCollection::getDate(const int id, const QString& name, const QDate& defaultValue) const
{
  return containsObject(id) ? m_objects.value(id)->getDate(name) : defaultValue;
}

inline QDateTime GenericDataCollection::getDateTime(const int id, const QString& name) const
{
  return containsObject(id) ? m_objects.value(id)->getDateTime(name) : QDateTime::currentDateTime();
}

inline QDateTime GenericDataCollection::getDateTime(const int id, const QString& name, const QDateTime& defaultValue) const
{
  return containsObject(id) ? m_objects.value(id)->getDateTime(name) : defaultValue;
}

inline const QString GenericDataCollection::getPropertyName(const int i) const
{
  return (0 <= i && i < m_propertyNames.size()) ? m_propertyNames.at(i) : "";
}

inline int GenericDataCollection::rowCount() const
{
    return m_objects.size();
}

inline int GenericDataCollection::getSortFieldCount() const
{
    return m_sortFields.size();
}

inline TableSortField * GenericDataCollection::getSortField(const int i) const
{
    return m_sortFields.at(i);
}

inline const QList<TableSortField*>& GenericDataCollection::getSortFields() const
{
    return m_sortFields;
}


#endif // GENERICDATACOLLECTION_H
