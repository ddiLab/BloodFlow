#pragma once
#include <mpi.h>
#include <string>


namespace Bridge
{
  void Initialize(MPI_Comm world, const std::string& config_file);
  void SetData(double **x, long ntimestep, int nghost, int nlocal, double xsublo, double xsubhi, 
               double ysublo, double ysubhi, double zsublo,
               double zsubhi);
  void Analyze(long ntimestep);
  void Finalize();
}

