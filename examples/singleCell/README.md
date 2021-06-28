 This is an example for blood cell model. 
you can either run it with just lammps, or couple it with fluid using palabos. 
* copy `bond_wlc_pow.h/.cpp, angle_rbc.h/.cpp dihedral_bend.h/.cpp` from subfolder `rbc` into folder `lammps/src`
## only lammps  
* compile lammps as a standard program (not a library) in `lammps/src`: `make mpi` 
* to run the code: `mpirun -np 2 lmp_mpi < in.lmp4cell`
You need to enable `fix nve` and other settings such as initial velocity, force field, etc. in file *in.lmp4cell*, otherwise there is no integrator for molecular simulation, e.g., the cell won't move. 
## lammps with palabos
* compile lammps as a a library in `lammps/src`: `make mpi mode=lib` 
* in `example/singleCell`, type the following commands
``` 
mkdir build
cd build
cmake ..
make
cd ..
```
here you may want to change the EXECUTABLE_NAME in `CMakelist.txt`, here, 
```
set(EXECUTABLE_NAME "cellFlow")
```
before you type `cmake ..`
* in folder `singleCell`, type the following commands to run the code:
```
mpirun -np 2 cellFlow in.lmp4cell
```
You may need to turn off `fix nve` in file *in.lmp4cell*, as the cell position will be updated by the immersed boundary algorithm. You may also want to create a `tmp` folder to save fluid data. 

