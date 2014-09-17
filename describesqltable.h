#ifndef DESCRIBESQLTABLE_H
#define DESCRIBESQLTABLE_H

//**************************************************************************
//! Used to describe what a table looks like for display and housekeeping.
/*!
 *
 **************************************************************************/

#include <QString>
#include <QStringList>
#include <QHash>
#include "describesqlfield.h"

class SqlFieldTypeMaster;

class DescribeSqlTable
{
public:
  DescribeSqlTable();
  DescribeSqlTable(const DescribeSqlTable& table);

  DescribeSqlTable(const char* const definitions[], bool idIsAutoInc = true ,const SqlFieldTypeMaster* typemaster = nullptr);


  QString getName() const { return m_name; }
  QString getViewName() const {return m_viewName; }
  QString getDescription() const { return m_description; }

  void setName(const QString& name) { m_name = name; }
  void setViewName(const QString& viewName) { m_viewName = viewName; }
  void setDescription(const QString& description) { m_description = description; }

  int getFieldCount() const { return m_Names.count(); }
  QStringList getFieldNames() const { return m_Names; }
  QString getFieldNameByIndex(const int index) const;

  DescribeSqlField getFieldByName(const QString& name) const;
  DescribeSqlField getFieldByIndex(const int index) const;
  bool addField(const DescribeSqlField& field);

  const DescribeSqlTable& operator=(const DescribeSqlTable& table) { return copy(table); }

  void setFieldKey(const QString& name, bool x);
  void setFieldAutoIncrement(const QString& name, bool x);
  void setFieldRequired(const QString& name, bool x);
  void setFieldLink(const QString& name, const QString& linkTableName, const QString& linkFieldName);

private:

  const DescribeSqlTable& copy(const DescribeSqlTable& table);

  /*! \brief Simple unique name as used by the database; probably all lower case. */
  QString m_name;

  /*! \brief Table name as shown to the user, probably has upper and lower case. */
  QString m_viewName;

  /*! \brief Describe what the table contains. */
  QString m_description;

  QStringList m_Names;
  QHash<QString, DescribeSqlField> m_fields;
};

#endif // DESCRIBESQLTABLE_H
