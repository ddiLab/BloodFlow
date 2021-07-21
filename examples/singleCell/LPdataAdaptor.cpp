#include "LPdataAdaptor.h"
#include "Error.h"
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkCellArray.h>
#include <vtkTriangle.h>

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
  long NumBlocks;
  int nlocal, nghost, nanglelist;
  double **x;
  int *type;
  int **anglelist;
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
}
//----------------------------------------------------------------------
void LPDataAdaptor::AddLAMMPSData(double **x, long ntimestep, int nghost, 
                                  int nlocal, double xsublo, double xsubhi,
                                  double ysublo, double ysubhi, double zsublo,
                                  double zsubhi, int **anglelist, int nanglelist)

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
/*
    double * values;
    for(int i = 0; i < nlocal ; i++)
    {
    values = internals.atompositions->gettuple3(i);
    cout << values[0] << " " << values[1] << " " << values[2] << std::endl;
    }
*/
  }  
  else
  {
    SENSEI_ERROR("Error. Internal AtomPositions structure not initialized")
  }

// anglelists
    internals.anglelist = anglelist;
    internals.nanglelist = nanglelist;
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

//vertices
if(internals.vertices)
  {
    vtkIdType pid[1] = {0};
    for( int i=0; i < nlocal; i++) {
	internals.vertices->InsertNextCell (1, pid);
	pid[0]++;
    }
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


  

 /* 
  for(int i = 0; i < nlocal; i++)
  {
   
   cout << x[i][0] << "," << x[i][1] << "," << x[i][2] << "\"" << endl;
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
  int rank = 0;
  int nRanks = 1;

  MPI_Comm_rank(this->GetCommunicator(), &rank);
  MPI_Comm_size(this->GetCommunicator(), &nRanks);

  metadata->MeshName = "cells";
   
  metadata->MeshType = VTK_POLY_DATA;
  metadata->BlockType = VTK_POLY_DATA;
  metadata->CoordinateType = VTK_DOUBLE;
  metadata->NumBlocks = nRanks;
  metadata->NumBlocksLocal = {1};
  //metadata->NumGhostCells = this->Internals->nghost;
  metadata->NumArrays = 1;
  metadata->ArrayName = {"data"};
  metadata->ArrayCentering = {vtkDataObject::CELL};
  metadata->ArrayComponents = {1};
  metadata->ArrayType = {VTK_FLOAT};
  metadata->StaticMesh = 1;  

  if (metadata->Flags.BlockDecompSet())
  {
    metadata->BlockOwner.push_back(rank);
    metadata->BlockIds.push_back(rank);
  }
  
  metadata->BlockNumCells.push_back(this->Internals->nlocal/3);
  metadata->BlockNumPoints.push_back(this->Internals->nlocal*3);
  metadata->BlockCellArraySize.push_back(this->Internals->nlocal);
 
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
   //cout << "INSIDE GetMesh" << endl; 
  DInternals& internals = (*this->Internals);
/* values;
      for(int i = 0; i < 20 ; i++)
      {
      values = internals.AtomPositions->GetTuple3(i);
      cout << values[0] << " " << values[1] << " " << values[2] << std::endl;
      }
*/

 //if (!internals.mesh){
   vtkPolyData *pd = vtkPolyData::New();

   if(!structureOnly){
     vtkPoints *pts = vtkPoints::New();
     pts->SetNumberOfPoints(internals.nlocal*3);
     pts->SetData(internals.AtomPositions);

     vtkCellArray *Triangles = vtkCellArray::New();
     for (int i = 0 ; i < internals.nanglelist ; i++)
     {
       vtkTriangle *Triangle = vtkTriangle::New();
       Triangle->GetPointIds()->SetId(0, internals.anglelist[i][0]);
       Triangle->GetPointIds()->SetId(1, internals.anglelist[i][1]);
       Triangle->GetPointIds()->SetId(2, internals.anglelist[i][2]);
       Triangles->InsertNextCell(Triangle);
     }

     pd->SetPoints(pts);
     pd->SetPolys(Triangles);

   }
   mesh = pd;
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

