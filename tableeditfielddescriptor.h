#ifndef TABLEEDITFIELDDESCRIPTOR_H
#define TABLEEDITFIELDDESCRIPTOR_H

#include <QObject>
#include <QList>

class QXmlStreamWriter;
class QXmlStreamReader;

//**************************************************************************
/*! \class TableEditFieldDescriptor
 * \brief Describe a single field in a table used for display while editing.
 *
 * The best way to represent the data of interest is an ongoing process.
 * The thought for this class is that while editing a single table, it may be desired for
 * a value from another table to be displayed in its place.
 * In fact, a single field may be displayed more than once. As an example,
 * I might want to link a "catalog ID" to a country, date, and description.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date December 6, 2014
 **************************************************************************/
class TableEditFieldDescriptor : public QObject
{
  Q_OBJECT
public:
  explicit TableEditFieldDescriptor(QObject *parent = nullptr);
  TableEditFieldDescriptor(const TableEditFieldDescriptor& fieldDescriptor);
  explicit TableEditFieldDescriptor(const TableEditFieldDescriptor& fieldDescriptor, QObject *parent);
  explicit TableEditFieldDescriptor(const QString& tableName, const QString& fieldName, const QString& displayName, const QString& separator="/", const bool readOnly=false, QObject *parent = nullptr);

  QString getFieldName() const { return m_fieldName;}
  void setFieldName(const QString& name) { m_fieldName = name; }

  QString getTableName() const { return m_tableName;}
  void setTableName(const QString& name) { m_tableName = name; }

  QString getDisplayName() const { return m_displayName;}
  void setDisplayName(const QString& name) { m_displayName = name; }

  QString getFieldDisplaySeparator() const { return m_fieldDisplaySeparator;}
  void setFieldDisplaySeparator(const QString& separator) { m_fieldDisplaySeparator = separator; }

  bool isReadOnly() const { return m_readOnly;}
  void setReadOnly(const bool readOnly) { m_readOnly = readOnly; }

  //**************************************************************************
  /*! \brief If this is a link field, then display value from another table.
   *
   *  \return True if there are any linked fields.
   ***************************************************************************/
  bool isLinked() const { return linkFieldSize() > 0; }

  int linkFieldSize() const { return m_fields.size(); }

  const TableEditFieldDescriptor& getLinkField(const int i) const { return m_fields.at(i); }

  void append(const TableEditFieldDescriptor& descriptor) { m_fields.append(descriptor); }

  void append(const QList<TableEditFieldDescriptor>& descriptors) { m_fields.append(descriptors); }

  void clearDescriptors() { m_fields.clear(); }

  const TableEditFieldDescriptor& operator=(const TableEditFieldDescriptor& o) { return copy(o); }

  // Read / write XMl is not tested.

  /*! \brief Write this object to an XML stream.
   *  \param [in, out] writer Write to this XML stream.
   *  \return reference to the writer stream.
   */
  QXmlStreamWriter& writeXml(QXmlStreamWriter& writer) const;

  /*! \brief Read and return an object of this type. Fields not included in the XML use the default values.
   *  \param [in, out] reader Read from this XML stream.
   *  \return A new field that was just read.
   */
  static TableEditFieldDescriptor readXml(QXmlStreamReader& reader);

signals:

public slots:

private:
  const TableEditFieldDescriptor& copy(const TableEditFieldDescriptor& o);
  static bool atDescriptor(QXmlStreamReader& reader);

  // Is this field read-only?
  bool m_readOnly;

  // If multiple fields are displayed, this is used to separate the fields.
  QString m_fieldDisplaySeparator;

  // Table name containing this field.
  QString m_tableName;

  // Field name in table.
  QString m_fieldName;

  // Name as displayed to the user.
  QString m_displayName;

  // If empty, display value from the field itself.
  // If not empty, concatenate field values together.
  QList<TableEditFieldDescriptor> m_fields;

};

inline QXmlStreamWriter& operator<<(QXmlStreamWriter& writer, const TableEditFieldDescriptor& descriptor)
{
  return descriptor.writeXml(writer);
}

#endif // TABLEEDITFIELDDESCRIPTOR_H
