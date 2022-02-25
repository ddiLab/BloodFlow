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
	         MultiScalarField3D<double> velocityNormArray,
           int nx, int ny, int nz, Box3D domainBox)
{
  GlobalDataAdaptor->AddLAMMPSData(x, ntimestep, nghost, nlocal, xsublo, xsubhi,
                                   ysublo, ysubhi, zsublo, zsubhi, anglelist, nanglelist);
  

  vtkDoubleArray *velocityDoubleArray = vtkDoubleArray::New();
  vtkDoubleArray *vorticityDoubleArray = vtkDoubleArray::New();
  vtkDoubleArray *velocityNormDoubleArray = vtkDoubleArray::New();

//XXXNew local values added with domainBox 2/23/22*****
  int nlx = domainBox.getNx(); 
  int nly = domainBox.getNy();
  int nlz = domainBox.getNz();
//*****************************************************

  velocityDoubleArray->SetNumberOfComponents(3);
  velocityDoubleArray->SetNumberOfTuples(nlx * nly * nlz); 

  vorticityDoubleArray->SetNumberOfComponents(3);
  vorticityDoubleArray->SetNumberOfTuples(nlx * nly * nlz);

   velocityNormDoubleArray->SetNumberOfComponents(1);
   velocityNormDoubleArray->SetNumberOfTuples(nlx * nly * nlz);

//XXX Need to convert this to zero copy: FUTURE WORK
//XXX Needs to be restructured to copy only the local domain
plint myrank = global::mpi().getRank();
cout << "RANK: " << myrank <<" NLX: " << nlx << " NLY: " << nly << " NLZ: " << nlz << endl;
  for (int i=0; i<nlz; i++)
  {
    for (int j=0; j<nly; j++)
    {
     for (int k=0; k<nlx; k++)
      {
        Array<double,3> vel = velocityArray.get(k,j,i); 
        Array<double,3> vor = vorticityArray.get(k,j,i);
        double norm = velocityNormArray.get(k,j,i);

        int index = j * nlx + k + i * nlx * nly;

        velocityDoubleArray->SetTuple3(index,vel[0],vel[1],vel[2]);
        vorticityDoubleArray->SetTuple3(index,vor[0],vor[1],vor[2]);
        velocityNormDoubleArray->SetTuple1(index,norm);
        if(myrank == 1 && i >= 9)
        {
          cout << i << " Velocity: " << velocityArray.getNx() << " " << velocityArray.getNy() << " " << velocityArray.getNz()<<endl;
          //cout << "X: " << k << " Y: " << j << " Z: " << i << endl;
        }
      }
    }
  }

  
 GlobalDataAdaptor->AddPalabosData(velocityDoubleArray, vorticityDoubleArray, velocityNormDoubleArray, nx, ny, nz, domainBox); 
 cout <<"RANK: " << myrank <<" LOOPING IS DONE"<< endl;  
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

