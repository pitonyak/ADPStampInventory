#ifndef GENERICDATAOBJECTLESSTHAN_H
#define GENERICDATAOBJECTLESSTHAN_H

#include "genericdatacollection.h"

class GenericDataObjectLessThan
{
public:
    GenericDataObjectLessThan(const QList<TableSortField*>* sortFields);

    bool operator()(const GenericDataObject* left, GenericDataObject* right) const;
    const QList<TableSortField*>* m_sortFields;
};

class GenericDataObjectLessThanById
{
public:
    GenericDataObjectLessThanById(const QList<TableSortField*>* sortFields, const GenericDataCollection* data);

    bool operator()(const int left, const int right) const;

    const GenericDataCollection* m_data;
    const QList<TableSortField*>* m_sortFields;
};



#endif // GENERICDATAOBJECTLESSTHAN_H
