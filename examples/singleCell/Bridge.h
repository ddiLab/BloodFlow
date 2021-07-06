#pragma once
#include <mpi.h>
#include <string>


namespace Bridge
{
  void Initialize(MPI_Comm world, const std::string& config_file);
  void SetData();
  void Analyze();
  void Finalize();
}

