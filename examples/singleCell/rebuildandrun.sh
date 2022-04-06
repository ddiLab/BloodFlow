#!/bin/sh
clear

cd build

echo 'make -j8'

make -j 8 

cd ..

mpirun -n 4 cellFlow in.lmp4cell

echo 'Transferring Paraview Data to:'
echo '/mnt/c/Users/Connor/Documents/post'

cd vtk_output

#rm /mnt/c/Users/Connor/Documents/post/
mv * /mnt/c/Users/Connor/Documents/post3

cd .. 

echo 'Data Transferred successfully'

