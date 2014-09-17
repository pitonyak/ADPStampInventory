#include "describesqltables.h"
#include "sqlfieldtypemaster.h"

DescribeSqlTables::DescribeSqlTables()
{
}


DescribeSqlTables::DescribeSqlTables(const DescribeSqlTables& tables)
{
  copy(tables);
}

const DescribeSqlTables& DescribeSqlTables::copy(const DescribeSqlTables& tables)
{
  if (this != &tables) {
    setName(tables.getName());
    setViewName(tables.getViewName());
    setDescription(tables.getDescription());
    m_Names = tables.getTableNames();
    m_tables = tables.m_tables;
  }
  return *this;
}

QString DescribeSqlTables::getNameByIndex(const int index) const
{
  if (0 <= index && index < getTableCount()) {
    return m_Names.at(index);
  } else {
    qDebug(qPrintable(QString("Field index = %1 is out of range for table %2").arg(index).arg(getName())));
  }
  return "";
}

DescribeSqlTable DescribeSqlTables::getTableByName(const QString& name) const
{
  QString simpleName = name.toLower();
  if (m_tables.contains(simpleName)) {
    return m_tables.value(simpleName);
  } else {
    qDebug(qPrintable(QString("Table name = '%1' is contained in table set %2").arg(name).arg(getName())));
  }
  return DescribeSqlTable();
}

DescribeSqlTable DescribeSqlTables::getTableByIndex(const int index) const
{
  return getTableByName(getNameByIndex(index));
}

bool DescribeSqlTables::addTable(const DescribeSqlTable& table)
{
  QString simpleName = table.getName().toLower();
  if (m_tables.contains(simpleName)) {
    qDebug(qPrintable(QString("Table name = '%1' is already contained in table set %2").arg(simpleName).arg(getName())));
    return false;
  } else {
    m_Names.append(simpleName);
    m_tables.insert(simpleName, table);
  }
  return true;
}

DescribeSqlTables DescribeSqlTables::getStampSchema()
{
  DescribeSqlTables schema;

  SqlFieldTypeMaster typeMaster;

  // Name, View Name, Type, Description, length
  const char* const catalogType[] = {"catalogtype", "Catalog Type", "Stamp category such as Air Mail, Postage, or Envelope",
             "id", "Id", "INTEGER", "Table Key", "10",
             "name", "Name", "VARCHAR", "Viewable name such as Air Mail or Postage", "20",
             "description", "Description", "Longer descriptive name", "100"};
  DescribeSqlTable catalogTypeTable(catalogType, true, &typeMaster);
  schema.addTable(catalogTypeTable);

  const char* const country[] = {"country", "Country", "Country",
             "id", "Id", "INTEGER", "Table Key", "10",
             "name", "Name", "VARCHAR", "Viewable name such as Air Mail or Postage", "50",
             "a3", "A3", "Three letter country designation", "3"};
  schema.addTable(DescribeSqlTable(country, true, &typeMaster));

  const char* const valueType[] = {"valuetype", "Value Type", "Categorizes the stamp for value such as mint or used.",
             "id", "Id", "INTEGER", "Table Key", "10",
             "description", "Description", "Describes the type", "100"};
  schema.addTable(DescribeSqlTable(valueType, true, &typeMaster));

  const char* const valueSource[] = {"valuesource", "Value Source", "Categorizes the stamp for value such as mint or used.",
             "id", "Id", "INTEGER", "Table Key", "10",
             "year", "Date", "DATE", "Date for the book from which the values were taken in the format MM/DD/YYYY", "10",
             "description", "Description", "Longer descriptive name such as Scotts Catelog", "100"};
  schema.addTable(DescribeSqlTable(valueSource, true, &typeMaster));

  const char* const catalog[] = {"catalog", "Catalog", "Catalog of stamp definitions.",
             "id", "Id", "INTEGER", "Table Key", "10",
             "scott", "Scott", "VARCHAR", "Scott Number", "10",
             "countryid", "Country Id", "INTEGER", "Country or origin", "10",
             "typeid", "Type Id", "INTEGER", "Stamp type such as Air Mail", "10",
             "releasedate", "Released", "DATE", "Date the stamp was released MM/DD/YYYY", "10",
             "updated", "Updated", "TIMESTAMP", "Date and time this record was last updated MM/DD/YYYY hh:mm:ss", "19",
             "facevalue", "Face Value", "DOUBLE", "Denomination on the stamp",
             "description", "Description", "Describe the stamp", "200"};
  DescribeSqlTable catalogTable(catalog, true, &typeMaster);
  catalogTable.setFieldLink("countryid", "country", "id");
  catalogTable.setFieldLink("typeid", "catalogtype", "id");
  schema.addTable(catalogTable);

  const char* const bookValues[] = {"bookvalues", "Book Values", "What is each stamp worth.",
             "id", "Id", "INTEGER", "Table Key", "10",
             "catalogid", "Catalog Id", "INTEGER", "Catalog from which the value was pulled", "10",
             "sourceid", "Source Id", "INTEGER", "?? What is this? Remove it?", "10",
             "valuetypeid", "Value Type Id", "INTEGER", "Value categorization such as mint or used.", "10",
             "releasedate", "Released", "DATE", "Date the stamp was released MM/DD/YYYY", "10",
             "updated", "Updated", "TIMESTAMP", "Date and time this record was last updated MM/DD/YYYY hh:mm:ss", "19",
             "facevalue", "Face Value", "DOUBLE", "Denomination on the stamp",
             "description", "Description", "Describe the stamp", "200"};
  DescribeSqlTable bookValuesTable(bookValues, true, &typeMaster);
  bookValuesTable.setFieldLink("countryid", "country", "id");
  bookValuesTable.setFieldLink("typeid", "catalogtype", "id");
  schema.addTable(bookValuesTable);




  //qDebug(qPrintable(QString("Cannot build a length from '%1'").arg(definitions[i-1])));
  return schema;
}

