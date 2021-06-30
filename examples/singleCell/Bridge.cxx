#include <iostream>
#include "Bridge.h"
#include "LPdataAdaptor.h"

using namespace std;

namespace Bridge
{
void Initialize(MPI_Comm world){
   cout << "SENSEI: Initialize()" << endl;
   senseiLP::LPDataAdaptor Adaptor();
   Adaptor.Initialize();
   
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

