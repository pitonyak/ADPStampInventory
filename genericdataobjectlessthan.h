#ifndef GENERICDATAOBJECTLESSTHAN_H
#define GENERICDATAOBJECTLESSTHAN_H

// TODO: Delete this class unless it is used somewhere.

#include "genericdatacollection.h"

//**************************************************************************
/*! \class GenericDataObjectLessThan
 * \brief Unused class that encapsulates determining if one object is less than another based on the pointers to those objects.
 *
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2013-2014
 ***************************************************************************/
class GenericDataObjectLessThan
{
public:
    GenericDataObjectLessThan(const QList<TableSortField*>* sortFields);

    //**************************************************************************
    /*! \brief Comare left object to the right object.
     *
     * Compares the left and right objects by calling the compare method on the left object.
     * In the process, both objects are derefenced, so, a nullptr will cause a crash.
     *
     *  \param [in] left
     *  \param [in] right
     * \return true if left < right. More specifically, if left->compare(*right) is < 0.
     ***************************************************************************/
    bool operator()(const GenericDataObject* left, GenericDataObject* right) const;
    const QList<TableSortField*>* m_sortFields;
};

//**************************************************************************
/*! \class GenericDataObjectLessThanById
 * \brief Unused class that encapsulates determining if one object is less than another based on the object ID.
 *
 *
 * \author Andrew Pitonyak
 * \copyright Andrew Pitonyak, but you may use without restriction.
 * \date 2013-2014
 ***************************************************************************/
class GenericDataObjectLessThanById
{
public:
    GenericDataObjectLessThanById(const QList<TableSortField*>* sortFields, const GenericDataCollection* data);

    bool operator()(const int left, const int right) const;

    const GenericDataCollection* m_data;
    const QList<TableSortField*>* m_sortFields;
};



#endif // GENERICDATAOBJECTLESSTHAN_H
