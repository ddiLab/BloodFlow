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
    Box3D domainBox; //XXX added for domainBox 2/23/22
    
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
      long nvals = nlocal+nghost;

      
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
                        int nx, int ny, int nz, Box3D domainBox) 
  {
    DInternals& internals = (*this->Internals);

    internals.pb_velocityDoubleArray = velocityDoubleArray;
    internals.pb_vorticityDoubleArray = vorticityDoubleArray; 
    internals.pb_velocityNormDoubleArray = velocityNormDoubleArray;
    internals.pb_velocityDoubleArray->SetName("velocity");
    internals.pb_vorticityDoubleArray->SetName("vorticity"); 
    internals.pb_velocityNormDoubleArray->SetName("velocityNorm");
      
    internals.pb_nx = nx;
    internals.pb_ny = ny;
    internals.pb_nz = nz;
    internals.domainBox = domainBox;//XXX domainBox 2/23/22
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
    //cout << "Calling GetMeshMetaData" << endl;
    int rank, nRanks;
    
    int nx = this->Internals->pb_nx;
    int ny = this->Internals->pb_ny;
    int nz = this->Internals->pb_nz; 

    //XXX Added for domainBox 2/23/22********	
    Box3D domainBox = this->Internals->domainBox;
    int nlx = domainBox.getNx(); 
    int nly = domainBox.getNy();
    int nlz = domainBox.getNz();
    plb::Array<plint, 6> localExtents = domainBox.to_plbArray();//XXX look at palabos/src/core/geometry3D.h for documentation
    //***************************************

    MPI_Comm_rank(this->GetCommunicator(), &rank);
    MPI_Comm_size(this->GetCommunicator(), &nRanks); 	

    if (id == 0) // id == 0 is cells
    {
      //cout << "GetMeshMetaData Cells Test" << endl;
      metadata->MeshName = "cells";
      metadata->MeshType = VTK_MULTIBLOCK_DATA_SET; //VTK_POLY_DATA;
      metadata->BlockType = VTK_POLY_DATA;
      metadata->CoordinateType = VTK_DOUBLE;
      metadata->NumBlocks = nRanks;
      metadata->NumBlocksLocal = {1};
      metadata->NumGhostCells = 0;
      metadata->NumArrays = 0;
      metadata->StaticMesh = 0;  

      if (metadata->Flags.BlockExtentsSet())
      {
        //SENSEI_WARNING("lammps data adaptor. Flags.BlockExtentsSet()")
        
        // fixme
        // There should be no extent for a PolyData, but ADIOS2 needs this
        std::array<int,6> ext = { 0, 0, 0, 0, 0, 0};
        metadata->Extent = std::move(ext);
        metadata->BlockExtents.reserve(1);	// One block per rank
        metadata->BlockExtents.emplace_back(std::move(ext));
      }
      
      if (metadata->Flags.BlockDecompSet())
      {
        metadata->BlockOwner.push_back(rank);
        metadata->BlockIds.push_back(rank);
      }
    
      //We use nanglelist for BlockNumCells because it give the number of triangles on a given processor
      metadata->BlockNumCells.push_back(this->Internals->nanglelist);
      metadata->BlockNumPoints.push_back(this->Internals->nlocal + this->Internals->nghost );
      metadata->BlockCellArraySize.push_back(0);
    }
    else if(id == 1) // id == 1 is fluid
    {
      metadata->MeshName = "fluid"; 
      metadata->MeshType = VTK_MULTIBLOCK_DATA_SET;
      metadata->BlockType= VTK_IMAGE_DATA; 
      metadata->CoordinateType = VTK_DOUBLE;
      metadata->NumBlocks = nRanks;
      metadata->NumBlocksLocal = {1}; 
      metadata->NumArrays=3;
      metadata->ArrayName = {"velocity","vorticity","velocityNorm"};
      metadata->ArrayComponents = {3, 3, 1}; 
      metadata->ArrayType = {VTK_DOUBLE, VTK_DOUBLE, VTK_DOUBLE};
      metadata->ArrayCentering = {vtkDataObject::POINT, vtkDataObject::POINT, vtkDataObject::POINT};
      metadata->StaticMesh = 1; 

      if (metadata->Flags.BlockDecompSet())
      {
        metadata->BlockOwner.push_back(rank);
        metadata->BlockIds.push_back(rank);
      }

      if (metadata->Flags.BlockExtentsSet())
      {
        //SENSEI_WARNING("lammps data adaptor. Flags.BlockExtentsSet()")
        
        // fixme
        // There should be no extent for a PolyData, but ADIOS2 needs this
        std::array<int,6> ext = { 0, nx, 0, ny, 0, nz };
        std::array<int,6> blockext = { localExtents[0], localExtents[1], localExtents[2], localExtents[3], localExtents[4], localExtents[5]}; //XXX Changes 2/23/22
        metadata->Extent = std::move(ext);
        metadata->BlockExtents.reserve(1);	// One block per rank
        metadata->BlockExtents.emplace_back(std::move(blockext)); //XXX We have to figure out the local numbers for block ext
      }

      metadata->BlockNumCells.push_back(nlx * nly * nlz * 3); //XXX Changed 2/23/22
      metadata->BlockNumPoints.push_back(nlx * nly * nlz * 3); //XXX Changed 2/23/22
      metadata->BlockCellArraySize.push_back(0); 
    }
    else
    {
      SENSEI_ERROR("MeshMetaData Error: id value does not exist")
    }
    return 0;
  }
  //----------------------------------------------------------------------
  int LPDataAdaptor::GetMesh(const std::string &meshName, bool structureOnly, vtkDataObject *&mesh)
  {
    int rank, size; 
    MPI_Comm_rank(this->GetCommunicator(), &rank);
    MPI_Comm_size(this->GetCommunicator(), &size);
    mesh = nullptr;
    //cout << "Calling GetMesh" << endl;
    if(meshName == "cells")
    {  
      DInternals& internals = (*this->Internals);

      vtkPolyData *pd = vtkPolyData::New();
      vtkMultiBlockDataSet *mb = vtkMultiBlockDataSet::New();

      if(!structureOnly)
      {
        vtkPoints *pts = vtkPoints::New();
        //pts->SetNumberOfPoints(internals.nlocal+internals.nghost);
        pts->SetData(internals.AtomPositions);
        vtkCellArray *Triangles = vtkCellArray::New();

        for (int i = 0 ; i < internals.nanglelist ; i++)
        {
          //cout << rank << " : Triangle Test :" << i << endl;
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

      mb->SetNumberOfBlocks(size);
      mb->SetBlock(rank,pd);
 
      mesh = mb;
    }

    else if(meshName == "fluid")
    {
      DInternals& internals = (*this->Internals); 
      vtkMultiBlockDataSet *mbfluid = vtkMultiBlockDataSet::New();
    
      //cout << "Inside get mesh " << meshName << endl;

      vtkImageData *FluidImageData = vtkImageData::New();
      FluidImageData->SetDimensions(internals.pb_nx, internals.pb_ny, internals.pb_nz); //XXX HERE IS WHERE WE NEED TO CHANGE DIMENSIONS
      
      //cout << internals.pb_nx << "," << internals.pb_ny << "," << internals.pb_nz << endl;
      /*
      FluidImageData->GetPointData()->AddArray(internals.pb_velocityDoubleArray);
      internals.pb_velocityDoubleArray->SetName("velocity");

      FluidImageData->GetPointData()->AddArray(internals.pb_vorticityDoubleArray);
      internals.pb_vorticityDoubleArray->SetName("vorticity"); 

      FluidImageData->GetPointData()->AddArray(internals.pb_velocityNormDoubleArray);
      internals.pb_velocityNormDoubleArray->SetName("velocityNorm");
      */

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
    //cout << "meshname: " << meshName<< "  ArrayName: " << arrayName << endl;
    int rank;
    MPI_Comm_rank(this->GetCommunicator(), &rank);
    if(meshName == "fluid")
    {
      DInternals& internals = (*this->Internals); 
      /*
      vtkImageData *velocity = dynamic_cast<vtkImageData*>(mesh); //XXX PROBABLY NEEDS TO BE vtkmultiblockdataset
      vtkImageData *vorticity = dynamic_cast<vtkImageData*>(mesh); //XXX extract image data and then add arrays
      vtkImageData *velocityNorm = dynamic_cast<vtkImageData*>(mesh); //XXX Once again, this should be one line, not three
      velocity->GetPointData()->AddArray(internals.pb_velocityDoubleArray);
      vorticity->GetPointData()->AddArray(internals.pb_vorticityDoubleArray); 
      velocityNorm->GetPointData()->AddArray(internals.pb_velocityNormDoubleArray);
      */
      vtkMultiBlockDataSet *mbfluid = dynamic_cast<vtkMultiBlockDataSet*>(mesh); 
      if(!mbfluid)   
      {
        SENSEI_ERROR("unexpected mesh type "<< (mesh ? mesh->GetClassName() : "nullptr"))
        return -1;
      }
      vtkImageData *FluidImageData = (vtkImageData*)mbfluid->GetBlock(rank);
      if(!FluidImageData)
      {
        SENSEI_ERROR("Cannot Get Block in LPDataAdaptor::AddArray")
        return -1;
      }
      if(arrayName == "velocity")
      {
        FluidImageData->GetPointData()->AddArray(internals.pb_velocityDoubleArray);
      }
      else if(arrayName == "vorticity")
      {
        FluidImageData->GetPointData()->AddArray(internals.pb_vorticityDoubleArray);
      }
      else if(arrayName == "velocityNorm")
      {
        FluidImageData->GetPointData()->AddArray(internals.pb_velocityNormDoubleArray);
      }
      else
      {
        SENSEI_ERROR("Array name for Palabos AddArray does not exist in LPDataAdaptor::AddArray")
        return -1;
      }
       
    }
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
