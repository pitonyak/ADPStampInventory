#ifndef TABLEEDITFIELDDESCRIPTORS_H
#define TABLEEDITFIELDDESCRIPTORS_H

#include "tableeditfielddescriptor.h"

#include <QObject>

class TableEditFieldDescriptors : public QObject
{
  Q_OBJECT
public:
  explicit TableEditFieldDescriptors(QObject *parent = nullptr);
  TableEditFieldDescriptors(const TableEditFieldDescriptors& fieldDescriptors);
  explicit TableEditFieldDescriptors(const TableEditFieldDescriptors& fieldDescriptors, QObject *parent);

  int size() const { return m_fields.size(); }
  const TableEditFieldDescriptor& at(const int i) const { return m_fields.at(i); }
  void clear() { m_fields.clear(); }

  void append(const TableEditFieldDescriptor& fieldDescriptor) { m_fields.append(fieldDescriptor); }
  void append(const QList<TableEditFieldDescriptor>& fieldDescriptors) { m_fields.append(fieldDescriptors); }

  const TableEditFieldDescriptors& operator=(const TableEditFieldDescriptors& o) { return copy(o); }

  QString getName() const { return m_name; }
  void setName(const QString& name) { m_name = name; }

  // Read / write XMl is not tested.

  /*! \brief Write this object to an XML stream.
   *  \param [in, out] writer Write to this XML stream.
   *  \return reference to the writer stream.
   */
  QXmlStreamWriter& writeXml(QXmlStreamWriter& writer) const;

  /*! \brief Read and return an object of this type. Fields not included in the XML use the default values.
   *  \param [in, out] reader Read from this XML stream.
   *  \return A new field that was just read.
   */
  static TableEditFieldDescriptors readXml(QXmlStreamReader& reader);

signals:

public slots:

private:
  const TableEditFieldDescriptors& copy(const TableEditFieldDescriptors& o);

  QString m_name;
  QList<TableEditFieldDescriptor> m_fields;

};

#endif // TABLEEDITFIELDDESCRIPTORS_H
