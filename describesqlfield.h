#ifndef DESCRIBESQLFIELD_H
#define DESCRIBESQLFIELD_H

//**************************************************************************
//! Used to describe what a single field looks like for display and housekeeping.
/*!
 *
 **************************************************************************/

#include <QString>
#include "sqlfieldtype.h"

class DescribeSqlField
{
public:
  DescribeSqlField();

  DescribeSqlField(const DescribeSqlField& field);

  QString getName() const { return m_name; }
  QString getViewName() const {return m_viewName; }
  QString getDescription() const { return m_description; }
  SqlFieldType getFieldType() const { return m_fieldType; }
  QString getPreferredTypeName() const { return m_preferredTypeName; }
  QString getLinkTableName() const { return m_linkTable; }
  QString getLinkeFieldName() const { return m_linkField; }
  bool isAutoIncrement() const { return m_isAutoIncrement; }
  bool isRequired() const { return m_isRequired; }
  bool isKey() const { return m_isKey; }
  bool isLinkField() const;
  int getFieldLength() const { return m_fieldLength; }

  void setName(const QString& name) { m_name = name; }
  void setViewName(const QString& viewName) { m_viewName = viewName; }
  void setDescription(const QString& description) { m_description = description; }
  void setFieldType(const SqlFieldType& fieldType) { m_fieldType = fieldType; }
  void setPreferredTypeName(const QString& preferredName) { m_preferredTypeName = preferredName; }
  void setLinkTableName(const QString& linkTableName) { m_linkTable = linkTableName; }
  void setLinkFieldName(const QString& linkFieldName) { m_linkField = linkFieldName; }
  void setIsAutoIncrement(const bool isAutoInc) { m_isAutoIncrement = isAutoInc; }
  void setIsRequired(const bool isReq) { m_isRequired = isReq; }
  void setIsKey(const bool isK) { m_isKey = isK; }
  void setFieldLength(const int len) { m_fieldLength = len; }

  const DescribeSqlField& operator=(const DescribeSqlField& field) { return copy(field); }

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

  /*! \brief Name of the field to which this field is linked in the associated table. */
  QString m_linkField;

  /*! \brief True if this field is auto-increment. */
  bool m_isAutoIncrement;

  /*! \brief True if this field is required. */
  bool m_isRequired;

  /*! \brief True if this is a key field for the table. */
  bool m_isKey;

  int m_fieldLength;
};

#endif // DESCRIBESQLFIELD_H
