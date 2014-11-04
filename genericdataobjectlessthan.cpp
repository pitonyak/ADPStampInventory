#include "genericdataobjectlessthan.h"

GenericDataObjectLessThan::GenericDataObjectLessThan(const QList<TableSortField*>* sortFields) :
  m_sortFields(sortFields)
{
}

bool GenericDataObjectLessThan::operator()(const GenericDataObject* left, GenericDataObject* right) const
{
  Q_ASSERT_X(left != nullptr, "GenericDataObjectLessThan::operator()", "left object is NULL");
  Q_ASSERT_X(right != nullptr, "GenericDataObjectLessThan::operator()", "right object is NULL");
  Q_ASSERT_X(m_sortFields != nullptr, "GenericDataObjectLessThan::operator()", "Sort Fields object is NULL");
  return left->compare(*right, *m_sortFields) < 0;
}

GenericDataObjectLessThanById::GenericDataObjectLessThanById(const QList<TableSortField*>* sortFields, const GenericDataCollection *data) :
  m_data(data), m_sortFields(sortFields)
{
}

bool GenericDataObjectLessThanById::operator()(const int left, const int right) const
{
  Q_ASSERT_X(m_sortFields != nullptr, "GenericDataObjectLessThanById::operator()", "Sort Fields object is NULL");
  Q_ASSERT_X(m_data != nullptr, "GenericDataObjectLessThanById::operator()", "Data Container object is NULL");
  Q_ASSERT_X(m_data->hasObject(left), "GenericDataObjectLessThanById::operator()", qPrintable(QString("data container does not have left object with ID %1").arg(left)));
  Q_ASSERT_X(m_data->hasObject(right), "GenericDataObjectLessThanById::operator()", qPrintable(QString("data container does not have right object with ID %1").arg(right)));
  return m_data->getObjectById(left)->compare(*m_data->getObjectById(right), *m_sortFields) < 0;
}
