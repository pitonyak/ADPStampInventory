#ifndef DESCRIBESQLFIELD_H
#define DESCRIBESQLFIELD_H

#include <QString>
#include "sqlfieldtype.h"

class QXmlStreamWriter;
class QXmlStreamReader;

//**************************************************************************
/*! \class DescribeSqlField
 * \brief Used to describe what a single field looks like for display and housekeeping.
 *
 * Gives a field a name, type, and other things used to describe a field.
 * This object is intended to live inside of a DescribeSqlTable.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date September 2014
 **************************************************************************/
class DescribeSqlField
{
public:
  /*! \brief Constructor
   *  \param [in, out] parent The object's owner. The parent's destructor destroys this object.
   */
  DescribeSqlField();

  /*! \brief Copy Constructor
   *  \param [in] field Object to copy.
   */
  DescribeSqlField(const DescribeSqlField& field);

  /*! \brief Get the fields name as used in the database. */
  QString getName() const { return m_name; }

  /*! \brief Get the fields display name, which is probably more user friendly. */
  QString getViewName() const {return m_viewName; }

  /*! \brief Get the field's description, which is probably suitable for pop-up help or similar. */
  QString getDescription() const { return m_description; }

  /*! \brief Get the fields type. */
  SqlFieldType getFieldType() const { return m_fieldType; }

  /*! \brief Get the fields preferred type name in case some mapping causes more than one type to be mapped to this field. */
  QString getPreferredTypeName() const { return m_preferredTypeName; }

  /*! \brief If this field is a foreign key to another table, this returns that tables name. */
  QString getLinkTableName() const { return m_linkTable; }

  /*! \brief If this field is a foreign key to another table, this returns the key field name in the linked table. */
  QString getLinkFieldName() const { return m_linkField; }

  /*! \brief If this is currency, return an appropriate currency symbol, such as "$". */
  QString getCurrencySymbol() const { return m_currencySymbol; }
  bool isAutoIncrement() const { return m_isAutoIncrement; }
  bool isRequired() const { return m_isRequired; }

  /*! \brief Is this a key field, which implies it can be used as a foreign key, and that it cannot be changed. */
  bool isKey() const { return m_isKey; }

  /*! \brief Determines if this field contains a foreign key by inspecting the link table name and the link field name.
   *  \return True if this field links to another field (foreign key constraint).
   */
  bool isLinkField() const;

  /*! \brief Determines if this is a currency field by inspecting the currency symbol to see if it is empty. */
  bool isCurrency() const { return !m_currencySymbol.isEmpty(); }

  int getFieldLength() const { return m_fieldLength; }
  int getFieldPrecision() const { return m_fieldPrecision; }

  void setName(const QString& name) { m_name = name; }
  void setViewName(const QString& viewName) { m_viewName = viewName; }
  void setDescription(const QString& description) { m_description = description; }
  void setFieldType(const SqlFieldType& fieldType) { m_fieldType = fieldType; }
  void setPreferredTypeName(const QString& preferredName) { m_preferredTypeName = preferredName; }
  void setLinkTableName(const QString& linkTableName) { m_linkTable = linkTableName; }
  void setLinkFieldName(const QString& linkFieldName) { m_linkField = linkFieldName; }
  void setCurrencySymbol(const QString& currencySymbol) { m_currencySymbol = currencySymbol; }
  void setIsAutoIncrement(const bool isAutoInc) { m_isAutoIncrement = isAutoInc; }
  void setIsRequired(const bool isReq) { m_isRequired = isReq; }
  void setIsKey(const bool isK) { m_isKey = isK; }
  void setFieldLength(const int len) { m_fieldLength = len; }
  void setFieldPrecision(const int len) { m_fieldPrecision = len; }

  const DescribeSqlField& operator=(const DescribeSqlField& field) { return copy(field); }

  QXmlStreamWriter& writeXml(QXmlStreamWriter& writer) const;

  /*! \brief Read and return an object of this type. Fields not included in the XML use the default values.
   *  \param [in, out] reader Read from this XML stream.
   *  \return A new field that was just read.
   */
  static DescribeSqlField readXml(QXmlStreamReader& reader);

  QString generateDDL() const;

private:

  const DescribeSqlField& copy(const DescribeSqlField& field);

  /*! \brief Simple unique name as used by the database; probably all lower case. */
  QString m_name;

  /*! \brief Table name as shown to the user, probably has upper and lower case. */
  QString m_viewName;

  /*! \brief Describe what the table contains. */
  QString m_description;

  /*! \brief Type associated with this field. */
  SqlFieldType m_fieldType;

  /*! \brief In case there is more than one name. */
  QString m_preferredTypeName;

  /*! \brief Name of the table to which this field is linked. */
  QString m_linkTable;

  /*! \brief Name of the field to which this field is linked in the associated link table. */
  QString m_linkField;

  /*! \brief If not empty, then this is a currency field. */
  QString m_currencySymbol;

  /*! \brief True if this field is auto-increment. */
  bool m_isAutoIncrement;

  /*! \brief True if this field is required. */
  bool m_isRequired;

  /*! \brief True if this is a key field for the table. */
  bool m_isKey;

  int m_fieldLength;

  int m_fieldPrecision;
};

#endif // DESCRIBESQLFIELD_H
