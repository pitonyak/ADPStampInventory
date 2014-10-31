#include "describesqltables.h"
#include "sqlfieldtypemaster.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>

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
    m_names = tables.getTableNames();
    m_tables = tables.m_tables;
  }
  return *this;
}

QString DescribeSqlTables::getNameByIndex(const int index) const
{
  if (0 <= index && index < getTableCount()) {
    return m_names.at(index);
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
    m_names.append(simpleName);
    m_tables.insert(simpleName, table);
  }
  return true;
}

DescribeSqlTables DescribeSqlTables::readXml(QXmlStreamReader& reader)
{
    DescribeSqlTables tables;
    bool foundTablesTag = false;
    while (!reader.atEnd()) {
        if (reader.isStartElement()) {
            if (reader.name().compare("Tables", Qt::CaseInsensitive) == 0) {
                if (foundTablesTag) {
                    // Found a second Tables tag.
                    break;
                }
                foundTablesTag = true;
                if (reader.attributes().hasAttribute("name"))
                    tables.setName(reader.attributes().value("name").toString());
                if (reader.attributes().hasAttribute("viewname"))
                    tables.setViewName(reader.attributes().value("viewname").toString());
                if (reader.attributes().hasAttribute("description"))
                    tables.setDescription(reader.attributes().value("description").toString());
                reader.readNext();
            } else if (reader.name().compare("Table", Qt::CaseInsensitive) == 0) {
                DescribeSqlTable table = DescribeSqlTable::readXml(reader);
                if (table.getName().isEmpty() || !tables.addTable(table)) {
                    qDebug(qPrintable(QString("Failed to add Table name = '%1'").arg(table.getName())));
                    break;
                }
            } else {
                // Unexpected element, what to do!
                qDebug(qPrintable(QString("Found unexpected XML element %1").arg(reader.name().toString())));
                break;
            }
        } else if (reader.isStartDocument()) {
            reader.readNext();
        } else if (reader.isEndElement()) {
            if (foundTablesTag && reader.name().compare("Tables", Qt::CaseInsensitive) == 0) {
                reader.readNext();
                break;
            }
            reader.readNext();
        } else {
            reader.readNext();
        }
    }
    return tables;
}

QXmlStreamWriter& DescribeSqlTables::writeXml(QXmlStreamWriter& writer) const
{
  writer.writeStartElement("Tables");
  if (!getName().isEmpty())
    writer.writeAttribute("name", getName());
  if (!getViewName().isEmpty())
    writer.writeAttribute("viewname", getViewName());
  if (!getDescription().isEmpty()) {
    writer.writeAttribute("description", getDescription());
  }
  for (int i=0; i<m_names.size(); ++i) {
    m_tables.value(m_names.at(i)).writeXml(writer);
  }
  writer.writeEndElement();
  return writer;
}

DescribeSqlTables DescribeSqlTables::getStampSchema()
{
  DescribeSqlTables schema;

  SqlFieldTypeMaster typeMaster;

  qDebug("Ready for catalog type");
  // Name, View Name, Type, Description, length

  QString catalogType[] = {"catalogtype", "Catalog Type", "Stamp category such as Air Mail, Postage, or Envelope",
             "id", "Id", "INTEGER", "Table Key", "10",
             "name", "Name", "VARCHAR", "Viewable name such as Air Mail or Postage", "20",
             "description", "Description", "VARCHAR", "Longer descriptive name", "100"};

  int n=sizeof(catalogType) / sizeof(catalogType[0]);
  schema.addTable(DescribeSqlTable(catalogType, n, true, &typeMaster));
  qDebug("Done with catalog type");

  QString country[] = {"country", "Country", "Country",
             "id", "Id", "INTEGER", "Table Key", "10",
             "name", "Name", "VARCHAR", "Viewable name such as Air Mail or Postage", "50",
             "a3", "A3", "VARCHAR", "Three letter country designation", "3"};
  n=sizeof(country) / sizeof(country[0]);
  schema.addTable(DescribeSqlTable(country, n, true, &typeMaster));

  QString  valueType[] = {"valuetype", "Value Type", "Categorizes the stamp for value such as mint or used.",
             "id", "Id", "INTEGER", "Table Key", "10",
             "description", "Description", "VARCHAR", "Describes the type", "100"};
  n=sizeof(valueType) / sizeof(valueType[0]);
  schema.addTable(DescribeSqlTable(valueType, n, true, &typeMaster));

  QString valueSource[] = {"valuesource", "Value Source", "Categorizes the stamp for value such as mint or used.",
             "id", "Id", "INTEGER", "Table Key", "10",
             "year", "Date", "DATE", "Date for the book from which the values were taken in the format MM/DD/YYYY", "10",
             "description", "Description", "VARCHAR", "Longer descriptive name such as Scotts Catelog", "100"};
  n=sizeof(valueSource) / sizeof(valueSource[0]);
  schema.addTable(DescribeSqlTable(valueSource, n, true, &typeMaster));

  QString catalog[] = {"catalog", "Catalog", "Catalog of stamp definitions.",
             "id", "Id", "INTEGER", "Table Key", "10",
             "scott", "Scott", "VARCHAR", "Scott Number", "10",
             "countryid", "Country Id", "INTEGER", "Country or origin", "10",
             "typeid", "Type Id", "INTEGER", "Stamp type such as Air Mail", "10",
             "releasedate", "Released", "DATE", "Date the stamp was released MM/DD/YYYY", "10",
             "updated", "Updated", "TIMESTAMP", "Date and time this record was last updated MM/DD/YYYY hh:mm:ss", "19",
             "facevalue", "Face Value", "DOUBLE", "Denomination on the stamp", "10",
             "description", "Description", "VARCHAR", "Describe the stamp", "200"};
  n=sizeof(catalog) / sizeof(catalog[0]);
  DescribeSqlTable catalogTable(catalog, n, true, &typeMaster);
  catalogTable.setFieldLink("countryid", "country", "id");
  catalogTable.setFieldLink("typeid", "catalogtype", "id");
  catalogTable.setFieldCurrencySymbol("facevalue", "$");
  schema.addTable(catalogTable);

  QString bookValues[] = {"bookvalues", "Book Values", "What is each stamp worth.",
             "id", "Id", "INTEGER", "Table Key", "10",
             "catalogid", "Catalog Id", "INTEGER", "Catalog from which the value was pulled", "10",
             "sourceid", "Source Id", "INTEGER", "?? What is this? Remove it?", "10",
             "valuetypeid", "Value Type Id", "INTEGER", "Value categorization such as mint or used.", "10",
             "bookvalue", "Book Value", "DOUBLE", "What is this stampe worth", "10",
             "description", "Description", "VARCHAR", "Describe the stamp", "200"};
  n=sizeof(bookValues) / sizeof(bookValues[0]);
  DescribeSqlTable bookValuesTable(bookValues, n, true, &typeMaster);
  bookValuesTable.setFieldLink("catalogid", "catalog", "id");
  bookValuesTable.setFieldLink("valuetypeid", "valuetype", "id");
  bookValuesTable.setFieldCurrencySymbol("bookvalue", "$");
  schema.addTable(bookValuesTable);

  QString inventory[] = {"inventory", "Inventory", "Physical stamps I own.",
             "id", "Id", "INTEGER", "Table Key", "10",
             "catalogid", "Catalog Id", "INTEGER", "Catalog from which the value was pulled", "10",
             "quantity", "Quantity", "INTEGER", "Number owned", "10",
             "grade","Grade", "VARCHAR", "M, U, and similar.", "10",
             "condition", "Condition", "VARCHAR", "F, VF, G, XF, etc.", "10",
             "selvage", "Selvage", "VARCHAR", "What is in the Selvage such as Plate numbers", "50",
             "centering", "Centering", "VARCHAR", "Grade the centering if desired. F, VF, XF, etc.", "10",
             "back", "Back", "VARCHAR", "Comments such as NH, OG, LH, VLH, NG.", "10",
             "comment", "Comment", "VARCHAR", "Comments about this stamp.", "200",
             "purchasedate", "Purchased", "DATE", "When the stamp was purchased MM/DD/YYYY", "10",
             "paid", "Paid", "DOUBLE", "What did I pay", "10",
             "dealerid", "Dealer", "INTEGER", "From whom did I aquire this stamp.", "10",
             "locationid", "stamplocation", "INTEGER", "Where is the stamp stored", "10",
             "replace", "Replace", "BIT", "Replace the stamp (true) or not (false)", "5",
             "updated", "Updated", "TIMESTAMP", "Date and time this record was last updated MM/DD/YYYY hh:mm:ss", "19",
             "typeid", "Type Id", "INTEGER", "Stamp type such as Air Mail", "10",
             "valuemultiplier", "Multiplier", "DOUBLE", "Use to set a premium over or under catelog value.", "10"};
  n=sizeof(inventory) / sizeof(inventory[0]);
  DescribeSqlTable inventoryTable(inventory, n, true, &typeMaster);
  inventoryTable.setFieldLink("catalogid", "catalog", "id");
  inventoryTable.setFieldLink("dealerid", "dealer", "id");
  inventoryTable.setFieldLink("locationid", "stamplocation", "id");
  // TODO: Remove type id.
  inventoryTable.setFieldLink("typeid", "catalogtype", "id");
  inventoryTable.setFieldCurrencySymbol("paid", "$");
  schema.addTable(inventoryTable);

  QString location[] = {"stamplocation", "Location", "Where is the stamp stored",
             "id", "Id", "INTEGER", "Table Key", "10",
             "name", "Name", "VARCHAR", "Viewable name", "20",
             "description", "Description", "VARCHAR", "Longer descriptive name", "100"};
  n=sizeof(location) / sizeof(location[0]);
  schema.addTable(DescribeSqlTable(location, n, true, &typeMaster));

  QString dealer[] = {"dealer", "Dealer", "From whom did I purchase this stamp.",
             "id", "Id", "INTEGER", "Table Key", "10",
             "updated", "Updated", "TIMESTAMP", "Date and time this record was last updated MM/DD/YYYY hh:mm:ss", "19",
             "name", "Name", "VARCHAR", "Viewable name", "50",
             "contact", "Contact", "VARCHAR", "Person I know or deal with", "50",
             "address1", "Address 1", "VARCHAR", "Address Line", "100",
             "address2", "Address 2", "VARCHAR", "Address Line", "100",
             "address3", "Address 3", "VARCHAR", "Address Line", "100",
             "address4", "Address 4", "VARCHAR", "Address Line", "100",
             "phone", "Phone", "VARCHAR", "Phone Number", "20",
             "fax", "Fax", "VARCHAR", "Fax Number", "20",
             "comment", "Comment", "VARCHAR", "Comments about this stamp.", "200",
             "email", "e-mail", "VARCHAR", "e-mail address", "60",
             "webiste", "Web", "VARCHAR", "Web Site", "60"};
  n=sizeof(dealer) / sizeof(dealer[0]);
  DescribeSqlTable dealerTable(dealer, n, true, &typeMaster);
  schema.addTable(dealerTable);

  return schema;
}

