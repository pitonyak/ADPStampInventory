#include "describesqlfield.h"

DescribeSqlField::DescribeSqlField()
{
  m_fieldLength = 0;
  m_isAutoIncrement = false;
  m_isKey = false;
  m_isRequired = false;
}

DescribeSqlField::DescribeSqlField(const DescribeSqlField& field)
{
  copy(field);
}

const DescribeSqlField& DescribeSqlField::copy(const DescribeSqlField& field)
{
  if (this != &field) {
    setName(field.getName());
    setViewName(field.getViewName());
    setDescription(field.getDescription());
    setFieldType(field.getFieldType());
    setPreferredTypeName(field.getPreferredTypeName());
    setLinkTableName(field.getLinkTableName());
    setLinkFieldName(field.getLinkeFieldName());
    setIsAutoIncrement(field.isAutoIncrement());
    setIsRequired(field.isRequired());
    setIsKey(field.isKey());
    setFieldLength(field.getFieldLength());
  }
  return *this;
}

bool DescribeSqlField::isLinkField() const
{
  return !getLinkTableName().isEmpty() && !getLinkeFieldName().isEmpty() ;
}
