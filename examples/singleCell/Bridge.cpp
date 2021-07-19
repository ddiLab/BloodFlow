#include "Bridge.h"
#include "LPdataAdaptor.h"
#include <vtkSmartPointer.h>
#include <ConfigurableAnalysis.h>
//#include <libIS/is_sim.h>
#include <iostream>

using namespace std;

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
void SetData(double **x, long ntimestep, int nghost, 
             int nlocal, double xsublo, double xsubhi, 
             double ysublo, double ysubhi, double zsublo, 
             double zsubhi)
{
   //cout << "SENSEI: SetData()" << endl;
   GlobalDataAdaptor->AddLAMMPSData(x, ntimestep, nghost, nlocal, xsublo, xsubhi, 
                                    ysublo, ysubhi, zsublo, zsubhi);
   
}
void Analyze(){
   //cout << "SENSEI: Analyze()" << endl;
   GlobalAnalysisAdaptor->Execute(GlobalDataAdaptor.GetPointer());
   GlobalDataAdaptor->ReleaseData();
}
void Finalize(){
   cout << "SENSEI: Finalize()" << endl;
}

}

