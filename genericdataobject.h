#ifndef GENERICDATAOBJECT_H
#define GENERICDATAOBJECT_H

#include <QObject>
#include <QStringList>
#include <QHash>
#include <QVariant>
#include <QDate>
#include <QDateTime>

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
  explicit GenericDataObject(QObject *parent = 0);
  
  /*! \brief Determine if the property was set.
   *  \param [in] name Property name of interest.
   *  \return True if there is a value for this property. Note that null values are generally not added and this is how you can test for that.
   */
  inline bool hasValue(const QString& name) const { return m_properties.contains(name); }

  /*! \brief Get the value associated to the name.
   *
   *  A default QVariant object is constructed and returned if a property with that name
   *  does not exist. So, it is best to call hasValue to make sure that you get what you want.
   *
   *  \param [in] name Property name of interest.
   *  \return Return the property value, or, a defaultly constructed object if it does not.
   */
  const QVariant& getValue(const QString& name) const;

  /*! \brief Set the value for a named property.
   *  \param [in] name Property name of interest.
   */
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

signals:
  
public slots:

private:
  QHash<QString, QVariant> m_properties;
};

#endif // GENERICDATAOBJECT_H
