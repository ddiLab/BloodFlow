#include "Bridge.h"
#include "LPdataAdaptor.h"
#include <vtkSmartPointer.h>
#include <ConfigurableAnalysis.h>
#include <iostream>

using namespace std; //Added if you want to test something with a cout command

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
             double zsubhi, int **anglelist, int nanglelist)
{
   //cout << "SENSEI: SetData()" << endl;
   GlobalDataAdaptor->AddLAMMPSData(x, ntimestep, nghost, nlocal, xsublo, xsubhi, 
                                    ysublo, ysubhi, zsublo, zsubhi, anglelist, nanglelist);
   
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

