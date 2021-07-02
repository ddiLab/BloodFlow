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


void Initialize(MPI_Comm world){
   cout << "SENSEI: Initialize()" << endl;
   GlobalDataAdaptor = vtkSmartPointer<senseiLP::LPDataAdaptor>::New();
   GlobalDataAdaptor->Initialize();
   //senseiLP::LPDataAdaptor Adaptor;
   //Adaptor.Initialize();
   
}
void SetData(){
   cout << "SENSEI: SetData()" << endl;
}
void Analyze(){
   cout << "SENSEI: Analyze()" << endl;
}
void Finalize(){
   cout << "SENSEI: Finalize()" << endl;
}
}

