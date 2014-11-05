#ifndef DESCRIBESQLTABLE_H
#define DESCRIBESQLTABLE_H

#include <QString>
#include <QStringList>
#include <QHash>
#include <QSet>
#include "describesqlfield.h"

class SqlFieldTypeMaster;
class QXmlStreamWriter;
class QXmlStreamReader;

//**************************************************************************
/*! \class DescribeSqlTable
 * \brief Used to describe a table as a collection of fields.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date September 2014
 **************************************************************************/
class DescribeSqlTable
{
public:
  DescribeSqlTable();
  DescribeSqlTable(const DescribeSqlTable& table);

  DescribeSqlTable(const QString definitions[], const int n, bool idIsAutoInc = true ,const SqlFieldTypeMaster* typemaster = nullptr);

  QString getFirstKeyFieldName() const;
  QStringList getKeyFieldNames() const;

  QString getName() const { return m_name; }
  QString getViewName() const {return m_viewName; }
  QString getDescription() const { return m_description; }

  void setName(const QString& name) { m_name = name; }
  void setViewName(const QString& viewName) { m_viewName = viewName; }
  void setDescription(const QString& description) { m_description = description; }

  int getFieldCount() const { return m_names.count(); }
  QStringList getFieldNames() const { return m_names; }
  QString getFieldNameByIndex(const int index) const;
  bool containsField(const QString& name) const;

  DescribeSqlField getFieldByName(const QString& name) const;
  DescribeSqlField getFieldByIndex(const int index) const;
  bool addField(const DescribeSqlField& field);

  QMetaType::Type getFieldMetaType(const QString& name) const { return getFieldByName(name).getFieldMetaType(); }
  QMetaType::Type getFieldMetaType(const int i) const { return getFieldByIndex(i).getFieldMetaType(); }

  const DescribeSqlTable& operator=(const DescribeSqlTable& table) { return copy(table); }

  void setFieldKey(const QString& name, bool x);
  void setFieldAutoIncrement(const QString& name, bool x);
  void setFieldRequired(const QString& name, bool x);
  void setFieldLink(const QString& name, const QString& linkTableName, const QString& linkFieldName, const QString& linkDisplayFields);
  void setFieldCurrencySymbol(const QString& name, const QString& currencySymbol);

  QSet<QString> getLinkedTableNames() const;

  QXmlStreamWriter& writeXml(QXmlStreamWriter& writer) const;
  static DescribeSqlTable readXml(QXmlStreamReader& reader);

  //**************************************************************************
  //! Generate the DDL usable to create this table.
  /*!
   * \param prettyPrint If true, each field is on its own line; if not, each field is separated by a comman and a space.
   * \returns DDL usable to create this table.
   *
   ***************************************************************************/
  QString getDDL(const bool prettyPrint) const;

private:

  const DescribeSqlTable& copy(const DescribeSqlTable& table);

  /*! \brief Simple unique name as used by the database; probably all lower case. */
  QString m_name;

  /*! \brief Table name as shown to the user, probably has upper and lower case. */
  QString m_viewName;

  /*! \brief Describe what the table contains. */
  QString m_description;

  QStringList m_names;
  QHash<QString, DescribeSqlField> m_fields;
};

inline bool DescribeSqlTable::containsField(const QString& name) const
{
  return m_fields.contains(name.toLower());
}

#endif // DESCRIBESQLTABLE_H
