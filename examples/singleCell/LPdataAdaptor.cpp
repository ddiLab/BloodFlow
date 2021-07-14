#include "LPdataAdaptor.h"


#include "Error.h"
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>/////////
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
struct LPDataAdaptor::DInternals
{
  vtkSmartPointer<vtkMultiBlockDataSet> mesh;
  vtkSmartPointer<vtkDoubleArray> AtomPositions;
  vtkSmartPointer<vtkIntArray> AtomTypes;
  vtkSmartPointer<vtkIntArray> AtomIDs;
  vtkSmartPointer<vtkCellArray> vertices;
  double xsublo, ysublo, zsublo, xsubhi, ysubhi, zsubhi;
  int nlocal, nghost;
  double **x;
  int *type;
  int *id;
};
//----------------------------------------------------------------------
senseiNewMacro(LPDataAdaptor);
//----------------------------------------------------------------------
LPDataAdaptor::LPDataAdaptor() :
  Internals(new LPDataAdaptor::DInternals())
{
}
//----------------------------------------------------------------------
LPDataAdaptor::~LPDataAdaptor()
{
  delete this->Internals;
}
//----------------------------------------------------------------------
void LPDataAdaptor::Initialize()
{
  this->ReleaseData();//ReleaseData must be correctly defined!!
  //cout << "DataAdaptor: Initialize()" << endl;
}
//----------------------------------------------------------------------
void LPDataAdaptor::AddLAMMPSData(double **x, long ntimestep, int nghost, 
                                  int nlocal, double xsublo, double xsubhi,
                                  double ysublo, double ysubhi, double zsublo,
                                  double zsubhi)

{

  DInternals& internals = (*this->Internals);
  
  if(!internals.AtomPositions)
  {
    internals.AtomPositions = vtkSmartPointer<vtkDoubleArray>::New();
  }
/* 
  if(!internals.AtomTypes)
  { 
    internals.AtomTypes = vtkSmartPointer<vtkIntArray>::New();
  }
  if(!internals.AtomIDs)
  {
    internals.AtomIDs = vtkSmartPointer<vtkIntArray>::New();
  } 
  if(!internals.vertices)
  {
    internals.vertices = vtkSmartPointer<vtkCellArray>::New();
  }

*/  
// atom coordinates
  if (internals.AtomPositions)
  {
    long nvals = nlocal;
    
    internals.AtomPositions->SetNumberOfComponents(3);
    internals.AtomPositions->SetArray(*x, nvals*3, 1);
    internals.AtomPositions->SetName("positions");
    
    internals.x = x;
  }  
  else
  {
    SENSEI_ERROR("Error. Internal AtomPositions structure not initialized")
  }
/*  
// atom types

  if (internals.AtomTypes)
  {
    long nvals = nlocal;
    
    internals.AtomTypes->SetNumberOfComponents(1);
    internals.AtomTypes->SetArray(type, nvals, 1);
    internals.AtomTypes->SetName("type");
    
    internals.type = type;  
  }
  else 
  {
    SENSEI_ERROR("Error. Internal AtomTypes structure not initialized")
  }
*/

// number of atoms
  internals.nlocal = nlocal;
  internals.nghost = nghost;

// bounding box 
  internals.xsublo = xsublo;
  internals.ysublo = ysublo;
  internals.zsublo = zsublo;
  internals.xsubhi = xsubhi;
  internals.ysubhi = ysubhi;
  internals.zsubhi = zsubhi;

// timestep
  this->SetDataTimeStep(ntimestep);

  

 /* 
>>>>>>> 34ba72c204ac8bb5dea0c2f62e793c58ac76fe25
  for(int i = 0; i < nlocal; i++)
  {
   x[i][2] += 0.05;
  }
*/ 

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
   if((meshName != "cells" ))
   {
     SENSEI_ERROR("No mesh \"" << meshName << "\"")
     return -1; 
   }
   
   DInternals& internals = (*this->Internals);
   vtkSmartPointer<vtkUnstructuredGrid> pts = vtkSmartPointer<vtkUnstructuredGrid>::New();
   pts->GetPointData()->AddArray(internals.AtomPositions);

   mesh = pts;
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
  DInternals& internals = (*this->Internals);
  internals.AtomPositions = NULL;
  internals.nlocal = 0;
  internals.nghost = 0;
  internals.xsublo = 0;
  internals.ysublo = 0;
  internals.zsublo = 0;
  internals.xsubhi = 0;
  internals.ysubhi = 0;
  internals.zsubhi = 0;
  
  return 0;
}

}

