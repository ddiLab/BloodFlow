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
	         TensorField3D<double, 3> velocityArray,
	         TensorField3D<double, 3> vorticityArray,
	         ScalarField3D<double> velocityNormArray,
           int nx, int ny, int nz, Box3D domainBox)
{
  GlobalDataAdaptor->AddLAMMPSData(x, ntimestep, nghost, nlocal, xsublo, xsubhi,
                                   ysublo, ysubhi, zsublo, zsubhi, anglelist, nanglelist);
  

  vtkDoubleArray *velocityDoubleArray = vtkDoubleArray::New();
  vtkDoubleArray *vorticityDoubleArray = vtkDoubleArray::New();
  vtkDoubleArray *velocityNormDoubleArray = vtkDoubleArray::New();

//XXXNew local values added with domainBox 2/23/22*****
  int nlx = velocityArray.getNx(); 
  int nly = velocityArray.getNy();
  int nlz = velocityArray.getNz();
  plint myrank = global::mpi().getRank();
  cout << "Rank: " << myrank << "Extent: " << nlx << endl;
//*****************************************************
  velocityDoubleArray->SetNumberOfComponents(3);
  velocityDoubleArray->SetNumberOfTuples(nlx * nly * nlz); 

  vorticityDoubleArray->SetNumberOfComponents(3);
  vorticityDoubleArray->SetNumberOfTuples(nlx * nly * nlz);

   velocityNormDoubleArray->SetNumberOfComponents(1);
   velocityNormDoubleArray->SetNumberOfTuples(nlx * nly * nlz);

   plint EW = 4;

//XXX Need to convert this to zero copy: FUTURE WORK

  for (int k=0+EW; k<nlz-EW; k++)
  {
    for (int j=0+EW; j<nly-EW; j++)
    {
     for (int i=0+EW; i<nlx-EW; i++)
      {
        Array<double,3> vel = velocityArray.get(i,j,k);
        Array<double,3> vor = vorticityArray.get(i,j,k);
        double norm = velocityNormArray.get(i,j,k);
        int index = (j-EW) * (nlx-2*EW) + (i-EW) + (k-EW) * (nlx-2*EW) * (nly-2*EW);
        velocityDoubleArray->SetTuple3(index,vel[0],vel[1],vel[2]);
        vorticityDoubleArray->SetTuple3(index,vor[0],vor[1],vor[2]);
        velocityNormDoubleArray->SetTuple1(index,norm);
      }
    }
  }

 GlobalDataAdaptor->AddPalabosData(velocityDoubleArray, vorticityDoubleArray, velocityNormDoubleArray, nx, ny, nz, domainBox); 
 
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

