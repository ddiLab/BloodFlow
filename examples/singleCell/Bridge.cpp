#include "Bridge.h"
#include "LPdataAdaptor.h"
#include <vtkSmartPointer.h>
#include <ConfigurableAnalysis.h>
//#include <libIS/is_sim.h>
#include <iostream>

using namespace std;
using namespace plb; 
namespace Bridge
{
   static vtkSmartPointer<senseiLP::LPDataAdaptor>  GlobalDataAdaptor;
   static vtkSmartPointer<sensei::ConfigurableAnalysis> GlobalAnalysisAdaptor;

void Initialize(MPI_Comm world, const std::string& config_file){
   //cout << "SENSEI: Initialize()" << endl;
   GlobalDataAdaptor = vtkSmartPointer<senseiLP::LPDataAdaptor>::New();
   GlobalDataAdaptor->Initialize();
   GlobalDataAdaptor->SetCommunicator(world);
   GlobalDataAdaptor->SetDataTimeStep(-1);

   GlobalAnalysisAdaptor = vtkSmartPointer<sensei::ConfigurableAnalysis>::New();
   GlobalAnalysisAdaptor->Initialize(config_file);
   //cout << "SENSEI:" << config_file << endl;   
}
void SetData(double **x, long ntimestep, int nghost, int nlocal, double xsublo, 
             double xsubhi, double ysublo, double ysubhi, 
             double zsublo, double zsubhi, 
	     MultiTensorField3D<double, 3> velocityArray,
	     MultiTensorField3D<double, 3> vorticityArray,
	     MultiScalarField3D<double> velocityNormArray)
{
   //cout << "SENSEI: SetData()" << endl;
  GlobalDataAdaptor->AddLAMMPSData(x, ntimestep, nghost, nlocal, xsublo, xsubhi, 
                                    ysublo, ysubhi, zsublo, zsubhi);

  int nx = 20, ny = 20, nz = 40;

 // MultiTensorField3D<double, 3> velocityArray=*computeVelocity(lattice);
 // MultiTensorField3D<double, 3> vorticityArray=*computeVorticity(*computeVelocity(lattice));
 // MultiScalarField3D<double> velocityNormArray=*computeVelocityNorm(lattice);

  vtkDoubleArray *velocityDoubleArray = vtkDoubleArray::New();
  vtkDoubleArray *vorticityDoubleArray = vtkDoubleArray::New();
  vtkDoubleArray *velocityNormDoubleArray = vtkDoubleArray::New();

  velocityDoubleArray->SetNumberOfComponents(3);
  velocityDoubleArray->SetNumberOfTuples(nx * ny * nz);

  vorticityDoubleArray->SetNumberOfComponents(3);
  vorticityDoubleArray->SetNumberOfTuples(nx * ny * nz);

   velocityNormDoubleArray->SetNumberOfComponents(1);
   velocityNormDoubleArray->SetNumberOfTuples(nx * ny * nz);
// Add comp and tuples  
// cout << "VEl : " << velocityArray << endl; 
  for (int i=0; i<nz; i++)
  {
    for (int j=0; j<ny; j++)
    {
     for (int k=0; k<nx; k++)
      {
        Array<double,3> vel = velocityArray.get(k,j,i); 
        Array<double,3> vor = vorticityArray.get(k,j,i);
        double norm = velocityNormArray.get(k,j,i);

        int index = j * nx + k + i * nx * ny;

        velocityDoubleArray->SetTuple3(index,vel[0],vel[1],vel[2]);
        vorticityDoubleArray->SetTuple3(index,vor[0],vor[1],vor[2]);
        velocityNormDoubleArray->SetTuple1(index,norm);
      }
    }
  }

 double *values;
  for(int i = 0; i < 1600 ; i++)
   {
     values = velocityDoubleArray->GetTuple3(i);
  //   cout << values[0] << " " << values[1] << " " << values[2] << std::endl;
   }
  
  GlobalDataAdaptor->AddPalabosData(velocityDoubleArray, vorticityDoubleArray, velocityNormDoubleArray); 
   
}
void Analyze(long ntimestep)
{
   GlobalDataAdaptor->SetDataTimeStep(ntimestep);
   GlobalDataAdaptor->SetDataTime(ntimestep);
   GlobalAnalysisAdaptor->Execute(GlobalDataAdaptor.GetPointer());
   GlobalDataAdaptor->ReleaseData();
}
void Finalize()
   {
   cout << "SENSEI: Finalize()" << endl;
   GlobalAnalysisAdaptor->Finalize();
   GlobalAnalysisAdaptor = NULL;
   GlobalDataAdaptor = NULL;
   }
}

