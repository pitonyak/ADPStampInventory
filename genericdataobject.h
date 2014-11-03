#ifndef GENERICDATAOBJECT_H
#define GENERICDATAOBJECT_H

#include <QObject>
#include <QStringList>
#include <QHash>
#include <QVariant>
#include <QDate>
#include <QDateTime>
#include "tablesortfield.h"

class QSqlQuery;
class SqlFieldType;

//**************************************************************************
/*! \class GenericDataObject
 * \brief Generic object with named properties.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2012-2014
 **************************************************************************/
class GenericDataObject : public QObject
{
  Q_OBJECT
public:
  /*! \brief Constructor
   *  \param [in, out] parent The object's owner. The parent's destructor destroys this object.
   */
  explicit GenericDataObject(QObject *parent = nullptr);

  /*! \brief Replace all of the "properties" in this object. The properties are the only thing that is copied.
   *  \param [in] obj Object from which the properties are copied.
   *  \param [in, out] parent The object's owner. The parent's destructor destroys this object.
   */
  GenericDataObject(const GenericDataObject& obj, QObject *parent = nullptr);

  /*! \brief Determine if the property was set. Tests based on the lower case value of the property name.
   *  \param [in] name Property name of interest.
   *  \return True if there is a value for this property. Note that null values are generally not added and this is how you can test for that.
   */
  inline bool hasValue(const QString& name) const;

  /*! \brief Determine if the property was set.
   *  \param [in] name Lowercase version of the property name of interest.
   *  \return True if there is a value for this property. Note that null values are generally not added and this is how you can test for that.
   */
  inline bool hasValueNoCase(const QString& name) const;

  /*! \brief Get the value associated to the name with no checking or smart translations.
   *
   *  A default QVariant object is constructed and returned if a property with that name
   *  does not exist. So, it is best to call hasValue to make sure that you get what you want.
   *
   *  Ultimately, this method directly accesses m_properties object without making any checks.
   *
   *  \param [in] name Property name of interest. Converted to lower case before use.
   *  \return Return the property value, or, a defaultly constructed object if it does not.
   */
  const QVariant getValueNative(const QString& name) const;

  /*! \brief Get the value associated to the name with a check to see if the field name implies a date or time, which will cause the value to be converted to a date or time value.
   *
   *  No checks are performed to see if a properties with the given name exists.
   *
   *  \param [in] name Property name of interest. Converted to lower case before use.
   *  \return Return the property value, or, a defaultly constructed object if it does not.
   */
  const QVariant getValue(const QString& name) const;

  /*! \brief Checks the name against some hard coded names assumed to indicate a date; for example, purchasedate or releasedate.
   *
   *  \param [in] name Property name of interest compared in a case insensitive way.
   *  \return True if the field is a known date field.
   */
  static bool fieldNameMeansDate(const QString& name);

  /*! \brief Checks the name against some hard coded names assumed to indicate a date/time; for example, updated.
   *
   *  \param [in] name Property name of interest compared in a case insensitive way.
   *  \return True if the field is a known date/time field.
   */
  static bool fieldNameMeansDateTime(const QString &name);

  /*! \brief Set the value for a named property.
   *  \param [in] name Property name of interest.
   */
  void setValueNative(const QString& name, const QVariant& value);
  void setValue(const QString& name, const QVariant& value);

  /*! \brief Get the proprety as a string.
   *  \param [in] name Property name of interest.
   *  \return Return the property as a string value. Return "" if the property does not exist.
   */
  QString getString(const QString& name) const;

  /*! \brief Get the proprety as a string.
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as a string value. Return defaultValue if the property does not exist.
   */
  QString getString(const QString& name, const QString& defaultValue) const;

  /*! \brief Get the proprety as an int.
   *  \param [in] name Property name of interest.
   *  \return Return the property as an int value or defaultValue if the property does not exist.
   */
  int getInt(const QString& name, const int defaultValue = -1) const;

  /*! \brief Get the proprety as a double;
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as a double value or defaultValue if the property does not exist.
   */
  double getDouble(const QString& name, const double defaultValue = 0.0) const;


  /*! \brief Get the proprety as a QDate.
   *  \param [in] name Property name of interest.
   *  \return Return the property as an Date value or the current date if the property does not exist.
   */
  QDate getDate(const QString& name) const;

  /*! \brief Get the proprety as a QDate
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as an Date value or defaultValue if the property does not exist.
   */
  QDate getDate(const QString& name, const QDate& defaultValue) const;

  /*! \brief Get the proprety as a QDateTime
   *  \param [in] name Property name of interest.
   *  \return Return the property as an DateTime value or the current date and time if the property does not exist.
   */
  QDateTime getDateTime(const QString& name) const;

  /*! \brief Get the proprety as a QDateTime
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as an DateTime value or defaultValue if the property does not exist.
   */
  QDateTime getDateTime(const QString& name, const QDateTime& defaultValue) const;

  /*! \brief Get the proprety as a QTime
   *  \param [in] name Property name of interest.
   *  \return Return the property as an Time value or the current date and time if the property does not exist.
   */
  QTime getTime(const QString& name) const;

  /*! \brief Get the proprety as a QTime
   *  \param [in] name Property name of interest.
   *  \param [in] defaultValue Returned if the property does not exist.
   *  \return Return the property as an Time value or defaultValue if the property does not exist.
   */
  QTime getTime(const QString& name, const QTime &defaultValue) const;

  /*! \brief Replace all of the "properties" in this object. The properties are the only thing that is copied.
   *  \param [in] obj Object from which the properties are copied.
   *  \return Reference to this object.
   */
  virtual const GenericDataObject& operator=(const GenericDataObject& obj);

  /*! \brief Return a new object containing the same properties as this object. Parent is not copied, you must provide it if you want it. You own the pointer.
   *  \param [in, out] parent The object's owner. The parent's destructor destroys this object.
   *  \return Cloned copy of this object; you own the object (unless the parent does).
   */
  virtual GenericDataObject* clone(QObject *parent = nullptr) const;

  /*! \brief Does this object contain the named property with the specified string value.
   *  \param [in] lowerCaseName Lower case name of the property of interest.
   *  \param [in] compareValue Value against which to compare.
   *  \param [in] sensitive Case sensitivity, default is NOT case sensitive.
   *  \return True if the property exists with the compare value, false otherwise.
   */
  bool valueIs(const QString& lowerCaseName, const QString& compareValue, const Qt::CaseSensitivity sensitive = Qt::CaseInsensitive) const;

  int compare(const GenericDataObject& obj, const QList<TableSortField*>& sortFields) const;
  int compare(const GenericDataObject& obj, const QStringList &fields, Qt::CaseSensitivity sensitive=Qt::CaseInsensitive) const;
  bool lessThan(const GenericDataObject& obj, const QList<TableSortField *> &sortFields) const;

  // TODO: Create a generic object filter!

  bool setBindValue(QSqlQuery& query, const QString& paramName, const QString& fieldName, const SqlFieldType& fieldType, bool missingMeansNull=true) const;

signals:
  
public slots:

private:
  QHash<QString, QVariant> m_properties;
};

inline const QVariant GenericDataObject::getValueNative(const QString& name) const
{
  return m_properties.value(name.toLower());
}

inline void GenericDataObject::setValueNative(const QString &name, const QVariant& value)
{
  m_properties.insert(name.toLower(), value);
}

inline bool GenericDataObject::hasValue(const QString& name) const
{
  return hasValueNoCase(name.toLower());
}

inline bool GenericDataObject::hasValueNoCase(const QString& name) const
{
  return m_properties.contains(name);
}

inline GenericDataObject* GenericDataObject::clone(QObject *parent) const
{
    return new GenericDataObject(*this, parent);
}

inline bool GenericDataObject::valueIs(const QString& lowerCaseName, const QString& compareValue, const Qt::CaseSensitivity sensitive) const
{
  return hasValueNoCase(lowerCaseName) && (m_properties.value(lowerCaseName).toString().compare(compareValue, sensitive) == 0);
}

#endif // GENERICDATAOBJECT_H
