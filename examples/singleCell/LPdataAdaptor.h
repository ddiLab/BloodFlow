#pragma once

#include <DataAdaptor.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>

namespace senseiLP
{
class LPDataAdaptor : public sensei::DataAdaptor
{
public:

//LPDataAdaptor(){};
static LPDataAdaptor* New();
senseiTypeMacro(LPDataAdaptor, sensei::DataAdaptor);

void Initialize();

};

}
