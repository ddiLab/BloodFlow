#pragma once
#include <mpi.h>
#include <string>
#include "palabos3D.h"
#include "palabos3D.hh"
using namespace plb; 
namespace Bridge
{
  void Initialize(MPI_Comm world, const std::string& config_file);
  void SetData(double **x, long ntimestep, int nghost, 
               int nlocal, double xsublo, double xsubhi, 
               double ysublo, double ysubhi, double zsublo,
               double zsubhi, int **anglelist, int nanglelist,
	           MultiTensorField3D<double, 3> velocityDoubleArray, 
	           MultiTensorField3D<double, 3> vorticityDoubleArray, 
	           MultiScalarField3D<double> velocityNormDoubleArray,
             int nx, int ny, int nz, Box3D domainBox); 
  void Analyze(long ntimestep); 
  void Finalize();
}

