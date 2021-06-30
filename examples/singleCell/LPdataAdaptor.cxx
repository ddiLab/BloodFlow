#include "LPdataAdaptor.h"
/*
#include "Error.h"
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkCellArray.h>
*/

#include <iostream>
using namespace std;

//----------------------------------------------------------------------
void LPDataAdaptor::Initialize()
{
  this->ReleaseData();
  cout << "DataAdaptor: Initialize()" << endl;
}
