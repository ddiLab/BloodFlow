# These are notes of interest on palabos code

1. `lattice.periodicity().toggle(2,true);` This command turns the boundaries in the z direction to periodic. For the coupling of LAMMPS and PALABOS to work, the periodic settings for both codes must be consistent.

2. ` IncomprFlowParam<T> parameters(` This class is found in palabos/src/core/units.h. There are several classes for different kinds of flow (ex. compressible flow). Additionally, these classes are overloaded to allow variance in the input parameters. There are a list of functions in these classes, like finding the dimension length in nodes (getNX, getNy, getNz)

3. `MultiBlockLattice3D<T, DESCRIPTOR>` This is found in palabos/src/multiBlock/multiBlockLattice3d.h. A complex Data structure useful for implementing sparse domains. While what goes on behind the scenes is more complicated, the use of MultiBlock works the same as the other data structures found in Palabos, so it is almost exclusively used in end user code. 
