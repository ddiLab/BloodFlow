# This is example for blood cell model. 
* copy `bond_wlc_pow.h/.cpp, angle_rbc.h/.cpp dihedral_bend.h/.cpp` from subfolder `rbc` into folder `lammps/src`
* compile lammps as a standard program (not a library) in `lammps/src`: `make mpi` 
* to run the code: `mpirun -np 2 lmp_mpi < in.lmp4cell`

