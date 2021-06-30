#include <iostream>
#include "Bridge.h"
#include "LPdataAdaptor.h"

using namespace std;

void Bridge::Initialize(MPI_Comm world){
   cout << "SENSEI: Initialize()" << endl;
   GlobalDataAdaptor = senseiLP::LPdataAdaptor->Initialize();
}
void SetData(){
   cout << "SENSEI: SetData()" << endl;
}
void Bridge::Analyze(){
   cout << "SENSEI: Analyze()" << endl;
}
void Bridge::Finalize(){
   cout << "SENSEI: Finalize()" << endl;
}


