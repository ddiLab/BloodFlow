#pragma once
#include <mpi.h>
#include <string>


namespace Bridge
{
  void Initialize(MPI_Comm world, const std::string& config_file);
  void SetData(double **x, long ntimestep, int nghost, int nlocal, double xsublo, double xsubhi, 
               double ysublo, double ysubhi, double zsublo,
               double zsubhi);
  /*
 long ntimestep, int nlocal, int *id, )
 LAMMPS:
 nlocal: BloodFlow/rbc/angle_rbc.cpp (391) atom->nlocal
 ntimestep: BloodFlow/rbc/angle_rbc.cpp (407) update->ntimestep Are these the same as used in SetData?
 **x : BloodFlow/rbc/angle_rbc.cpp (719) **x = atom->x;
 **f : BloodFlow/rbc/angle_rbc.cpp (122) **f = atom->f;
 *molecule : BloodFlow/rbc/angle_rbc.cpp (78) tagint *molecule = atom->molecule;
 What would *id be?
 **v : BloodFlow/ibm/ibm3D.hh (863) **v = wrapper.lmp->atom->v;
 *mask : BloodFlow/ibm/ibm3D.hh (866) *mask = wrapper.lmp->atom->mask; Somehow groups atoms up to distinguish between different bloodcells/cancercells/platelets
 Do we need domain values? (lammps input domain given in topology file BloodFlow/examples/singleCell/in.cells)
 *type : atom_style? (molecular) From singleCell/in.lmp4cell
 bond_style? (wlc/pow) from singleCell/in.lmp4cell
 angle_style? (rbc) from in.lmp4cell
 dihedral_style (bend) from in.lmp4cell
 anglelist: angle_rbc.cpp (125) int **anglelist = neighbor->anglelist;
 nanglelist: int nanglelist = neighbor->nanglelist;
 nfix (found in ibm3D.hh) 
  */
  void Analyze();
  void Finalize();
}

