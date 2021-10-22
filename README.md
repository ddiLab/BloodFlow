# Using the BloodFlow Repository

Palabos + LAMMPS + SENSEI Integration for in-situ visualization is the goal for this repository.
Dr. Jifu Tan had successfully coupled Palabos and LAMMPS to simulate the flow of blood cells within plasma. 
BloodFlow has several directories and text files, each with a specific purpose. These are listed below

1. PALABOS.md : This text file includes lines of code within embolism.cpp that are of interest.
It attempts to explain code developed by Palabos that someone new to the software may not understand, even when being proficient in c++.

2. SENSEI.md : Instructions on how to build SENSEI on Cooley and how to run an example called oscillator\
   [SenseiPersonal.md](BloodFlow/SenseiPersonal.md)

3. examples (directory) : Several examples exist here to begin getting familiar to running such code. 
Embolism contains a README file with instructions for running its code on both Cooley and one's personnal computer.
Once embolism is made to work on Cooley, singleCell and Training (five blood cells) should be relatively easy to move to Cooley. They use the same code
as embolism and only change a few lines and have different topology files.

4. ibm (directory) : Newer users won't need to touch this directory until they need to dig deeper into understanding how LAMMPS and Palabos are coupled.
All of the coupling code is located here. If the class for a called object in embolism.cpp can't be found in the palabos/src, it most likely is located here.

5. lmp2vtk (directory) : The example codes output a .xyz file for the particles. The code in here converts this file to .vtk, which can be visualized in Paraview. lmp2vtk.bash file has a list of commands to use that can easily convert .xyz to .vtk. 

6. rbc (directory) : Multiple files are stored in here that are needed for the example simulations. Instruction on what to do with them is located 
in the README file for each example. NOTE: Training example uses the same files as singleCell.

7. sites (directory) : cooley.cmake is basically a bash file called when using the cmake command on Cooley (listed in embolism's README). 
The paths listed in the file will need to be changed to reflect one's personnal paths.
