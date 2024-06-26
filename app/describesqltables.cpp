#include "describesqltables.h"
#include "sqlfieldtypemaster.h"

#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QDebug>

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

QSet<QString> DescribeSqlTables::getLinkedTableNames(const QString& tableName) const
{
    Q_ASSERT_X(containsTable(tableName), "DescribeSqlTables::getLinkedTableNames", qPrintable(QString("Unknown table %1").arg(tableName)));
    const DescribeSqlTable* table = getTableByName(tableName);
    return (table != nullptr) ? table->getLinkedTableNames() : QSet<QString>();
}

QString DescribeSqlTables::getNameByIndex(const int index) const
{
  if (0 <= index && index < getTableCount()) {
    return m_names.at(index);
  } else {
    qDebug() << qPrintable(QString("Field index = %1 is out of range for table %2").arg(index).arg(getName()));
  }
  return "";
}

QMetaType::Type DescribeSqlTables::getFieldMetaType(const QString& tableName, const QString& fieldName) const
{
  const DescribeSqlTable* table = getTableByName(tableName);
  return (table != nullptr) ? table->getFieldMetaType(fieldName) : QMetaType::UnknownType;
}


const DescribeSqlTable *DescribeSqlTables::getTableByName(const QString& tableName) const
{
  QString simpleName = tableName.toLower();
  if (m_tables.contains(simpleName)) {
    return &const_cast<QHash<QString, DescribeSqlTable>&>(m_tables)[simpleName];
  } else {
    qDebug() << qPrintable(QString("Table name = '%1' is contained in table set %2").arg(tableName).arg(getName()));
  }
  return nullptr;
}

const DescribeSqlTable *DescribeSqlTables::getTableByIndex(const int index) const
{
  return getTableByName(getNameByIndex(index));
}

bool DescribeSqlTables::addTable(const DescribeSqlTable& table)
{
  QString simpleName = table.getName().toLower();
  if (m_tables.contains(simpleName)) {
    qDebug() << "Table name = '" << simpleName << "' is already contained in table set " << getName();
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
            if (reader.name().compare(QLatin1String("Tables"), Qt::CaseInsensitive) == 0) {
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
            } else if (reader.name().compare(QLatin1String("Table"), Qt::CaseInsensitive) == 0) {
                DescribeSqlTable table = DescribeSqlTable::readXml(reader);
                if (table.getName().isEmpty() || !tables.addTable(table)) {
                    qDebug() << qPrintable(QString("Failed to add Table name = '%1'").arg(table.getName()));
                    break;
                }
            } else {
                // Unexpected element, what to do!
                qDebug() << qPrintable(QString("Found unexpected XML element %1").arg(reader.name().toString()));
                break;
            }
        } else if (reader.isStartDocument()) {
            reader.readNext();
        } else if (reader.isEndElement()) {
            if (foundTablesTag && reader.name().compare(QLatin1String("Tables"), Qt::CaseInsensitive) == 0) {
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

QHash<QString, TableEditFieldDescriptors> DescribeSqlTables::getPrimaryTableEditDescriptors()
{
  QHash<QString, TableEditFieldDescriptors> tableDescriptors;

  TableEditFieldDescriptors catalogDescriptors;
  TableEditFieldDescriptor linkField("country", "a3", "");
  catalogDescriptors.append(TableEditFieldDescriptor("catalog", "id", "ID", "/", true));
  catalogDescriptors.append(TableEditFieldDescriptor(linkField, "catalog", "countryid", "Country"));
  linkField.setTableName("catalogtype");
  linkField.setFieldName("name");
  catalogDescriptors.append(TableEditFieldDescriptor(linkField, "catalog", "typeid", "Type"));
  catalogDescriptors.append(TableEditFieldDescriptor("catalog", "scott", "Scott"));
  catalogDescriptors.append(TableEditFieldDescriptor("catalog", "description", "Description"));
  catalogDescriptors.append(TableEditFieldDescriptor("catalog", "releasedate", "Release"));
  catalogDescriptors.append(TableEditFieldDescriptor("catalog", "facevalue", "Face"));
  tableDescriptors.insert("catalog", catalogDescriptors);

  return tableDescriptors;
}

DescribeSqlTables DescribeSqlTables::getStampSchema()
{
  DescribeSqlTables schema;

  SqlFieldTypeMaster typeMaster;

  qDebug() << "Ready for catalog type";
  // Name, View Name, Type, Description, length

  QString catalogType[] = {"catalogtype", "Catalog Type", "Stamp category such as Air Mail, Postage, or Envelope",
             "id", "Id", "INTEGER", "Table Key", "10",
             "prefix", "Prefix", "VARCHAR", "Prefix for a stamp value that determines the type", "20",
             "name", "Name", "VARCHAR", "Viewable name such as Air Mail or Postage", "20",
             "description", "Description", "VARCHAR", "Longer descriptive name", "100"};

  int n=sizeof(catalogType) / sizeof(catalogType[0]);
  DescribeSqlTable catalogTypeTable(catalogType, n, true, &typeMaster);
  catalogTypeTable.setFieldKey("id", true);
  schema.addTable(catalogTypeTable);

  QString country[] = {"country", "Country", "Country",
             "id", "Id", "INTEGER", "Table Key", "10",
             "name", "Name", "VARCHAR", "Viewable name such as Air Mail or Postage", "50",
             "a3", "A3", "VARCHAR", "Three letter country designation", "3"};
  n=sizeof(country) / sizeof(country[0]);
  DescribeSqlTable countryType(country, n, true, &typeMaster);
  countryType.setFieldKey("id", true);
  schema.addTable(countryType);

  QString  valueType[] = {"valuetype", "Value Type", "Categorizes the stamp for value such as mint or used.",
             "id", "Id", "INTEGER", "Table Key", "10",
             "description", "Description", "VARCHAR", "Describes the type", "100"};
  n=sizeof(valueType) / sizeof(valueType[0]);
  DescribeSqlTable valueTable(valueType, n, true, &typeMaster);
  valueTable.setFieldKey("id", true);
  schema.addTable(valueTable);

  QString valueSource[] = {"valuesource", "Value Source", "Categorizes the stamp for value such as mint or used.",
             "id", "Id", "INTEGER", "Table Key", "10",
             "year", "Date", "DATE", "Date for the book from which the values were taken in the format MM/DD/YYYY", "10",
             "description", "Description", "VARCHAR", "Longer descriptive name such as Scotts Catelog", "100"};
  n=sizeof(valueSource) / sizeof(valueSource[0]);
  DescribeSqlTable valueSourceTable(valueSource, n, true, &typeMaster);
  valueSourceTable.setFieldKey("id", true);
  schema.addTable(valueSourceTable);

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
   catalogTable.setFieldLink("id", "catalog", "id", "countryid,scott,typeid");

  catalogTable.setFieldLink("countryid", "country", "id", "a3");
  catalogTable.setFieldLink("typeid", "catalogtype", "id", "name");
  catalogTable.setFieldCurrencySymbol("facevalue", "$");
  catalogTable.setFieldKey("id", true);
  schema.addTable(catalogTable);

  QString bookValues[] = {"bookvalues", "Book Values", "What is each stamp worth.",
             "id", "Id", "INTEGER", "Table Key", "10",
             "catalogid", "Catalog Id", "INTEGER", "Catalog from which the value was pulled", "10",
             "sourceid", "Source Id", "INTEGER", "?? What is this? Remove it?", "10",
             "valuetypeid", "Value Type Id", "INTEGER", "Value categorization such as mint or used.", "10",
             "bookvalue", "Book Value", "DOUBLE", "What is this stampe worth", "10" };
  n=sizeof(bookValues) / sizeof(bookValues[0]);
  DescribeSqlTable bookValuesTable(bookValues, n, true, &typeMaster);
  bookValuesTable.setFieldLink("catalogid", "catalog", "id", "countryid,scott,typeid");
  bookValuesTable.setFieldLink("valuetypeid", "valuetype", "id", "description");
  bookValuesTable.setFieldCurrencySymbol("bookvalue", "$");
  bookValuesTable.setFieldKey("id", true);
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
             "certificate", "Certificate", "VARCHAR", "Comments about this stamp.", "200",
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
  inventoryTable.setFieldLink("catalogid", "catalog", "id", "countryid,scott,typeid");
  inventoryTable.setFieldLink("dealerid", "dealer", "id", "name");
  inventoryTable.setFieldLink("locationid", "stamplocation", "id", "name");
  inventoryTable.setFieldKey("id", true);
  // TODO: Remove type id.
  inventoryTable.setFieldLink("typeid", "catalogtype", "id", "name");
  inventoryTable.setFieldCurrencySymbol("paid", "$");
  schema.addTable(inventoryTable);

  QString location[] = {"stamplocation", "Location", "Where is the stamp stored",
             "id", "Id", "INTEGER", "Table Key", "10",
             "name", "Name", "VARCHAR", "Viewable name", "20",
             "description", "Description", "VARCHAR", "Longer descriptive name", "100"};
  n=sizeof(location) / sizeof(location[0]);
  DescribeSqlTable locationTable(location, n, true, &typeMaster);
  locationTable.setFieldKey("id", true);
  schema.addTable(locationTable);

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
             "comment", "Comment", "VARCHAR", "Comments about this dealer.", "200",
             "email", "e-mail", "VARCHAR", "e-mail address", "60",
             "website", "Web", "VARCHAR", "Web Site", "60"};
  n=sizeof(dealer) / sizeof(dealer[0]);
  DescribeSqlTable dealerTable(dealer, n, true, &typeMaster);
  dealerTable.setFieldKey("id", true);
  schema.addTable(dealerTable);

  return schema;
}

QStringList DescribeSqlTables::getDDL(const bool prettyPrint) const
{
  QStringList list;
  for (int i=0; i<m_names.size(); ++i) {
    DescribeSqlTable table = m_tables.value(m_names.at(i));
    list << table.getDDL(prettyPrint);
  }
  return list;
}
