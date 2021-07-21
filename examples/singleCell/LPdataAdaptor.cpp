#include "LPdataAdaptor.h" 
#include "palabos3D.h"
#include "palabos3D.hh"

#include "Error.h"
#include <vtkObjectFactory.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkUnstructuredGrid.h>/////////
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkCellArray.h>
#include "Bridge.h"
#include <iostream>
using namespace std;
using namespace plb; 
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
// -------- PALABOS ---------------------
  //MultiTensorField3D<double, 3> pb_velocityDoubleArray; 
  vtkDoubleArray *pb_velocityDoubleArray;
  vtkDoubleArray *pb_vorticityDoubleArray; 
  vtkDoubleArray *pb_velocityNormDoubleArray;
  //vtkDoubleArray *velocityDoubleArray;
  //vtkDoubleArray *vorticityDoubleArray;
  //vtkDoubleArray *velocityNormDoubleArray;   
// --------------------------------------
  double xsublo, ysublo, zsublo, xsubhi, ysubhi, zsubhi;
  long NumBlocks;
  int nlocal, nghost;
  double **x;
  int *type;
  int *id;
  int pb_nx, pb_ny, pb_nz;
//  MultiTensorField3D<double, 3> velocityDoubleArray;
//  MultiTensorField3D<double, 3> vorticityDoubleArray;
//  MultiScalarField3D<double> velocityNormDoubleArray; 
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
//int nblocks = 1; 
//this->Internals->NumBlocks = nblocks; 
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
*/ 
  if(!internals.vertices)
  {
    internals.vertices = vtkSmartPointer<vtkCellArray>::New();
  }

 
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
    for(int i = 0; i < 20 ; i++)
    {
    values = internals.AtomPositions->GetTuple3(i);
    cout << values[0] << " " << values[1] << " " << values[2] << std::endl;
    }
*/
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
//vertices
if(internals.vertices)
  {
    vtkIdType pid[1] = {0};
    for( int i=0; i < nlocal; i++) {
	internals.vertices->InsertNextCell (1, pid);
	pid[0]++;
    }
  }

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
<<<<<<< HEAD
// timestep
  this->SetDataTimeStep(ntimestep);


  


}

void LPDataAdaptor::AddPalabosData(vtkDoubleArray *velocityDoubleArray,
		     		   vtkDoubleArray *vorticityDoubleArray,
		    		   vtkDoubleArray *velocityNormDoubleArray)  
   {

	int nx = 20, ny = 20, nz = 40; 
	DInternals& internals = (*this->Internals);

	
	internals.pb_velocityDoubleArray = velocityDoubleArray;
	internals.pb_vorticityDoubleArray = vorticityDoubleArray; 
	internals.pb_velocityNormDoubleArray = velocityNormDoubleArray;  

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
	int nx = 20, ny = 20, nz =40; // pass these from palabos!!! 
	MPI_Comm_rank(this->GetCommunicator(), &rank);
	MPI_Comm_rank(this->GetCommunicator(), &nRanks); 	
	//int nBlocks = this->Internals->BlockData.size(); 
 
	metadata->MeshName = "fluid"; 
 if(MeshName == "fluid")
  {
	metadata->MeshType = VTK_IMAGE_DATA;
	metadata->BlockType=VTK_IMAGE_DATA; 
	metadata->CoordinateType = VTK_DOUBLE;
	metadata->NumBlocks = nRanks;
	metadata->NumBlocksLocal = {1}; 
	metadata->NumArrays=1;
	metadata->ArrayName = {"velocity"};
	metadata->ArrayComponents = {3}; 
	metadata->ArrayType = {VTK_DOUBLE};
	metadata->ArrayCentering = {vtkDataObject::CELL};
	metadata->StaticMesh = 1; 

	if (metadata->Flags.BlockDecompSet())
    	{
     	  metadata->BlockOwner.push_back(rank);
     	  metadata->BlockIds.push_back(rank);
    	}
	 metadata->BlockNumCells.push_back(nx * ny * nz * 3); 
	 metadata->BlockNumPoints.push_back(nx * ny * nz * 3); 
	 metadata->BlockCellArraySize.push_back(nx * ny * nz); 
  } 

  metadata->MeshName = "cells";
if(MeshName == "cells")
{   
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
  metadata->StaticMesh = 0;  

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
   int nx = 20, ny = 20, nz = 40; 
   DInternals& internals = (*this->Internals);
   mesh = nullptr; 
//   if((meshName != "cells" || meshName != "fluid" ))
 //  {
  //   SENSEI_ERROR("No mesh \"" << meshName << "\"")
   //  return -1; 
  // }
 
  cout << "Inside get mesh " << meshName << endl;

  vtkImageData *velocity = vtkImageData::New();
  velocity->SetDimensions(nx, ny, nz); 
 
  vtkImageData *vorticity = vtkImageData::New(); 
  vorticity->SetDimensions(nx, ny, nz); 

  vtkImageData *velocityNorm = vtkImageData::New();
  velocityNorm->SetDimensions(nx, ny, nz); 
 // vtkDoubleArray *velocity = vtkImageData::New(); 
   
  velocity->GetPointData()->AddArray(internals.pb_velocityDoubleArray);
  internals.pb_velocityDoubleArray->SetName("velocity");

  vorticity->GetPointData()->AddArray(internals.pb_vorticityDoubleArray);
  internals.pb_vorticityDoubleArray->SetName("vorticity"); 

  velocityNorm->GetPointData()->AddArray(internals.pb_velocityNormDoubleArray);
  internals.pb_velocityNormDoubleArray->SetName("velocityNorm");
 // fluid->GetPointData()->AddArray(internals.pb_velocityDoubleArray);   
 // pb_velocity->GetPointCells(internals.pb_velocityDoubleArray); 

   mesh = velocity;
   mesh = vorticity; 
   mesh = velocityNorm; 

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
   DInternals& internals = (*this->Internals); 
   //vtkImageData *velocity = vtkImageData::New();
   vtkImageData *velocity = dynamic_cast<vtkImageData*>(mesh);
   vtkImageData *vorticity = dynamic_cast<vtkImageData*>(mesh); 
   vtkImageData *velocityNorm = dynamic_cast<vtkImageData*>(mesh);
  // vtkImageData *velocity = vtkImageData::New(); 
  // velocity = vtkImageData::New(); 
   
   velocity->GetPointData()->AddArray(internals.pb_velocityDoubleArray);
   vorticity->GetPointData()->AddArray(internals.pb_vorticityDoubleArray); 
   velocityNorm->GetPointData()->AddArray(internals.pb_velocityNormDoubleArray);
   return 0;
}
//----------------------------------------------------------------------
int LPDataAdaptor::AddArrays(vtkDataObject* mesh, const std::string &meshName, int association, const std::vector<std::string> &arrayName)
{
  // DInternals& internals = (*this->Internals);

//   vtkImageData *imageData = vtkImageData::New();

 //  imageData->GetPointData()->AddArray(internals.pb_velocityDoubleArray);
   //            internals.pb_velocityDoubleArray->SetName("velocity");


   
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

