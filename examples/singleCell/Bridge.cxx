#include <iostream>
#include "Bridge.h"

using namespace std;

void Bridge::Initialize(MPI_Comm world){
   cout << "SENSEI: Initialize()" << endl;
}
void Bridge::Analyze(){
   cout << "SENSEI: Analyze()" << endl;
}
void Bridge::Finalize(){
   cout << "SENSEI: Finalize()" << endl;
}


