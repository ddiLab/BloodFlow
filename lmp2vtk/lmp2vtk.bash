 g++ -o lmp2vtk4s lmp2vtk4s.cpp #Creates an executable named lmp2vtk4s
 ./lmp2vtk4s -p dump.rbc.xyz -t in.cells -w vtk/cell -s cell -c 3 -z 1
#Information on parameters can befound in lmp2vtk4s.cpp
