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

  DescribeSqlTable getTableByName(const QString& name) const;
  DescribeSqlTable getTableByIndex(const int index) const;
  bool addTable(const DescribeSqlTable& table);

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

#endif // DESCRIBESQLTABLES_H
