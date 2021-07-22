#include "Bridge.h"
#include "LPdataAdaptor.h"
#include <vtkSmartPointer.h>
#include <ConfigurableAnalysis.h>
#include <iostream>

using namespace std;
using namespace plb; 
namespace Bridge
{
   static vtkSmartPointer<senseiLP::LPDataAdaptor>  GlobalDataAdaptor;
   static vtkSmartPointer<sensei::ConfigurableAnalysis> GlobalAnalysisAdaptor;

void Initialize(MPI_Comm world, const std::string& config_file){
   
   GlobalDataAdaptor = vtkSmartPointer<senseiLP::LPDataAdaptor>::New();
   GlobalDataAdaptor->Initialize();
   GlobalDataAdaptor->SetCommunicator(world);
   GlobalDataAdaptor->SetDataTimeStep(-1);

   GlobalAnalysisAdaptor = vtkSmartPointer<sensei::ConfigurableAnalysis>::New();
   GlobalAnalysisAdaptor->Initialize(config_file);

}
void SetData(double **x, long ntimestep, int nghost, 
             int nlocal, double xsublo, double xsubhi, 
             double ysublo, double ysubhi, double zsublo, 
             double zsubhi, int **anglelist, int nanglelist, 
	         MultiTensorField3D<double, 3> velocityArray,
	         MultiTensorField3D<double, 3> vorticityArray,
	         MultiScalarField3D<double> velocityNormArray)
{
  GlobalDataAdaptor->AddLAMMPSData(x, ntimestep, nghost, nlocal, xsublo, xsubhi,
                                   ysublo, ysubhi, zsublo, zsubhi, anglelist, nanglelist);
  
  int nx = 20, ny = 20, nz = 40;


  vtkDoubleArray *velocityDoubleArray = vtkDoubleArray::New();
  vtkDoubleArray *vorticityDoubleArray = vtkDoubleArray::New();
  vtkDoubleArray *velocityNormDoubleArray = vtkDoubleArray::New();

  velocityDoubleArray->SetNumberOfComponents(3);
  velocityDoubleArray->SetNumberOfTuples(nx * ny * nz);

  vorticityDoubleArray->SetNumberOfComponents(3);
  vorticityDoubleArray->SetNumberOfTuples(nx * ny * nz);

   velocityNormDoubleArray->SetNumberOfComponents(1);
   velocityNormDoubleArray->SetNumberOfTuples(nx * ny * nz);

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
   GlobalAnalysisAdaptor->Finalize();
   GlobalAnalysisAdaptor = NULL;
   GlobalDataAdaptor = NULL;
   }
}

