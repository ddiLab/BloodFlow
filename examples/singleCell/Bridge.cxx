#include <iostream>
#include "Bridge.h"
#include "LPdataAdaptor.h"

using namespace std;


void Bridge::Initialize(MPI_Comm world){
   cout << "SENSEI: Initialize()" << endl;
   senseiLP::LPDataAdaptor::Initialize();
   
}
void Bridge::SetData(){
   cout << "SENSEI: SetData()" << endl;
}
void Bridge::Analyze(){
   cout << "SENSEI: Analyze()" << endl;
}
void Bridge::Finalize(){
   cout << "SENSEI: Finalize()" << endl;
}


