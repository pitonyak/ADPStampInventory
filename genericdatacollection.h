#ifndef GENERICDATACOLLECTION_H
#define GENERICDATACOLLECTION_H

#include "genericdataobject.h"

#include <QObject>
#include <QStringList>
#include <QHash>
#include <QVariant>

class CSVWriter;

//**************************************************************************
/*! \class GenericDataCollection
 * \brief Collection of generic objects with named properties.
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
  explicit GenericDataCollection(QObject *parent = 0);

  /*! \brief Get a property name.
   *  \param [in] i Index of the property name of interest. Must be a valid index.
   *  \return Property at index i.
   */
  inline const QString& getPropertyName(const int i) const { return m_propertyNames.at(i); }

  /*! \brief Get a property name using a case insensitive search
   *  \param [in] name Case insensitive property name desired.
   *  \return Property name used by the generic objects or "" if the property is not present.
   */
  QString getPropertyName(const QString& name) const;

  inline const QStringList& getPropertNames() const { return m_propertyNames; }
  inline bool hasProperty(const QString& name) const { return m_LowerCasePropertyNameMap.contains(name.toLower()); }
  inline int getPropertyNameCount() const { return m_propertyNames.count(); }

  bool appendPropertyName(const QString& name, const QVariant::Type pType);

  QVariant::Type getPropertyType(const QString& name) const;
  inline QVariant::Type getPropertyType(const int i) const { return m_propertyTypes.at(i); }

  void appendObject(const int id, GenericDataObject* obj);
  void removeObject(const int);

  bool exportToCSV(CSVWriter& writer) const;

  inline int getObjectCount() const { return m_objects.count(); }

  /*! \brief Determine if an object with the specified ID exists.
   *  \param [in] id Unique object identifier.
   *  \return True if the specified object exists.
   */
  inline bool hasObject(const int id) const { return m_objects.contains(id); }

  inline const GenericDataObject* getObjectById (const int id) const { return hasObject(id) ? m_objects.value(id) : nullptr; }
  inline GenericDataObject* getObjectById (const int id) { return hasObject(id) ? m_objects.value(id) : nullptr; }

  inline bool hasValue(const int id, const QString& name) const { return hasObject(id) ? m_objects.value(id)->hasValue(name) : false; }

  /*! \brief Get the proprety as a string.
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \return Return the property as a string value. Return "" if the property does not exist.
   */
  inline QString getString(const int id, const QString& name) const { return hasObject(id) ? m_objects.value(id)->getString(name) : ""; }

  /*! \brief Get the proprety as a string.
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as a string value. Return defaultValue if the property does not exist.
   */
  QString getString(const int id, const QString& name, const QString& defaultValue) const { return hasObject(id) ? m_objects.value(id)->getString(name) : defaultValue; }


  /*! \brief Get the proprety as an int.
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \return Return the property as an int value or defaultValue if the property does not exist.
   */
  int getInt(const int id, const QString& name, const int defaultValue = -1) const { return hasObject(id) ? m_objects.value(id)->getInt(name) : defaultValue; }

  /*! \brief Get the proprety as a double;
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as a double value or defaultValue if the property does not exist.
   */
  double getDouble(const int id, const QString& name, const double defaultValue = 0.0) const { return hasObject(id) ? m_objects.value(id)->getDouble(name) : defaultValue; }


  /*! \brief Get the proprety as a QDate.
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \return Return the property as an Date value or the current date if the property does not exist.
   */
  QDate getDate(const int id, const QString& name) const { return hasObject(id) ? m_objects.value(id)->getDate(name) : QDate::currentDate(); }

  /*! \brief Get the proprety as a QDate
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as an Date value or defaultValue if the property does not exist.
   */
  QDate getDate(const int id, const QString& name, const QDate& defaultValue) const { return hasObject(id) ? m_objects.value(id)->getDate(name) : defaultValue; }

  /*! \brief Get the proprety as a QDateTime
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \return Return the property as an DateTime value or the current date and time if the property does not exist.
   */
  QDateTime getDateTime(const int id, const QString& name) const { return hasObject(id) ? m_objects.value(id)->getDateTime(name) : QDateTime::currentDateTime(); }

  /*! \brief Get the proprety as a QDateTime
   *  \param [in] id Unique object identifier.
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as an DateTime value or defaultValue if the property does not exist.
   */
  QDateTime getDateTime(const int id, const QString& name, const QDateTime& defaultValue) const { return hasObject(id) ? m_objects.value(id)->getDateTime(name) : defaultValue; }

signals:

public slots:

private:
  QStringList m_propertyNames;
  QList<QVariant::Type> m_propertyTypes;
  QHash<int, GenericDataObject*> m_objects;

  /*! \brief Provides a fast way to map to the actual property name in a case insensitive way. */
  QHash<QString, int> m_LowerCasePropertyNameMap;
};

#endif // GENERICDATACOLLECTION_H
