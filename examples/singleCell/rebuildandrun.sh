#!/bin/sh
clear

cd build

echo 'make -j8'

make -j 8 

cd ..

#mpirun -n 4 cellFlow in.lmp4cell



