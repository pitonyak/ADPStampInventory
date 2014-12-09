#ifndef DESCRIBESQLTABLES_H
#define DESCRIBESQLTABLES_H

#include "describesqltable.h"
#include "tableeditfielddescriptors.h"
#include <QHash>
#include <QStringList>
#include <QString>

class QXmlStreamWriter;
class QXmlStreamReader;

//**************************************************************************
/*! \class DescribeSqlTables
 *
 * \brief Describe the schema / form of a set of tables.
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2014-2014
 ***************************************************************************/
class DescribeSqlTables
{
public:
    //**************************************************************************
    /*! \brief Default constructor
     ***************************************************************************/
  DescribeSqlTables();

  //**************************************************************************
  /*! \brief Copy constructor
   *  \param [in] tables object to copy.
   ***************************************************************************/
  DescribeSqlTables(const DescribeSqlTables& tables);

  //**************************************************************************
  /*! \brief Not used for anything at this time.
   *  \return The object's name.
   ***************************************************************************/
  QString getName() const { return m_name; }

  //**************************************************************************
  /*! \brief Mixed case potentially fancy object name. Not used for anything at this time.
   *  \return Get the view name for this object.
   ***************************************************************************/
  QString getViewName() const {return m_viewName; }

  //**************************************************************************
  /*! \brief
   *  \return Get the object description.
   ***************************************************************************/
  QString getDescription() const { return m_description; }

  //**************************************************************************
  /*! \brief Set the name to use for this object.
   *
   *  \param [in] name
   ***************************************************************************/
  void setName(const QString& name) { m_name = name; }

  //**************************************************************************
  /*! \brief Set the view name for this object.
   *
   *  \param [in] viewName
   ***************************************************************************/
  void setViewName(const QString& viewName) { m_viewName = viewName; }

  //**************************************************************************
  /*! \brief Set the description for this object.
   *
   *  \param [in] description New description to use.
   ***************************************************************************/
  void setDescription(const QString& description) { m_description = description; }

  //**************************************************************************
  /*! \brief How many tables are known to this object.
   *
   *  \return Number of know tables.
   ***************************************************************************/
  int getTableCount() const { return m_names.size(); }

  //**************************************************************************
  /*! \brief Get a list of all table names.
   *
   *  \return List of all table names.
   ***************************************************************************/
  QStringList getTableNames() { return m_names; }

  //**************************************************************************
  /*! \brief Get a list of all table names.
   *
   *  \return List of all table names.
   ***************************************************************************/
  const QStringList& getTableNames() const { return m_names; }


  //**************************************************************************
  /*! \brief Get a table name based on an index, which allows iterating through the table names.
   *
   *  \param [in] index Which table is desired; inde <= 0 < getTableCount().
   *  \return Table name of interest or "" if outside the range.
   ***************************************************************************/
  QString getNameByIndex(const int index) const;

  //**************************************************************************
  /*! \brief Determine if a table is contained in this set.
   *
   *  \param [in] name Table name of interest.
   *  \return True if the table is contained in this object.
   ***************************************************************************/
  bool containsTable(const QString& name) const;

  //**************************************************************************
  /*! \brief Get a list of all tables linked to the specified table.
   *
   *  \param [in] tableName Table of interest.
   *  \return A set of table names that are linked by fields in the specified table.
   ***************************************************************************/
  QSet<QString> getLinkedTableNames(const QString& tableName) const;

  /*! \brief Get a table schema based on the tables name.
   *
   * This is called often so a pointer is returned since that is faster than using a copy constuctor.
   *
   *  \param [in] tableName Name of the table for which a schema is desired.
   *  \return a pointer to the table schema, or, nullptr if the table name is not contained.
   */
  const DescribeSqlTable* getTableByName(const QString& tableName) const;

  /*! \brief Get a table schema based on the tables index in the list (so that all tables can be easily enumerated).
   *
   * This is called often so a pointer is returned since that is faster than using a copy constuctor.
   *
   *  \param [in] index Index from 0 to count -1 of the table.
   *  \return a pointer to the table schema, or, nullptr if the table index is not valid.
   */
  const DescribeSqlTable* getTableByIndex(const int index) const;

  //**************************************************************************
  /*! \brief Add the table to this list. A copy is made so you can destroy the original if desired.
   *
   * This adds the table name to the list of names.
   * this will NOT replace an existing table.
   *
   *  \param [in] table Table to add.
   *  \return True if added, false otherwise.
   ***************************************************************************/
  bool addTable(const DescribeSqlTable& table);

  QMetaType::Type getFieldMetaType(const QString& tableName, const QString& fieldName) const;

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

  static QHash<QString, TableEditFieldDescriptors> getPrimaryTableEditDescriptors();

  //**************************************************************************
  /*! \brief Write this object as XML to a stream.
   *
   *  \param [in] writer XML stream used for writing.
   *  \return Reference to the writer.
   ***************************************************************************/
  QXmlStreamWriter& writeXml(QXmlStreamWriter& writer) const;

  //**************************************************************************
  /*! \brief Read a set of tables from XML.
   *
   *  \param [in] reader Stream from which to read the XML.
   *  \return A copy of the data.
   ***************************************************************************/
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

  //**************************************************************************
  /*! \brief Copy a set of tables into this object.
   *
   *  \param [in] tables Object to copy into this one.
   *  \return Reference to this object.
   ***************************************************************************/
  const DescribeSqlTables& copy(const DescribeSqlTables& tables);

  /*! \brief Simple unique name as used by the database; probably all lower case. */
  QString m_name;

  /*! \brief Special name for this collection of tables shown to the user, probably has upper and lower case. */
  QString m_viewName;

  /*! \brief Describe this set. */
  QString m_description;

  /*! \brief List of table names in lower case. */
  QStringList m_names;

  /*! \brief Collection of tables mapped using the lower case table name to the table object. */
  QHash<QString, DescribeSqlTable> m_tables;
};

inline bool DescribeSqlTables::containsTable(const QString& name) const
{
  return m_tables.contains(name.toLower());
}

#endif // DESCRIBESQLTABLES_H
