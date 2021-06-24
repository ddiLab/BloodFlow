# How to run the embolism code (Personnal Computer)
She embolism example is tested with the following versions:
- palabos version downloaded in March 10th, 2021. [Palabos](https://gitlab.com/unigespc/palabos)
- lammps version downloaded in Feb. 2021. [LAMMPS](https://github.com/lammps/lammps)

**Steps to compile the code:** 
1. copy fix_fcm.h/.cpp fix_rls.h/.cpp fix_activate_platelet.h/cpp in rbc folder into your lammps/src directory. Please don't copy other files in rbc folder as they are not compatible with the latest lammps version yet.  
2. set up the correct file path in CMakeLists.txt in examples/embolism. Those places you need to change are commented with lines starting with "Jifu: ..."
3. make sure you have cmake (version 2.8.12.2), make (GNU Make 3.82), openmpi (Open MPI 4.0.3) installed. My tested version for each are listed in the parenthesis. Other versions may work but I haven't tested yet. 
4. compile lammps as a library, go to folder lammps/src, type `make mpi mode=lib` You may need to append `-std=c++11` flags in CCFLAGS in Makefile.mpi under lammps/src/MAKE folder.
5. go to embolism/build, type `cmake ..`
`make`
then go to embolism, you should be able to see the executable embolism generated. You can run it with `mpirun -np 4 embolism in.embolism`. You need to make sure you run it in the embolism folder, as in.embolism requires a path to lammps data file in.platelet.

# How to run the embolism code (Cooley)
The embolism example is tested with the following versions:
- palabos version downloaded in May 16, 2021. (https://gitlab.com/unigespc/palabos)\
`git clone git@gitlab.com:unigespc/palabos.git` (clone with ssh)\
`git clone https://gitlab.com/unigespc/palabos.git` (clone with https)\
`git checkout e960674cea38515ae3749218c314a9e1a3c6c140` (update to the version we know works)
- lammps version downloaded in May 16, 2021. (https://github.com/lammps/lammps)\
`git clone git@github.com:lammps/lammps.git` (clone with ssh)\
`git clone https://github.com/lammps/lammps.git` (clone with https)\
`git checkout e498e8ad7f24fd7ff87313670db7873703c1fd3f` (update to the version we know works)

**Steps to compile the code:**
1. Clone the ddilab/BloodFlow repository. In BloodFlow/sites/cooley.cmake, there are a few file paths that need to be modified.
`git clone git@github.com:ddiLab/BloodFlow.git` (clone with ssh)
`git clone https://github.com/ddiLab/BloodFlow.git` (clone with https)
2. The embolism.sh executable file needs to have a directory path updated. This file is in BloodFlow/examples/embolism.
3. Compile lammps as a library, go to folder lammps/src, type `make mpi mode=lib`. You may need to append `-std=c++11` flags in CCFLAGS in Makefile.mpi under lammps/src/MAKE folder. When compiling compiling the executable file later on, an error might occur complaining about package that is needed in lammps (I had to install the MC package). Enter the lammps/src directory and use the command `make yes-<packagename>`, remake lammps as a library, and rebuild the executable.
4. go to embolism/build and use the command: `cmake -C /path/to/BloodFlow/sites/cooley.cmake ../`
`make`
5. A executable named embolism should now exist in the embolism directory. Finally, use the following command while in the embolism directory to submit the code to run on a Cooley node:
`qsub -n 1 -t 10 -A <JOBNAME> ./embolism.sh`
The parameters of this command can be different, see https://www.alcf.anl.gov/support-center/cooley/submitting-jobs-cooley
