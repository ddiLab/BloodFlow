# include <iostream>
# include <fstream>
# include <sstream>
# include <unistd.h>
# include <string>
# include <stdlib.h>
# include <vector>

# include <omp.h>

using namespace std;

void readXYZ(string fp_name, vector<int> &time, vector< vector<double> > &pos);
void readTopo(string fp_name, vector< vector<int> > &face);
void writeVTKsolid(vector<int> &time, vector< vector<double> > &pos, vector< vector<int> > &face);
int main(int argc, char **argv)
{
  int opt;
  string fp_name,ftp_name;
  while ((opt =getopt(argc,argv,"p:t:")) != -1){
    switch(opt){
      case 'p':
        fp_name =string(optarg);
        cout<<"file name "<<fp_name<<endl;
        break;
      case 't':
        ftp_name = string(optarg);
        break;
      default:
        cerr<<"error"<<endl;
        return 0;
    }
  }
  int id;
  #pragma omp parallel for
  for (int i=0;i<10;i++)
    for(int j=0;j<10;j++)
      for (int k=0;k<10;k++){
        int a = i+j+k;
        id = omp_get_thread_num();
        #pragma omp critical
        cout<<"id "<<id<<" i,j,k "<<i<<" "<<j<<" "<<k<<" a= "<<a<<endl;
  }
  //#pragma omp parallel
  //cout<<"hello"<<endl;

  vector <int> time;
  vector < vector<double> > pos;
  vector < vector<int> > face;
  readXYZ(fp_name, time, pos);
  readTopo(ftp_name,face);
  writeVTKsolid(time,pos,face);

  return 0;
}

void readXYZ(string fp_name, vector<int> &time, vector< vector<double> > &pos){
  ifstream fp;
  vector <double> row(3,0.0);
  fp.open(fp_name.c_str());
 
  int n;
  int tmp,timestep;
  string buf;
  while(fp>>n){

    fp>>buf>>buf>>buf;
    timestep = atoi(buf.c_str());
    time.push_back(timestep);
    
    for (int i=0;i<n;i++){
      fp>>tmp>>row[0]>>row[1]>>row[2];
      pos.push_back(row);
    }
  }
  fp.close();
}

void readTopo(string ftp_name, vector< vector<int> > &face){
  fstream ftp;
  ftp.open(ftp_name.c_str());
  vector<int> row(3,0);
  int na,count,tmp;
  count=0;
  string buf;
  size_t loc;
  while (getline(ftp,buf)){
    loc = buf.find("angles");
    if (loc != string::npos)  {
      na = atoi(buf.substr(0,loc).c_str());
    }
    loc = buf.find("Angles");
    if (loc != string::npos)  {
      for (int i=0;i<na;i++){
        ftp>>tmp>>tmp>>row[0]>>row[1]>>row[2];
        face.push_back(row);
      }
    }
  }
  ftp.close();
}

void writeVTKsolid(vector<int> &time, vector< vector<double> > &pos, vector< vector<int> > &face){
      int i,j,k;
      int nn = pos.size()/time.size();
      int na = face.size()/time.size();
      for (i=0;i<time.size();i++){
        stringstream output_filename;
        output_filename << "vtk_solid/cell_t" << time[i] << ".vtk";
    
        ofstream of;
        of.open(output_filename.str().c_str());
        of << "# vtk DataFile Version 3.0\n";
        of << "Cells\n";
        of << "ASCII\n";
        of << "DATASET POLYDATA\n";
        
        of << "POINTS "<<nn<< " float\n";
        for (j=0;j<nn;j++)
          of << pos[i*nn+j][0]<<" "<< pos[i*nn+j][1]<<" "<<pos[i*nn+j][2]<<endl;

        of << "POLYGONS "<<na<<" "<<4*na<<endl;
        for (j=0;j<na;j++)
          of <<"3 "<<face[i*na+j][0]<<" "<<face[i*na+j][1]<<" "<<face[i*na+j][2]<<endl;

        of.close();
      }
     
}
