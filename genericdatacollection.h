#ifndef GENERICDATACOLLECTION_H
#define GENERICDATACOLLECTION_H

#include "genericdataobject.h"
#include "tablesortfield.h"

#include <QObject>
#include <QStringList>
#include <QHash>
#include <QVariant>

class CSVWriter;

//**************************************************************************
/*! \class GenericDataCollection
 * \brief Collection of generic objects with named and typed properties.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2012-2014
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

  /*! \brief Get a property name.
   *  \param [in] i Index of the property name of interest. Must be a valid index.
   *  \return Property at index i.
   */
  const QString& getPropertyName(const int i) const;

  /*! \brief Get a property name using a case insensitive search
   *  \param [in] name Case insensitive property name desired.
   *  \return Property name used by the generic objects or "" if the property is not present.
   */
  QString getPropertyName(const QString& name) const;

  /*! \brief Get the property names.
   *  \return List of property names in index order.
   */
  const QStringList& getPropertNames() const;

  /*! \brief Determine if a particular property exists.
   *  \param [in] name Case insensitive property name desired.
   *  \return True if the property exists, false otherwise.
   */
  bool hasProperty(const QString& name) const;

  /*! \brief Get the number of property names.
   *  \return Number of property names.
   */
  int getPropertyNameCount() const;

  /*! \brief Add a property name and type.
   *  \param [in] name Case insensitive property name desired.
   *  \param [in] pType Property type for this name.
   *  \return True if the property is added, and false if it already exists.
   */
  bool appendPropertyName(const QString& name, const QVariant::Type pType);

  /*! \brief Get a properties type based on its name.
   *  \param [in] name Case insensitive property name desired.
   *  \return Properties type if the name exists, and QVariant::Invalid if it does not.
   */
  QVariant::Type getPropertyType(const QString& name) const;

  /*! \brief Get a properties type based on its index.
   *  \param [in] i Index of the property name (the column).
   *  \return Properties type. If the index is out of bounds, well, it had better not be out of bounds.
   */
  QVariant::Type getPropertyType(const int i) const;

  /*! \brief Add an object with the specified integer ID. Null objects are ignored. This class owns and deletes the object. The sorted ID list is not updated.
   *  \param [in] id Objects integer ID.
   *  \param [in, out] obj Pointer to an object.
   */
  void appendObject(const int id, GenericDataObject* obj);

  /*! \brief Delete an object from the list based on its ID. The sorted ID list is not updated.
   *  \param [in] id Objects integer ID.
   */
  void removeObject(const int);

  // TODO: Deal with the sorted list.

  bool exportToCSV(CSVWriter& writer) const;

  int getObjectCount() const;

  /*! \brief Determine if an object with the specified ID exists.
   *  \param [in] id Unique object identifier.
   *  \return True if the specified object exists.
   */
  bool hasObject(const int id) const;

  const GenericDataObject* getObjectById (const int id) const;
  GenericDataObject* getObjectById (const int id);

  bool hasValue(const int id, const QString& name) const;

  /*! \brief Get the proprety as a string.
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \return Return the property as a string value. Return "" if the property does not exist.
   */
  QString getString(const int id, const QString& name) const;

  /*! \brief Get the proprety as a string.
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as a string value. Return defaultValue if the property does not exist.
   */
  QString getString(const int id, const QString& name, const QString& defaultValue) const;


  /*! \brief Get the proprety as an int.
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \return Return the property as an int value or defaultValue if the property does not exist.
   */
  int getInt(const int id, const QString& name, const int defaultValue = -1) const;

  /*! \brief Get the proprety as a double;
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as a double value or defaultValue if the property does not exist.
   */
  double getDouble(const int id, const QString& name, const double defaultValue = 0.0) const;


  /*! \brief Get the proprety as a QDate.
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \return Return the property as an Date value or the current date if the property does not exist.
   */
  QDate getDate(const int id, const QString& name) const;

  /*! \brief Get the proprety as a QDate
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as an Date value or defaultValue if the property does not exist.
   */
  QDate getDate(const int id, const QString& name, const QDate& defaultValue) const;

  /*! \brief Get the proprety as a QDateTime
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \return Return the property as an DateTime value or the current date and time if the property does not exist.
   */
  QDateTime getDateTime(const int id, const QString& name) const;

  /*! \brief Get the proprety as a QDateTime
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
  bool addSortField(const QString& name, const TableSortField::SortOrder order=TableSortField::Ascending, Qt::CaseSensitivity sensitive=Qt::CaseInsensitive);
  int getSortFieldCount() const;
  TableSortField* getSortField(const int i) const;
  const QList<TableSortField*>& getSortFields() const;

  /*! \brief Assignment operator. The copied objects set the parent to be this object.
   *  \param [in] obj Object that is be copied.
   */
  const GenericDataCollection& operator=(const GenericDataCollection& obj);

  int rowCount() const;
  void sort();

signals:

public slots:

private:
  /*! \brief In-order list of property names using what ever case is desired. This list is assumed to not have duplicate names based on case. */
  QStringList m_propertyNames;

  /*! \brief In-order list of property types. */
  QList<QVariant::Type> m_propertyTypes;

  /*! \brief Map an integer ID to a specific object. */
  QHash<int, GenericDataObject*> m_objects;

  /*! \brief Provides a fast way to map to the actual property name in a case insensitive way. */
  QHash<QString, int> m_LowerCasePropertyNameMap;

  /*! \brief List of field names against which an object should be sorted. */
  QList<TableSortField*> m_sortFields;

  // TODO: Deal with sync issues!
  /*! \brief IDs in some sorted order for record traversal. So, you sort this list and then traverse it. */
  QList<int> m_sortedIDs;
};

inline const QStringList& GenericDataCollection::getPropertNames() const
{
  return m_propertyNames;
}

inline bool GenericDataCollection::hasProperty(const QString& name) const
{
  return m_LowerCasePropertyNameMap.contains(name.toLower());
}

inline int GenericDataCollection::getPropertyNameCount() const
{
  return m_propertyNames.count();
}

inline QVariant::Type GenericDataCollection::getPropertyType(const int i) const
{
  return m_propertyTypes.at(i);
}

inline int GenericDataCollection::getObjectCount() const
{
  return m_objects.count();
}

inline bool GenericDataCollection::hasObject(const int id) const
{
  return m_objects.contains(id);
}

inline const GenericDataObject* GenericDataCollection::getObjectById (const int id) const
{
  return hasObject(id) ? m_objects.value(id) : nullptr;
}

inline GenericDataObject* GenericDataCollection::getObjectById (const int id)
{
  return hasObject(id) ? m_objects.value(id) : nullptr;
}

inline bool GenericDataCollection::hasValue(const int id, const QString& name) const
{
  return hasObject(id) ? m_objects.value(id)->hasValue(name) : false;
}

inline QString GenericDataCollection::getString(const int id, const QString& name) const
{
  return hasObject(id) ? m_objects.value(id)->getString(name) : "";
}

inline QString GenericDataCollection::getString(const int id, const QString& name, const QString& defaultValue) const
{
  return hasObject(id) ? m_objects.value(id)->getString(name) : defaultValue;
}

inline int GenericDataCollection::getInt(const int id, const QString& name, const int defaultValue) const
{
  return hasObject(id) ? m_objects.value(id)->getInt(name) : defaultValue;
}

inline double GenericDataCollection::getDouble(const int id, const QString& name, const double defaultValue) const
{
  return hasObject(id) ? m_objects.value(id)->getDouble(name) : defaultValue;
}

inline QDate GenericDataCollection::getDate(const int id, const QString& name) const
{
  return hasObject(id) ? m_objects.value(id)->getDate(name) : QDate::currentDate();
}

inline QDate GenericDataCollection::getDate(const int id, const QString& name, const QDate& defaultValue) const
{
  return hasObject(id) ? m_objects.value(id)->getDate(name) : defaultValue;
}

inline QDateTime GenericDataCollection::getDateTime(const int id, const QString& name) const
{
  return hasObject(id) ? m_objects.value(id)->getDateTime(name) : QDateTime::currentDateTime();
}

inline QDateTime GenericDataCollection::getDateTime(const int id, const QString& name, const QDateTime& defaultValue) const
{
  return hasObject(id) ? m_objects.value(id)->getDateTime(name) : defaultValue;
}

inline const QString& GenericDataCollection::getPropertyName(const int i) const
{
  return m_propertyNames.at(i);
}

inline int GenericDataCollection::rowCount() const
{
    return m_objects.count();
}

inline int GenericDataCollection::getSortFieldCount() const
{
    return m_sortFields.count();
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
