#pragma once
#include <mpi.h>
#include <string>


namespace Bridge
{
  void Initialize(MPI_Comm world, const std::string& config_file);
  void SetData();
  /*
 long ntimestep, int nlocal, int *id, )
 LAMMPS:
 nlocal: angle_rbp(391) atom->nlocal
 ntimestep: angle_rbc(407) update->ntimestep
  */
  void Analyze();
  void Finalize();
}

