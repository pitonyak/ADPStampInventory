#ifndef GENERICDATAOBJECTLESSTHAN_H
#define GENERICDATAOBJECTLESSTHAN_H

#include "genericdatacollection.h"

class GenericDataObjectLessThan
{
public:
    GenericDataObjectLessThan(const GenericDataCollection* data);

    const GenericDataCollection* m_data;

};

#endif // GENERICDATAOBJECTLESSTHAN_H
