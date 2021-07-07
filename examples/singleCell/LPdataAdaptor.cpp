#include "LPdataAdaptor.h"


#include "Error.h"
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkCellArray.h>


#include <iostream>
using namespace std;
namespace senseiLP
{
//----------------------------------------------------------------------
senseiNewMacro(LPDataAdaptor);
//----------------------------------------------------------------------
void LPDataAdaptor::Initialize()
{
  this->ReleaseData();//ReleaseData must be correctly defined!!
  cout << "DataAdaptor: Initialize()" << endl;
}
//----------------------------------------------------------------------
int LPDataAdaptor::GetNumberOfMeshes(unsigned int &numMeshes)
{
   numMeshes = 1;
   return 0;
}
//----------------------------------------------------------------------
int LPDataAdaptor::GetMeshMetadata(unsigned int id, sensei::MeshMetadataPtr &metadata) 
{
   return 0;
}
//----------------------------------------------------------------------
int LPDataAdaptor::GetMesh(const std::string &meshName, bool structureOnly, vtkDataObject *&mesh)
{
   return 0;
}
//----------------------------------------------------------------------
int LPDataAdaptor::GetMesh(const std::string &meshName, bool structureOnly, vtkCompositeDataSet *&mesh)
{
   return 0;
}
//----------------------------------------------------------------------
int LPDataAdaptor::AddGhostNodesArray(vtkDataObject* mesh, const std::string &meshName)
{
   return 0;
}
//----------------------------------------------------------------------
int LPDataAdaptor::AddGhostCellsArray(vtkDataObject* mesh, const std::string &meshName)
{
   return 0; 
}
//----------------------------------------------------------------------
int LPDataAdaptor::AddArray(vtkDataObject* mesh, const std::string &meshName,
    int association, const std::string &arrayName)
{
   return 0;
}
//----------------------------------------------------------------------
int LPDataAdaptor::AddArrays(vtkDataObject* mesh, const std::string &meshName, int association, const std::vector<std::string> &arrayName)
{
   return 0;
}
//----------------------------------------------------------------------
int LPDataAdaptor::ReleaseData() 
{
   return 0;
}

}
