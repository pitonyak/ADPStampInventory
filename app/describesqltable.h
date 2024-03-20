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
    //**************************************************************************
    /*! \brief Default constructor
     ***************************************************************************/
  DescribeSqlTable();

  //**************************************************************************
  /*! \brief Copy constructor
   *  \param [in] table object to copy.
   ***************************************************************************/
  DescribeSqlTable(const DescribeSqlTable& table);

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  DescribeSqlTable(const QString definitions[], const int n, bool idIsAutoInc = true ,const SqlFieldTypeMaster* typemaster = nullptr);

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  QString getFirstKeyFieldName() const;

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  QStringList getKeyFieldNames() const;

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  QString getName() const { return m_name; }

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  QString getViewName() const {return m_viewName; }

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  QString getDescription() const { return m_description; }

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  void setName(const QString& name) { m_name = name; }

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  void setViewName(const QString& viewName) { m_viewName = viewName; }

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  void setDescription(const QString& description) { m_description = description; }

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  int getFieldCount() const { return m_names.size(); }

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  QStringList getFieldNames() const { return m_names; }

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  QString getFieldNameByIndex(const int index) const;

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  bool containsField(const QString& name) const;

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  const DescribeSqlField* getFieldByName(const QString& name) const;

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  const DescribeSqlField* getFieldByIndex(const int index) const;

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  bool addField(const DescribeSqlField& field);

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  QMetaType::Type getFieldMetaType(const QString& name) const;

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  QMetaType::Type getFieldMetaType(const int i) const;

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  const DescribeSqlTable& operator=(const DescribeSqlTable& table) { return copy(table); }

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  void setFieldKey(const QString& name, bool x);

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  void setFieldAutoIncrement(const QString& name, bool x);

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  void setFieldRequired(const QString& name, bool x);

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  void setFieldLink(const QString& name, const QString& linkTableName, const QString& linkFieldName, const QString& linkDisplayFields);

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  void setFieldCurrencySymbol(const QString& name, const QString& currencySymbol);

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  QSet<QString> getLinkedTableNames() const;

  //**************************************************************************
  /*! \brief
   *
   *  \param [in]
   *  \return
   ***************************************************************************/
  QXmlStreamWriter& writeXml(QXmlStreamWriter& writer) const;

  //**************************************************************************
  /*! \brief Read an XML representation of a table.
   *
   *  \param [in] reader
   *  \return Table that was read
   ***************************************************************************/
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

  //**************************************************************************
  /*! \brief Copy a table into this object.
   *
   *  \param [in] table Object to copy into this one.
   *  \return Reference to this object.
   ***************************************************************************/
  const DescribeSqlTable& copy(const DescribeSqlTable& table);

  /*! \brief Simple unique name as used by the database; probably all lower case. */
  QString m_name;

  /*! \brief Table name as shown to the user, probably has upper and lower case. */
  QString m_viewName;

  /*! \brief Describe what the table contains. */
  QString m_description;

  /*! \brief Lowercase list of names. */
  QStringList m_names;

  /*! \brief Map lower case field names to the field objects. */
  QHash<QString, DescribeSqlField> m_fields;
};

inline bool DescribeSqlTable::containsField(const QString& name) const
{
  return m_fields.contains(name.toLower());
}

#endif // DESCRIBESQLTABLE_H
