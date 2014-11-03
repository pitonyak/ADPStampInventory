#ifndef DESCRIBESQLTABLES_H
#define DESCRIBESQLTABLES_H

#include "describesqltable.h"
#include <QHash>
#include <QStringList>
#include <QString>

class QXmlStreamWriter;
class QXmlStreamReader;

class DescribeSqlTables
{
public:
  DescribeSqlTables();

  DescribeSqlTables(const DescribeSqlTables& tables);

  QString getName() const { return m_name; }
  QString getViewName() const {return m_viewName; }
  QString getDescription() const { return m_description; }

  void setName(const QString& name) { m_name = name; }
  void setViewName(const QString& viewName) { m_viewName = viewName; }
  void setDescription(const QString& description) { m_description = description; }

  int getTableCount() const { return m_names.count(); }
  QStringList getTableNames() const { return m_names; }
  QString getNameByIndex(const int index) const;
  bool hasTable(const QString& name) const;

  DescribeSqlTable getTableByName(const QString& name) const;
  DescribeSqlTable getTableByIndex(const int index) const;
  bool addTable(const DescribeSqlTable& table);

  QMetaType::Type getFieldMetaType(const QString& tableName, const QString& fieldName) const { return getTableByName(tableName).getFieldMetaType(fieldName); }

  /*! \brief Perform a deep copy of the parameter into this object.
   *
   *  \param [in] tables Object from which to copy.
   *  \return a reference to this object.
   */
  const DescribeSqlTables& operator=(const DescribeSqlTables& tables) { return copy(tables); }

  /*! \brief Return a description of how the schema should look.
   *
   *  \return A stamp schema object describing the fields and such as desired.
   */
  static DescribeSqlTables getStampSchema();

  QXmlStreamWriter& writeXml(QXmlStreamWriter& writer) const;

  static DescribeSqlTables readXml(QXmlStreamReader& reader);

  //**************************************************************************
  //! Get the DDL usable to create the tables known to this object; one table per string.
  /*!
   * \param prettyPrint If true, each field is on its own line; if not, each field is separated by a comman and a space.
   * \returns DDL usable to create the tables known to this object; one table per string.
   *
   ***************************************************************************/
  QStringList getDDL(const bool prettyPrint) const;

private:

  const DescribeSqlTables& copy(const DescribeSqlTables& tables);

  /*! \brief Simple unique name as used by the database; probably all lower case. */
  QString m_name;

  /*! \brief Table name as shown to the user, probably has upper and lower case. */
  QString m_viewName;

  /*! \brief Describe what the table contains. */
  QString m_description;

  QStringList m_names;
  QHash<QString, DescribeSqlTable> m_tables;
};

inline bool DescribeSqlTables::hasTable(const QString& name) const
{
  return m_tables.contains(name.toLower());
}

#endif // DESCRIBESQLTABLES_H
