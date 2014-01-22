#include "genericdataobjectlessthan.h"

GenericDataObjectLessThan::GenericDataObjectLessThan(const QList<TableSortField*>* sortFields) :
  m_sortFields(sortFields)
{
}

bool GenericDataObjectLessThan::operator()(const GenericDataObject* left, GenericDataObject* right) const
{
  return left->compare(*right, *m_sortFields) < 0;
}

GenericDataObjectLessThanById::GenericDataObjectLessThanById(const QList<TableSortField*>* sortFields, const GenericDataCollection *data) :
  m_data(data), m_sortFields(sortFields)
{
}

bool GenericDataObjectLessThanById::operator()(const int left, const int right) const
{
  return m_data->getObjectById(left)->compare(*m_data->getObjectById(right), *m_sortFields) < 0;
}
