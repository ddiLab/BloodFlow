#include "LPdataAdaptor.h" 
#include "palabos3D.h"
#include "palabos3D.hh"

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
    vtkDoubleArray *pb_velocityDoubleArray;
    vtkDoubleArray *pb_vorticityDoubleArray; 
    vtkDoubleArray *pb_velocityNormDoubleArray;
  // --------------------------------------
    double xsublo, ysublo, zsublo, xsubhi, ysubhi, zsubhi;
    long NumBlocks;
    int nlocal, nghost, nanglelist;
    double **x;
    int *type;
    int **anglelist;
    int *id;
    int pb_nx, pb_ny, pb_nz;
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

    

  // anglelists
    internals.anglelist = anglelist;
    internals.nanglelist = nanglelist;

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

  }
  //---------------------------------------------------------------------------
  void LPDataAdaptor::AddPalabosData(vtkDoubleArray *velocityDoubleArray,
                vtkDoubleArray *vorticityDoubleArray,
                vtkDoubleArray *velocityNormDoubleArray,
                        int nx, int ny, int nz)  
  {
    DInternals& internals = (*this->Internals);

    internals.pb_velocityDoubleArray = velocityDoubleArray;
    internals.pb_vorticityDoubleArray = vorticityDoubleArray; 
    internals.pb_velocityNormDoubleArray = velocityNormDoubleArray;
    internals.pb_nx = nx;
    internals.pb_ny = ny;
    internals.pb_nz = nz;
  }   
  //----------------------------------------------------------------------
  int LPDataAdaptor::GetNumberOfMeshes(unsigned int &numMeshes)
  {
    numMeshes = 2;
    return 0;
  }
  //----------------------------------------------------------------------
  int LPDataAdaptor::GetMeshMetadata(unsigned int id, sensei::MeshMetadataPtr &metadata) 
  {
    int rank, nRanks;
    
    int nx = this->Internals->pb_nx;
    int ny = this->Internals->pb_ny;
    int nz = this->Internals->pb_nz;	
    MPI_Comm_rank(this->GetCommunicator(), &rank);
    MPI_Comm_size(this->GetCommunicator(), &nRanks); 	

    if (id == 0)
    {
      metadata->MeshName = "cells";
      metadata->MeshType = VTK_MULTIBLOCK_DATA_SET; //VTK_POLY_DATA;
      metadata->BlockType = VTK_POLY_DATA;
      metadata->CoordinateType = VTK_DOUBLE;
      metadata->NumBlocks = nRanks;
      metadata->NumBlocksLocal = {1};
      metadata->NumGhostCells = this->Internals->nghost;
      
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
    //We use nanglelist for BlockNumCells because it give the number of triangles on a given processor
    
      metadata->BlockNumCells.push_back(this->Internals->nanglelist);
      metadata->BlockNumPoints.push_back(this->Internals->nlocal + this->Internals->nghost);
      metadata->BlockCellArraySize.push_back(0);
    }
    else
    {
      metadata->MeshName = "fluid"; 
      metadata->MeshType = VTK_IMAGE_DATA;
      metadata->BlockType=VTK_IMAGE_DATA; 
      metadata->CoordinateType = VTK_DOUBLE;
      metadata->NumBlocks = nRanks;
      metadata->NumBlocksLocal = {1}; 
      metadata->NumArrays=3;
      metadata->ArrayName = {"velocity","vorticity","velocityNorm"};
      metadata->ArrayComponents = {3, 3, 3}; 
      metadata->ArrayType = {VTK_DOUBLE, VTK_DOUBLE, VTK_DOUBLE};
      metadata->ArrayCentering = {vtkDataObject::CELL, vtkDataObject::CELL, vtkDataObject::CELL};
      metadata->StaticMesh = 1; 

      if (metadata->Flags.BlockDecompSet())
      {
        metadata->BlockOwner.push_back(rank);
        metadata->BlockIds.push_back(rank);
      }

      metadata->BlockNumCells.push_back(nx * ny * nz * 3); //THESE NEED TO BE CHANGED MOST LIKELY
      metadata->BlockNumPoints.push_back(nx * ny * nz * 3); 
      metadata->BlockCellArraySize.push_back(0); 
    }
    
    return 0;
  }
  //----------------------------------------------------------------------
  int LPDataAdaptor::GetMesh(const std::string &meshName, bool structureOnly, vtkDataObject *&mesh)
  {
  

    if(meshName == "cells")
    {
      //mesh = nullptr; //QUESTION: DO WE NEED THIS????  
      DInternals& internals = (*this->Internals);

      vtkPolyData *pd = vtkPolyData::New();
      vtkMultiBlockDataSet *mb = vtkMultiBlockDataSet::New();

      if(!structureOnly)
      {
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
      
      pd->SetVerts( internals.vertices );
      int rank, size; 
      MPI_Comm_rank(this->GetCommunicator(), &rank);
      MPI_Comm_size(this->GetCommunicator(), &size);

      mb->SetNumberOfBlocks(size);
      mb->SetBlock(rank,pd);
 
      mesh = mb;
    }

    else if(meshName == "fluid")
    {
      DInternals& internals = (*this->Internals);
      mesh = nullptr; 
      vtkMultiBlockDataSet *mbfluid = vtkMultiBlockDataSet::New();
    
      cout << "Inside get mesh " << meshName << endl;

      vtkImageData *FluidImageData = vtkImageData::New();
      FluidImageData->SetDimensions(internals.pb_nx, internals.pb_ny, internals.pb_nz); //HERE IS WHERE WE NEED TO CHANGE DIMENSIONS
    /*
      vtkImageData *vorticity = vtkImageData::New(); 
      vorticity->SetDimensions(internals.pb_nx, internals.pb_ny, internals.pb_nz); 

      vtkImageData *velocityNorm = vtkImageData::New();
      velocityNorm->SetDimensions(internals.pb_nx, internals.pb_ny, internals.pb_nz); 
      */
      
      //cout << internals.pb_nx << "," << internals.pb_ny << "," << internals.pb_nz << endl;
      
      FluidImageData->GetPointData()->AddArray(internals.pb_velocityDoubleArray);
      internals.pb_velocityDoubleArray->SetName("velocity");

      FluidImageData->GetPointData()->AddArray(internals.pb_vorticityDoubleArray);
      internals.pb_vorticityDoubleArray->SetName("vorticity"); 

      FluidImageData->GetPointData()->AddArray(internals.pb_velocityNormDoubleArray);
      internals.pb_velocityNormDoubleArray->SetName("velocityNorm");
    
      int rank, size;
      MPI_Comm_rank(this->GetCommunicator(), &rank);
      MPI_Comm_size(this->GetCommunicator(), &size);

      mbfluid->SetNumberOfBlocks(size);
      mbfluid->SetBlock(rank,FluidImageData);
      
      mesh = mbfluid;
    }
    else
    {
      SENSEI_ERROR("No mesh \"" << meshName << "\"")
      return -1;
    }

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
